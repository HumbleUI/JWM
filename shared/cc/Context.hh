#pragma once
#include "impl/RefCounted.hh"
#include "Window.hh"

namespace jwm {
    class Context: public RefCounted {
    public:
        Context() = default;
        virtual ~Context() = default;

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