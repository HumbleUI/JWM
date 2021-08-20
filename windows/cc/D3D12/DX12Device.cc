#include <D3D12/DX12Device.hh>

jwm::DX12Device::DX12Device(jwm::DX12Common &dx12Common) : _dx12Common(dx12Common) {

}

bool jwm::DX12Device::init() {
    using namespace Microsoft::WRL;

    _dxgiAdapter = _dx12Common.getAdapter();

    if (!_dxgiAdapter)
        return false;

    _setupDefaultDenyList();
    _setupDefaultSuppressSeverity();

    CHECK_IF_FAILED(D3D12CreateDevice(_dxgiAdapter.Get(), _dx12Common.getFeatureLevel(), IID_PPV_ARGS(&_d3d12Device)));

    if (!_d3d12Device)
        return false;

#if defined(_DEBUG)
    ComPtr<ID3D12InfoQueue> pInfoQueue;

    if (SUCCEEDED(_d3d12Device.As(&pInfoQueue))) {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

        D3D12_INFO_QUEUE_FILTER NewFilter{};
        NewFilter.DenyList.NumSeverities = static_cast<UINT>(_suppressSeverity.size());
        NewFilter.DenyList.pSeverityList = _suppressSeverity.data();
        NewFilter.DenyList.NumIDs = static_cast<UINT>(_denyList.size());
        NewFilter.DenyList.pIDList = _denyList.data();

        CHECK_IF_FAILED(pInfoQueue->PushStorageFilter(&NewFilter));
    }
#endif

    return true;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> jwm::DX12Device::createDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, unsigned int numDescriptors) {
    using namespace Microsoft::WRL;

    ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.NumDescriptors = numDescriptors;
    desc.Type = type;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    desc.NodeMask = 0;

    CHECK_IF_FAILED(_d3d12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

    return descriptorHeap;
}

void jwm::DX12Device::_setupDefaultDenyList() {
    _denyList = {
        D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
        D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
        D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
    };
}

void jwm::DX12Device::_setupDefaultSuppressSeverity() {
    _suppressSeverity = {
        D3D12_MESSAGE_SEVERITY_INFO
    };
}
