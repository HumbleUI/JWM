#pragma once

namespace jwm {
    class Window;

    class Context {
    public:
        Context() = default;
        virtual ~Context() = default;

        virtual void attach(Window* window) = 0;
        virtual void swapBuffers() = 0;
        virtual void resize() = 0;

        int fWidth = 0;
        int fHeight = 0;
        float fScale = 0;
    };

    void deleteContext(Context* instance);
}