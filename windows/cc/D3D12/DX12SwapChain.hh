#pragma once
#include <D3D12/DX12Common.hh>
#include <vector>

namespace jwm {

    class DX12SwapChain {
    public:
        static const DXGI_FORMAT DEFAULT_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
        static const DXGI_SCALING DEFAULT_SCALING = DXGI_SCALING_NONE;
        static const DXGI_SWAP_EFFECT DEFAULT_SWAP_EFFECT = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        static const DXGI_ALPHA_MODE DEFAULT_ALPHA_MODE = DXGI_ALPHA_MODE_IGNORE;
        static const UINT DEFAULT_BUFFERS_COUNT = 2;

        template<typename T>
        using ComPtr = Microsoft::WRL::ComPtr<T>;

    public:
        DX12SwapChain(class WindowWin32* window, class DX12CommandQueue& queue);
        ~DX12SwapChain();

        void setFormat(DXGI_FORMAT format) { _format = format; }
        void setScaling(DXGI_SCALING scaling) { _scaling = scaling; };
        void setSwapEffect(DXGI_SWAP_EFFECT swapEffect) { _swapEffect = swapEffect; };
        void setAlphaMode(DXGI_ALPHA_MODE alphaMode) { _alphaMode = alphaMode; }
        void setBuffersCount(unsigned int buffersCount);

        void create();
        void recreate();

        void transitionLayout(const ComPtr<ID3D12GraphicsCommandList> &cmdList, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
        void clearTarget(const ComPtr<ID3D12GraphicsCommandList> &cmdList, float r, float g, float b, float a);
        void present(UINT syncInterval, UINT presentationFlags);
        void resize(int newWidth, int newHeight);
        UINT getCurrentBackBufferIndex() const;
        UINT getBuffersCount() const { return _buffersCount; }

    private:
        void _updateRenderTargetViews();

    private:
        ComPtr<IDXGISwapChain4> _dxgiSwapChain;
        ComPtr<ID3D12DescriptorHeap> _rtvDescriptorHeap;

        std::vector<ComPtr<ID3D12Resource>> _backBuffers;

        DXGI_FORMAT _format = DEFAULT_FORMAT;
        DXGI_SCALING _scaling = DEFAULT_SCALING;
        DXGI_SWAP_EFFECT _swapEffect = DEFAULT_SWAP_EFFECT;
        DXGI_ALPHA_MODE _alphaMode = DEFAULT_ALPHA_MODE;

        UINT _buffersCount = DEFAULT_BUFFERS_COUNT;
        UINT _rtvDescriptorSize = 0;

        int _currentWidth = 0;
        int _currentHeight = 0;

        class WindowWin32* _window;
        class DX12CommandQueue& _dx12commandQueue;
        class DX12Device& _dx12device;
    };

}