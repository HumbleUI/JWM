#pragma once
#include <D3D12/DX12Common.hh>
#include <vector>

namespace jwm {

    class DX12Device {
    public:
        explicit DX12Device(DX12Common& dx12Common);
        DX12Device(const DX12Device&) = delete;
        DX12Device(DX12Device&&) = delete;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> createDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, unsigned int numDescriptors);

        Microsoft::WRL::ComPtr<ID3D12Device2> getDevicePtr() const { return _d3d12Device; };
        Microsoft::WRL::ComPtr<IDXGIAdapter4> getAdapterPtr() const { return _dxgiAdapter; };

        const std::vector<D3D12_MESSAGE_ID> &getDenyList() const { return _denyList; }
        const std::vector<D3D12_MESSAGE_SEVERITY> &getSuppressSeverity() const { return _suppressSeverity; };

        DX12Common& getDx12Common() const { return _dx12Common; };

    private:
        void _setupDefaultDenyList();
        void _setupDefaultSuppressSeverity();

    private:
        Microsoft::WRL::ComPtr<ID3D12Device2> _d3d12Device;
        Microsoft::WRL::ComPtr<IDXGIAdapter4> _dxgiAdapter;

        std::vector<D3D12_MESSAGE_ID> _denyList;
        std::vector<D3D12_MESSAGE_SEVERITY> _suppressSeverity;

        DX12Common& _dx12Common;
    };

}