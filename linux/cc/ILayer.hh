#pragma once

namespace jwm {

class ILayer {
public:
    void makeCurrent();
    virtual void makeCurrentForced();


    static ILayer* _ourCurrentLayer;
};

} // namespace jwm