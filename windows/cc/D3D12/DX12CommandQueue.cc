#include <D3D12/DX12CommandQueue.hh>
#include <D3D12/DX12Device.hh>
#include <D3D12/DX12Fence.hh>

jwm::DX12CommandQueue::DX12CommandQueue(D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_QUEUE_PRIORITY priority,
                                        DX12Device &dx12device)
    : _cmdListType(type), _priority(priority), _dx12device(dx12device) {

    D3D12_COMMAND_QUEUE_DESC desc{};
    desc.Type = _cmdListType;
    desc.Priority = priority;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    THROW_IF_FAILED(_dx12device.getDevicePtr()->CreateCommandQueue(&desc, IID_PPV_ARGS(&_d3d12CommandQueue)));
}


UINT64 jwm::DX12CommandQueue::Signal(DX12Fence &fence, UINT64 &value) {
    UINT64 valueToSignal = ++value;
    THROW_IF_FAILED(_d3d12CommandQueue->Signal(fence.getPtr().Get(), valueToSignal));
    return valueToSignal;
}

void jwm::DX12CommandQueue::waitIdle(DX12Fence &fence, UINT64 &value) {
    UINT64 valueToSignal = Signal(fence, value);
    fence.waitFor(valueToSignal);
}

void  jwm::DX12CommandQueue::Submit(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &cmdList) {
    ID3D12CommandList* const commandLists[] = { cmdList.Get() };
    _d3d12CommandQueue->ExecuteCommandLists(1, commandLists);
}

Microsoft::WRL::ComPtr<ID3D12CommandAllocator> jwm::DX12CommandQueue::createCommandAllocator() {
    using namespace Microsoft::WRL;

    ComPtr<ID3D12Device2> device = _dx12device.getDevicePtr();
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    THROW_IF_FAILED(device->CreateCommandAllocator(_cmdListType, IID_PPV_ARGS(&commandAllocator)));

    return commandAllocator;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> jwm::DX12CommandQueue::createCommandList
    (const Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& cmdAlloc) {
    using namespace Microsoft::WRL;

    ComPtr<ID3D12Device2> device = _dx12device.getDevicePtr();
    ComPtr<ID3D12GraphicsCommandList> commandList;

    THROW_IF_FAILED(device->CreateCommandList(0, _cmdListType, cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&commandList)));
    THROW_IF_FAILED(commandList->Close());

    return commandList;
}
