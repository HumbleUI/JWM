#pragma once

#include <jni.h>

namespace jwm
{

    /**
     * Name for C++ <-> Java class. Must be implemented for each used Java class with template overload.
     * @return name of class i.e. org/jetbrains/jwm/Key
     */
    template<class T>
    const char* getJavaClassName() {
        assert(("unimplemented" && 0));
        return nullptr;
    }
} // namespace jwm
