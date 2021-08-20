#pragma once
#include <D3D12/DX12Common.hh>
#include <vector>

namespace jwm {

    class DX12SwapChain {
    public:
        static const DXGI_FORMAT DEFAULT_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
        static const DXGI_SCALING DEFAULT_SCALING = DXGI_SCALING_STRETCH;
        static const DXGI_SWAP_EFFECT DEFAULT_SWAP_EFFECT = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        static const DXGI_ALPHA_MODE DEFAULT_ALPHA_MODE = DXGI_ALPHA_MODE_IGNORE;
        static const UINT DEFAULT_BUFFERS_COUNT = 2;
        static const UINT SAMPLE_COUNT = 1;
        static const UINT LEVEL_COUNT = 0;

    public:
        DX12SwapChain(class WindowWin32* window, class DX12CommandQueue& queue);
        DX12SwapChain(const DX12SwapChain&) = delete;
        DX12SwapChain(DX12SwapChain&&) = delete;
        ~DX12SwapChain();

        void setFormat(DXGI_FORMAT format) { _format = format; }
        void setScaling(DXGI_SCALING scaling) { _scaling = scaling; };
        void setSwapEffect(DXGI_SWAP_EFFECT swapEffect) { _swapEffect = swapEffect; };
        void setAlphaMode(DXGI_ALPHA_MODE alphaMode) { _alphaMode = alphaMode; }
        void setBuffersCount(unsigned int buffersCount);

        bool create();

        void present(UINT syncInterval, UINT presentationFlags);
        void resize(int newWidth, int newHeight);

        Microsoft::WRL::ComPtr<ID3D12Resource> getCurrentBackBuffer() const;
        UINT getCurrentBackBufferIndex() const;
        UINT getBuffersCount() const { return _buffersCount; }
        DXGI_FORMAT getFormat() const { return _format; }
        UINT getSamplesCount() const { return SAMPLE_COUNT; }
        UINT getLevelsCount() const { return LEVEL_COUNT; }

    private:
        void _updateRenderTargetViews();
        void _verifyBufferCounters();

    private:
        Microsoft::WRL::ComPtr<IDXGISwapChain4> _dxgiSwapChain;

        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> _backBuffers;

        DXGI_FORMAT _format = DEFAULT_FORMAT;
        DXGI_SCALING _scaling = DEFAULT_SCALING;
        DXGI_SWAP_EFFECT _swapEffect = DEFAULT_SWAP_EFFECT;
        DXGI_ALPHA_MODE _alphaMode = DEFAULT_ALPHA_MODE;

        UINT _buffersCount = DEFAULT_BUFFERS_COUNT;

        int _currentWidth = 0;
        int _currentHeight = 0;

        class WindowWin32* _window;
        class DX12CommandQueue& _dx12commandQueue;
        class DX12Device& _dx12device;
    };

}