#include <D3D12/DX12Common.hh>

bool jwm::DX12Common::init() {
    std::lock_guard<std::mutex> lock(_accessMutex);

    if (_initialized)
        return true;

    // Disable for now (somehow does not work with skia/skija)
    // enableDebugLayer();

    return _initialized = true;
}

bool jwm::DX12Common::finalize() {
    std::lock_guard<std::mutex> lock(_accessMutex);

    if (_initialized)
        _initialized = false;

    return true;
}

void jwm::DX12Common::enableDebugLayer() {
#if defined(_DEBUG)
    using namespace Microsoft::WRL;

    ComPtr<ID3D12Debug> debugInterface;
    CHECK_IF_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));

    debugInterface->EnableDebugLayer();
#endif
}

bool jwm::DX12Common::checkTearingFeature() const {
    using namespace Microsoft::WRL;

    BOOL allowTearing;

    ComPtr<IDXGIFactory4> factory4 = getFactory();
    ComPtr<IDXGIFactory5> factory5;

    if (SUCCEEDED(factory4.As(&factory5))) {
        if (FAILED(factory5->CheckFeatureSupport(
                DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                &allowTearing, sizeof(allowTearing)))) {
            allowTearing = FALSE;
        }
    }

    return allowTearing == TRUE;
}

Microsoft::WRL::ComPtr<IDXGIFactory4> jwm::DX12Common::getFactory() const {
    using namespace Microsoft::WRL;

    ComPtr<IDXGIFactory4> dxgiFactory;
    UINT factoryFlags = _createFactoryFlags;

#if defined(_DEBUG)
    factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

    CHECK_IF_FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&dxgiFactory)));

    return dxgiFactory;
}

Microsoft::WRL::ComPtr<IDXGIAdapter4> jwm::DX12Common::getAdapter(bool useWarp) const {
    using namespace Microsoft::WRL;

    ComPtr<IDXGIFactory4> dxgiFactory = getFactory();

    ComPtr<IDXGIAdapter1> dxgiAdapter1;
    ComPtr<IDXGIAdapter4> dxgiAdapter4;

    if (useWarp) {
        CHECK_IF_FAILED(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
        CHECK_IF_FAILED(dxgiAdapter1.As(&dxgiAdapter4));
    }
    else {
        SIZE_T maxDedicatedVideoMemory = 0;
        D3D_FEATURE_LEVEL featureLevel = getFeatureLevel();

        for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i) {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

            // Ignore warp adapters
            bool notSoftwareAdapter = (dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0;
            // Create dummy device to check if supports required feature level
            bool canCreateDX12Device = SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(), featureLevel, __uuidof(ID3D12Device), nullptr));
            // Has more dedicated VRAM (has more mem, then has more power)
            // Maybe somehow force integrated card (for power efficiency) ?
            bool hasMoreMem = dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory;

            // Select preferred adapter
            // Maybe add later some hooks to customize the choice
            if (notSoftwareAdapter && canCreateDX12Device && hasMoreMem) {
                maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                CHECK_IF_FAILED(dxgiAdapter1.As(&dxgiAdapter4));
            }
        }
    }

    return dxgiAdapter4;
}
