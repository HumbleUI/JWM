#pragma once
#include <PlatformWin32.hh>
#include <d3d12.h>
#include <D3D12/d3dx12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <exception>
#include <mutex>

#define THROW_IF_FAILED(hr)                     \
    {                                           \
        if (FAILED(hr))                         \
        { throw std::exception(); }             \
    }

namespace jwm {

    class DX12Common {
    public:
        static const D3D_FEATURE_LEVEL FEATURE_LEVEL = D3D_FEATURE_LEVEL_11_0;
        static const UINT DEFAULT_FACTORY_FLAGS = 0;
        static const bool USE_WIN_ADVANCED_RASTER_PLATFORM = false;

    public:
        bool init();
        bool finalize();

    public:
        void enableDebugLayer();
        bool checkTearingFeature() const;

        Microsoft::WRL::ComPtr<IDXGIFactory4> getFactory() const;
        Microsoft::WRL::ComPtr<IDXGIAdapter4> getAdapter(bool useWarp = USE_WIN_ADVANCED_RASTER_PLATFORM) const;

        D3D_FEATURE_LEVEL getFeatureLevel() const { return _featureLevel; }

    private:
        D3D_FEATURE_LEVEL _featureLevel = FEATURE_LEVEL;
        UINT _createFactoryFlags = DEFAULT_FACTORY_FLAGS;

        bool _initialized = false;

        mutable std::mutex _accessMutex;
    };

}