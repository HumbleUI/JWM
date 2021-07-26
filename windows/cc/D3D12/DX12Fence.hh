#pragma once
#include <D3D12/DX12Common.hh>
#include <PlatformWin32.hh>
#include <chrono>

namespace jwm {

    class DX12Fence {
    public:
        using milliseconds = std::chrono::milliseconds;
        static const UINT64 INITIAL_VALUE = 0;
        
    public:
        explicit DX12Fence(class DX12Device& device);
        DX12Fence(const DX12Fence&) = delete;
        DX12Fence(DX12Fence&&) = delete;
        ~DX12Fence();

        void waitFor(UINT64 value, milliseconds duration = milliseconds::max());

        Microsoft::WRL::ComPtr<ID3D12Fence> getPtr() const { return _fence; }
        HANDLE getEventHnd() const { return _event; }

    private:
        Microsoft::WRL::ComPtr<ID3D12Fence> _fence;
        HANDLE _event = nullptr;

        class DX12Device& _dx12device;
    };

}