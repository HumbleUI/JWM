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
        void swapBuffers();
        void close();
        void vsync(bool enable);

    private:
        std::unique_ptr<class DX12Device> _dx12device;
        std::unique_ptr<class DX12CommandQueue> _dx12commandQueue;
        std::unique_ptr<class DX12SwapChain> _dx12swapChain;
        std::unique_ptr<class DX12Fence> _dx12fence;

        std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> _frameCmdAllocs; // todo: remove
        std::vector<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> _frameCmdLists; // todo: remove
        std::vector<UINT64> _frameFenceValues;
        UINT64 _fenceValue;
        UINT _backBufferIndex;

        class WindowWin32* _window = nullptr;
        int _callbackID = -1;
        bool _tearingFeature = false;

        Vsync _vsync = Vsync::Enable;
    };

}