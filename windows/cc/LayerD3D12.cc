#include <LayerD3D12.hh>
#include <WindowWin32.hh>
#include <AppWin32.hh>
#include <D3D12/DX12Fence.hh>
#include <D3D12/DX12Device.hh>
#include <D3D12/DX12SwapChain.hh>
#include <D3D12/DX12CommandQueue.hh>
#include <impl/Library.hh>

void jwm::LayerD3D12::attach(WindowWin32 *window) {
    assert(!_window);

    AppWin32& app = AppWin32::getInstance();

    if (!window) {
        app.sendError("Passed null WindowWin32 object to attach");
        return;
    }

    _window = jwm::ref(window);

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
    _dx12swapChain = std::make_unique<DX12SwapChain>(_window, *_dx12commandQueue);
    _dx12swapChain->create();

    // Fence used to synchronize commands submission and back buffers presentation
    _dx12fence = std::make_unique<DX12Fence>(*_dx12device);

    // Register callback to track window events
    _callbackID = _window->addEventListener([this](WindowWin32::Event event){
        switch (event) {
            case WindowWin32::Event::SwitchContext:
                break;
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

    for (UINT i =  0; i < _dx12swapChain->getBuffersCount(); i++) {
        _frameCmdAllocs.push_back(_dx12commandQueue->createCommandAllocator());
        _frameCmdLists.push_back(_dx12commandQueue->createCommandList(_frameCmdAllocs.back()));
        _frameFenceValues.push_back(DX12Fence::INITIAL_VALUE);
    }

    _fenceValue = DX12Fence::INITIAL_VALUE;
    _backBufferIndex = _dx12swapChain->getCurrentBackBufferIndex();
    _tearingFeature = dx12Common.checkTearingFeature();
}

void jwm::LayerD3D12::resize(int width, int height) {
    _dx12commandQueue->WaitIdle(*_dx12fence, _fenceValue);
    _dx12swapChain->resize(width, height);

    UINT64 currentFrameFenceValue = _frameFenceValues[_backBufferIndex];
    _frameFenceValues.clear();
    _frameFenceValues.resize(_dx12swapChain->getBuffersCount(), currentFrameFenceValue);
}

void jwm::LayerD3D12::swapBuffers() {
    static float t = 0.0f;
    static float dt = 0.01f;

    t += dt;

    auto& frameAlloc = _frameCmdAllocs[_backBufferIndex];
    auto& frameCmdList = _frameCmdLists[_backBufferIndex];

    frameAlloc->Reset();
    frameCmdList->Reset(frameAlloc.Get(), nullptr);

    _dx12swapChain->transitionLayout(
        frameCmdList,
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );

    _dx12swapChain->clearTarget(
        frameCmdList,
        std::sin(t) * 0.5f + 0.5f, 0.0f, std::cos(2.0f * t) * 0.5f + 0.5f, 1.0f
    );

    _dx12swapChain->transitionLayout(
        frameCmdList,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );

    THROW_IF_FAILED(frameCmdList->Close());

    _dx12commandQueue->Submit(frameCmdList);
    _frameFenceValues[_backBufferIndex] = _dx12commandQueue->Signal(*_dx12fence, _fenceValue);

    UINT syncInterval = _vsync == Vsync::Enable ? 1: 0;
    UINT presentationFlags = _tearingFeature && _vsync != Vsync::Enable ? DXGI_PRESENT_ALLOW_TEARING : 0;
    _dx12swapChain->present(syncInterval, presentationFlags);

    _backBufferIndex = _dx12swapChain->getCurrentBackBufferIndex();
    _dx12fence->waitFor(_frameFenceValues[_backBufferIndex]);
}

void jwm::LayerD3D12::close() {
    // Wait until full queue exec completion to safely release resources
    _dx12commandQueue->WaitIdle(*_dx12fence, _fenceValue);

    // Release in reverse order
    _frameCmdLists.clear();
    _frameCmdAllocs.clear();
    _dx12fence.reset();
    _dx12swapChain.reset();
    _dx12commandQueue.reset();
    _dx12device.reset();

    if (_window) {
        _window->removeEventListener(_callbackID);
        jwm::unref(&_window);
    }
}

void jwm::LayerD3D12::vsync(bool enable) {
    _vsync = enable? Vsync::Enable: Vsync::Disable;
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