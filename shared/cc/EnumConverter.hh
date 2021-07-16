#pragma once

#include <type_traits>
#include <jni.h>
#include "JavaClassName.hh"
#include <string>
#include "impl/Library.hh"

namespace jwm {

    /**
     * Converts C++ enum to Java enum. jwm::JavaClass must be implemented for this enum
     */
    template<typename T>
    struct EnumConverter {
        static_assert(std::is_enum<T>::value, "EnumConverter can be used only with enums");

        static jobjectArray values(JNIEnv* env) {
            // call Enum.values() only once because the values() method of enums is expensive
            // Enum[] kKeys = Enum.values()
            struct once {
                jobjectArray kKeys;

                once(JNIEnv* env)
                {
                    // cls = Key
                    jclass cls = env->FindClass(jwm::getJavaClassName<T>());
                    assert(cls);
                    classes::Throwable::exceptionThrown(env);

                    // values = Key::values()
                    jmethodID values = env->GetStaticMethodID(
                        cls,
                        "values",
                        (std::string("()[L") + jwm::getJavaClassName<T>() + ";").c_str()
                    );
                    classes::Throwable::exceptionThrown(env);
                    
                    assert(values);

                    // Enum[] kKeys = Enum.values()
                    jobject array = env->CallStaticObjectMethod(cls, values);
                    classes::Throwable::exceptionThrown(env);
                    assert(array);
                    kKeys = static_cast<jobjectArray>(env->NewGlobalRef(array));
                    assert(kKeys);
                    classes::Throwable::exceptionThrown(env);
                }
            };
            static once once(env);

            return once.kKeys;
        }

        static jobject convert(JNIEnv* env, T v) {
            jobject object = env->GetObjectArrayElement(values(env), int(v));
            Throwable::exceptionThrown(env);
            return object;
        }
    };
}
