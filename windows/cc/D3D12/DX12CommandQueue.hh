#pragma once
#include <D3D12/DX12Common.hh>
#include <PlatformWin32.hh>

namespace jwm {

    class DX12CommandQueue {
    public:
        static const D3D12_COMMAND_LIST_TYPE DEFAULT_TYPE = D3D12_COMMAND_LIST_TYPE_DIRECT;
        static const D3D12_COMMAND_QUEUE_PRIORITY DEFAULT_PRIORITY = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

    public:
        DX12CommandQueue(D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_QUEUE_PRIORITY priority,
                         class DX12Device &dx12device);
        DX12CommandQueue(const DX12CommandQueue&) = delete;
        DX12CommandQueue(DX12CommandQueue&&) = delete;

        UINT64 Signal(class DX12Fence& fence, UINT64 &value);
        void waitIdle(class DX12Fence& fence, UINT64 &value);
        void Submit(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &cmdList);

        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> createCommandAllocator();
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> createCommandList(const Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& cmdAlloc);

        Microsoft::WRL::ComPtr<ID3D12CommandQueue> getQueuePtr() const { return _d3d12CommandQueue; }
        D3D12_COMMAND_LIST_TYPE getCmdListType() const { return _cmdListType; }
        D3D12_COMMAND_QUEUE_PRIORITY getQueuePriority() const { return _priority; }

        class DX12Device &getDx12device() const { return _dx12device; };

    private:
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> _d3d12CommandQueue;
        D3D12_COMMAND_LIST_TYPE _cmdListType;
        D3D12_COMMAND_QUEUE_PRIORITY _priority;

        class DX12Device &_dx12device;
    };

}