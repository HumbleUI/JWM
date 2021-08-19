#pragma once
#include <PlatformWin32.hh>
#include <D3D12/DX12Common.hh>
#include <impl/RefCounted.hh>
#include <memory>
#include <vector>

namespace jwm {

    class LayerD3D12: public RefCounted {
    public:
        enum class Vsync {
            Disable = 0,
            Enable = 1,
        };

    public:
        void attach(class WindowWin32* window);
        void resize(int width, int height);
        void reconfigure();
        void swapBuffers();
        void close();
        void vsync(bool enable);
        void requestSwap();

    public:
        IDXGIAdapter1* getAdapterPtr() const;
        ID3D12Device* getDevicePtr() const;
        ID3D12CommandQueue* getQueuePtr() const;
        ID3D12Resource* getNextRenderTexture() const;
        class DX12SwapChain &getSwapChain() const;

    private:
        std::unique_ptr<class DX12Device> _dx12device;
        std::unique_ptr<class DX12CommandQueue> _dx12commandQueue;
        std::unique_ptr<class DX12SwapChain> _dx12swapChain;
        std::unique_ptr<class DX12Fence> _dx12fence;
        std::vector<UINT64> _fenceFrameValues;

        UINT64 _fenceValue;

        class WindowWin32* _windowWin32 = nullptr;
        int _callbackID = -1;
        bool _tearingFeature = false;

        Vsync _vsync = Vsync::Enable;
    };

}