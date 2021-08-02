#include <LayerD3D12.hh>
#include <WindowWin32.hh>
#include <AppWin32.hh>
#include <D3D12/DX12Fence.hh>
#include <D3D12/DX12Device.hh>
#include <D3D12/DX12SwapChain.hh>
#include <D3D12/DX12CommandQueue.hh>
#include <impl/Library.hh>

void jwm::LayerD3D12::attach(WindowWin32 *window) {
    assert(!_windowWin32);

    AppWin32& app = AppWin32::getInstance();

    if (!window) {
        app.sendError("Passed null WindowWin32 object to attach");
        return;
    }

    if (window->testFlag(WindowWin32::Flag::HasAttachedLayer)) {
        app.sendError("Window already has attached layer. Cannot re-attach.");
        return;
    }

    _windowWin32 = jwm::ref(window);
    _windowWin32->setFlag(WindowWin32::Flag::HasAttachedLayer);
    _windowWin32->setFlag(WindowWin32::Flag::HasLayerD3D);

    DX12Common& dx12Common = app.getDx12Common();

    if (!dx12Common.init()) {
        app.sendError("Failed to init DX12Common");
        return;
    }

    // Create unique device instance for layer
    _dx12device = std::make_unique<DX12Device>(dx12Common);

    // Direct (default) command queue, used to submit all commands
    _dx12commandQueue = std::make_unique<DX12CommandQueue>(
        DX12CommandQueue::DEFAULT_TYPE,
        DX12CommandQueue::DEFAULT_PRIORITY,
        *_dx12device
    );

    // Window swap chain (encapsulates all presentation/resize logic)
    _dx12swapChain = std::make_unique<DX12SwapChain>(_windowWin32, *_dx12commandQueue);
    _dx12swapChain->create();

    // Fence used to synchronize commands submission and back buffers presentation
    _dx12fence = std::make_unique<DX12Fence>(*_dx12device);

    // Register callback to track window events
    _callbackID = _windowWin32->addEventListener([this](WindowWin32::Event event){
        switch (event) {
            case WindowWin32::Event::SwapBuffers:
                swapBuffers();
                break;
            case WindowWin32::Event::EnableVsync:
                vsync(true);
                break;
            case WindowWin32::Event::DisableVsync:
                vsync(false);
                break;
            default:
                return;
        }
    });

    _fenceFrameValues.resize(_dx12swapChain->getBuffersCount(), DX12Fence::INITIAL_VALUE);
    _fenceValue = DX12Fence::INITIAL_VALUE;
    _tearingFeature = dx12Common.checkTearingFeature();
}

void jwm::LayerD3D12::resize(int width, int height) {
    _dx12commandQueue->waitIdle(*_dx12fence, _fenceValue);
    _dx12swapChain->resize(width, height);
}

void jwm::LayerD3D12::swapBuffers() {
    UINT bufferIndex = _dx12swapChain->getCurrentBackBufferIndex();
    UINT syncInterval = _vsync == Vsync::Enable ? 1: 0;
    UINT presentationFlags = _tearingFeature && _vsync == Vsync::Disable ? DXGI_PRESENT_ALLOW_TEARING : 0;
    _dx12swapChain->present(syncInterval, presentationFlags);
    _fenceFrameValues[bufferIndex] = _dx12commandQueue->Signal(*_dx12fence, _fenceValue);
}

void jwm::LayerD3D12::close() {
    // Wait until queue idle to safely release resources
    _dx12commandQueue->waitIdle(*_dx12fence, _fenceValue);

    // Release in reverse order
    _dx12fence.reset();
    _dx12swapChain.reset();
    _dx12commandQueue.reset();
    _dx12device.reset();

    if (_windowWin32) {
        _windowWin32->removeFlag(WindowWin32::Flag::HasAttachedLayer);
        _windowWin32->removeFlag(WindowWin32::Flag::HasLayerD3D);
        _windowWin32->setFlag(WindowWin32::Flag::RecreateForNextLayer);
        _windowWin32->removeEventListener(_callbackID);
        jwm::unref(&_windowWin32);
    }
}

void jwm::LayerD3D12::vsync(bool enable) {
    _vsync = enable? Vsync::Enable: Vsync::Disable;
}

IDXGIAdapter1 *jwm::LayerD3D12::getAdapterPtr() const {
    using namespace Microsoft::WRL;
    ComPtr<IDXGIAdapter1> adapter1;
    ComPtr<IDXGIAdapter4> adapter4 = _dx12device->getAdapterPtr();
    THROW_IF_FAILED(adapter4.As(&adapter1));
    return adapter1.Get();
}

