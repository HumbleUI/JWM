#pragma once

#include <ILayer.hh>

namespace jwm {
    class ILayerWayland: public ILayer {
    public:
        virtual void attachBuffer() = 0;
        virtual void swapBuffers() = 0;
        virtual void detachBuffer() = 0;
    };
}
