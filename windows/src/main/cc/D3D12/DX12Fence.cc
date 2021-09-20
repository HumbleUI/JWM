#include <D3D12/DX12Fence.hh>
#include <D3D12/DX12Device.hh>
#include <cassert>

jwm::DX12Fence::DX12Fence(DX12Device &device) : _dx12device(device) {
    CHECK_IF_FAILED(_dx12device.getDevicePtr()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));
    _event = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    assert(_event);
}

jwm::DX12Fence::~DX12Fence() {
    if (_event) {
        CloseHandle(_event);
        _event = nullptr;
    }
}

void jwm::DX12Fence::waitFor(UINT64 value, jwm::DX12Fence::milliseconds duration) {
    if (_fence->GetCompletedValue() < value) {
        CHECK_IF_FAILED(_fence->SetEventOnCompletion(value, _event));
        WaitForSingleObject(_event, static_cast<DWORD>(duration.count()));
    }
}
