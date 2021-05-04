#pragma once

namespace jwm {
    class Context {
    public:
        Context();
        virtual ~Context();

        void swapBuffers();
        void resize();
    };

    void deleteContext(Context* instance);
}