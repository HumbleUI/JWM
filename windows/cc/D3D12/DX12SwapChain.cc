#include <D3D12/DX12SwapChain.hh>
#include <D3D12/DX12CommandQueue.hh>
#include <D3D12/DX12Device.hh>
#include <impl/RefCounted.hh>
#include <WindowWin32.hh>
#include <cassert>

jwm::DX12SwapChain::DX12SwapChain(WindowWin32 *window, DX12CommandQueue &queue)
    : _window(ref(window)), _dx12commandQueue(queue), _dx12device(queue.getDx12device()) {
    assert(queue.getCmdListType() == D3D12_COMMAND_LIST_TYPE_DIRECT);
}

jwm::DX12SwapChain::~DX12SwapChain() {
    resize(0, 0);
    present(0, 0);

    _verifyBufferCounters();
    _backBuffers.clear();
    _dxgiSwapChain.Reset();

    unref(&_window);
}

void jwm::DX12SwapChain::setBuffersCount(unsigned int buffersCount) {
    assert(buffersCount >= 2);
    assert(buffersCount <= 3);
    _buffersCount = buffersCount;
}

bool jwm::DX12SwapChain::create() {
    using namespace Microsoft::WRL;

    DX12Common& dx12Common = _dx12device.getDx12Common();
    ComPtr<IDXGIFactory4> dxgiFactory4 = dx12Common.getFactory();
    ComPtr<IDXGISwapChain1> swapChain1;

    UIRect rect = _window->getContentRect();
    _currentWidth = rect.getWidth();
    _currentHeight = rect.getHeight();

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.Width = _currentWidth;
    swapChainDesc.Height = _currentHeight;
    swapChainDesc.Format = _format;
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc = { SAMPLE_COUNT, LEVEL_COUNT };
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = _buffersCount;
    swapChainDesc.Scaling = _scaling;
    swapChainDesc.SwapEffect = _swapEffect;
    swapChainDesc.AlphaMode = _alphaMode;
    swapChainDesc.Flags =
        dx12Common.checkTearingFeature()?
        DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    HWND hWnd = _window->getHWnd();

    CHECK_IF_FAILED(dxgiFactory4->CreateSwapChainForHwnd(
        _dx12commandQueue.getQueuePtr().Get(),
        hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1)
    );

    if (!swapChain1)
        return false;

    // Disable the Alt+Enter fullscreen toggle feature
    CHECK_IF_FAILED(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
    CHECK_IF_FAILED(swapChain1.As(&_dxgiSwapChain));

    // Set transparent background color
    DXGI_RGBA color = { 0.0f, 0.0f, 0.0f, 0.0f };
    _dxgiSwapChain->SetBackgroundColor(&color);

    _updateRenderTargetViews();

    return true;
}

void jwm::DX12SwapChain::present(UINT syncInterval, UINT presentationFlags) {
    CHECK_IF_FAILED(_dxgiSwapChain->Present(syncInterval, presentationFlags));
}

void jwm::DX12SwapChain::resize(int newWidth, int newHeight) {
    if (_currentWidth != newWidth || _currentHeight != newHeight) {
        _currentWidth = std::max(1, newWidth);
        _currentHeight = std::max(1, newHeight);

        _verifyBufferCounters();
        _backBuffers.clear();

        DXGI_SWAP_CHAIN_DESC swapChainDesc{};
        CHECK_IF_FAILED(_dxgiSwapChain->GetDesc(&swapChainDesc));
        CHECK_IF_FAILED(_dxgiSwapChain->ResizeBuffers(
            _buffersCount,
            _currentWidth, _currentHeight,
            swapChainDesc.BufferDesc.Format,
            swapChainDesc.Flags
        ));

        _updateRenderTargetViews();
    }
}

Microsoft::WRL::ComPtr<ID3D12Resource> jwm::DX12SwapChain::getCurrentBackBuffer() const {
    return _backBuffers[getCurrentBackBufferIndex()];
}

UINT jwm::DX12SwapChain::getCurrentBackBufferIndex() const {
    return _dxgiSwapChain->GetCurrentBackBufferIndex();
}

void jwm::DX12SwapChain::_updateRenderTargetViews() {
    using namespace Microsoft::WRL;

    ComPtr<ID3D12Device2> device = _dx12device.getDevicePtr();

    for (unsigned int i = 0; i < _buffersCount; i++) {
        ComPtr<ID3D12Resource> backBuffer;

        CHECK_IF_FAILED(_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

        _backBuffers.push_back(std::move(backBuffer));
    }
}

void jwm::DX12SwapChain::_verifyBufferCounters() {
#if defined(_DEBUG)
    for (auto& buffer: _backBuffers) {
        ID3D12Resource* resource = buffer.Get();

        resource->AddRef();
        auto refCount = resource->Release();

        assert(refCount == 2);
    }
#endif
}
