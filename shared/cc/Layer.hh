#pragma once
#include "impl/RefCounted.hh"

namespace jwm {
    class Window;

    class Layer: public RefCounted {
    public:
        Layer() = default;
        virtual ~Layer() = default;

        virtual void attach(Window* window) = 0;
        virtual void detach() = 0;
        virtual void invalidate() = 0;
        virtual void resize() = 0;
        virtual void swapBuffers() = 0;
        
        int fWidth = 0;
        int fHeight = 0;
        float fScale = 0;
        Window* fWindow = nullptr;
    };
}