ID3D12Device *jwm::LayerD3D12::getDevicePtr() const {
    using namespace Microsoft::WRL;
    ComPtr<ID3D12Device> device;
    ComPtr<ID3D12Device2> device2 = _dx12device->getDevicePtr();
    THROW_IF_FAILED(device2.As(&device));
    return device.Get();
}

ID3D12CommandQueue *jwm::LayerD3D12::getQueuePtr() const {
    return _dx12commandQueue->getQueuePtr().Get();
}

ID3D12Resource *jwm::LayerD3D12::getNextRenderTexture() const {
    using namespace Microsoft::WRL;
    UINT bufferIndex = _dx12swapChain->getCurrentBackBufferIndex();
    ComPtr<ID3D12Resource> texture = _dx12swapChain->getCurrentBackBuffer();
    _dx12fence->waitFor(_fenceFrameValues[bufferIndex]);
    return texture.Get();
}

jwm::DX12SwapChain &jwm::LayerD3D12::getSwapChain() const {
    return *_dx12swapChain;
}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_LayerD3D12__1nMake
        (JNIEnv* env, jclass jclass) {
    jwm::LayerD3D12* instance = new jwm::LayerD3D12();
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerD3D12__1nAttach
        (JNIEnv* env, jobject obj, jobject windowObj) {
    jwm::LayerD3D12* instance = reinterpret_cast<jwm::LayerD3D12*>(jwm::classes::Native::fromJava(env, obj));
    jwm::WindowWin32* window = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, windowObj));
    instance->attach(window);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerD3D12__1nReconfigure
        (JNIEnv* env, jobject obj, jint width, jint height) {
    // todo: what to do here?
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerD3D12__1nResize
        (JNIEnv* env, jobject obj, jint width, jint height) {
    jwm::LayerD3D12* instance = reinterpret_cast<jwm::LayerD3D12*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerD3D12__1nSwapBuffers
        (JNIEnv* env, jobject obj) {
    //jwm::LayerD3D12* instance = reinterpret_cast<jwm::LayerD3D12*>(jwm::classes::Native::fromJava(env, obj));
    //instance->swapBuffers();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerD3D12__1nClose
        (JNIEnv* env, jobject obj) {
    jwm::LayerD3D12* instance = reinterpret_cast<jwm::LayerD3D12*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_LayerD3D12_getAdapterPtr
        (JNIEnv* env, jobject obj) {
    jwm::LayerD3D12* instance = reinterpret_cast<jwm::LayerD3D12*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>(instance->getAdapterPtr());
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_LayerD3D12_getDevicePtr
        (JNIEnv* env, jobject obj) {
    jwm::LayerD3D12* instance = reinterpret_cast<jwm::LayerD3D12*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>(instance->getDevicePtr());
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_LayerD3D12_getQueuePtr
        (JNIEnv* env, jobject obj) {
    jwm::LayerD3D12* instance = reinterpret_cast<jwm::LayerD3D12*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>(instance->getQueuePtr());
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_LayerD3D12_getFormat
        (JNIEnv* env, jobject obj) {
    jwm::LayerD3D12* instance = reinterpret_cast<jwm::LayerD3D12*>(jwm::classes::Native::fromJava(env, obj));
    jwm::DX12SwapChain& swapChain = instance->getSwapChain();
    return static_cast<jint>(swapChain.getFormat());
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_LayerD3D12_getSampleCount
        (JNIEnv* env, jobject obj) {
    jwm::LayerD3D12* instance = reinterpret_cast<jwm::LayerD3D12*>(jwm::classes::Native::fromJava(env, obj));
    jwm::DX12SwapChain& swapChain = instance->getSwapChain();
    return static_cast<jint>(swapChain.getSamplesCount());
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_LayerD3D12_getLevelCount
        (JNIEnv* env, jobject obj) {
    jwm::LayerD3D12* instance = reinterpret_cast<jwm::LayerD3D12*>(jwm::classes::Native::fromJava(env, obj));
    jwm::DX12SwapChain& swapChain = instance->getSwapChain();
    return static_cast<jint>(swapChain.getLevelsCount());
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_LayerD3D12_nextDrawableTexturePtr
        (JNIEnv* env, jobject obj) {
    using namespace Microsoft::WRL;
    jwm::LayerD3D12* instance = reinterpret_cast<jwm::LayerD3D12*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>(instance->getNextRenderTexture());
}