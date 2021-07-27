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
    _verifyBufferCounters();
    _backBuffers.clear();
    _rtvDescriptorHeap.Reset();
    _dxgiSwapChain.Reset();

    unref(&_window);
}

void jwm::DX12SwapChain::setBuffersCount(unsigned int buffersCount) {
    assert(buffersCount >= 2);
    assert(buffersCount <= 3);
    _buffersCount = buffersCount;
}

void jwm::DX12SwapChain::create() {
    using namespace Microsoft::WRL;

    DX12Common& dx12Common = _dx12device.getDx12Common();
    ComPtr<IDXGIFactory4> dxgiFactory4 = dx12Common.getFactory();
    ComPtr<IDXGISwapChain1> swapChain1;

    _window->getClientAreaSize(_currentWidth, _currentHeight);

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

    THROW_IF_FAILED(dxgiFactory4->CreateSwapChainForHwnd(
        _dx12commandQueue.getQueuePtr().Get(),
        hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1)
    );

    // Disable the Alt+Enter fullscreen toggle feature
    THROW_IF_FAILED(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
    THROW_IF_FAILED(swapChain1.As(&_dxgiSwapChain));

    ComPtr<ID3D12Device2> device = _dx12device.getDevicePtr();

    _rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    _rtvDescriptorHeap = _dx12device.createDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, _buffersCount);
    _updateRenderTargetViews();
}

void jwm::DX12SwapChain::transitionLayout(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &cmdList,
                                          D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
    using namespace Microsoft::WRL;

    UINT backBufferIndex = getCurrentBackBufferIndex();
    ComPtr<ID3D12Resource> backBuffer = _backBuffers[backBufferIndex];

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        backBuffer.Get(),
        before,
        after
    );

    cmdList->ResourceBarrier(1, &barrier);
}

void jwm::DX12SwapChain::clearTarget(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &cmdList,
                                     float r, float g, float b, float a) {
    UINT backBufferIndex = getCurrentBackBufferIndex();
    FLOAT clearColor[] = { r, g, b, a };

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
        _rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        static_cast<INT>(backBufferIndex),
        _rtvDescriptorSize
    );

    cmdList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

void jwm::DX12SwapChain::present(UINT syncInterval, UINT presentationFlags) {
    THROW_IF_FAILED(_dxgiSwapChain->Present(syncInterval, presentationFlags));
}

void jwm::DX12SwapChain::resize(int newWidth, int newHeight) {
    if (_currentWidth != newWidth || _currentHeight != newHeight) {
        _currentWidth = std::max(1, newWidth);
        _currentHeight = std::max(1, newHeight);

        _verifyBufferCounters();
        _backBuffers.clear();

        DXGI_SWAP_CHAIN_DESC swapChainDesc{};
        THROW_IF_FAILED(_dxgiSwapChain->GetDesc(&swapChainDesc));
        THROW_IF_FAILED(_dxgiSwapChain->ResizeBuffers(
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

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    for (unsigned int i = 0; i < _buffersCount; i++) {
        ComPtr<ID3D12Resource> backBuffer;

        THROW_IF_FAILED(_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

        device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
        _backBuffers.push_back(std::move(backBuffer));

        rtvHandle.Offset(static_cast<int>(_rtvDescriptorSize));
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
