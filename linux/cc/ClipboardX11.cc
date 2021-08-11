#include <jni.h>
#include <StringUTF16.hh>
#include <impl/Library.hh>
#include <impl/JNILocal.hh>
#include "AppX11.hh"
#include <cstring>

namespace jwm {
    class ClipboardX11 {
    public:
        static ClipboardX11& inst() {
            static ClipboardX11 s;
            return s;
        }
        
        jobject get(JNIEnv* env, jobjectArray formats) {
            jsize formatsSize = env->GetArrayLength(formats);
            for (jsize i = 0; i < formatsSize; ++i) {
                jobject format = env->GetObjectArrayElement(formats, i);
                if (format) {
                    jwm::StringUTF16 formatId = jwm::StringUTF16::makeFromJString(env, classes::ClipboardFormat::getFormatId(env, format));


                    ByteBuf contents;
                    // HACK: prefer UTF8_STRING over text/plain and convert it to utf16
                    if (formatId == "text/plain") {
                        contents = app.getWindowManager().getClipboard("UTF8_STRING");
                    }
                    // TODO add another formats
                    if (contents.empty()) {
                        return nullptr;
                    }
                    JNILocal<jbyteArray> data(env, env->NewByteArray(static_cast<jsize>(contents.size())));
                    jbyte* bytes = env->GetByteArrayElements(data.get(), nullptr);
                    std::memcpy(bytes, contents.data(), contents.size());

                    env->ReleaseByteArrayElements(data.get(), bytes, 0);


                    JNILocal<jobject> entry(env, classes::ClipboardEntry::make(env, format, data.get()));
                    return env->NewGlobalRef(entry.get());
                }
            }
            classes::Throwable::exceptionThrown(env);
            return nullptr;
        }

    };
}


// JNI

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Clipboard__1nSet
        (JNIEnv* env, jclass jclass, jobjectArray entries) {
    
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_Clipboard__1nGet
        (JNIEnv* env, jclass jclass, jobjectArray formats) {
    return jwm::ClipboardX11::inst().get(env, formats);
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_org_jetbrains_jwm_Clipboard__1nGetFormats
        (JNIEnv* env, jclass jclass) {
    return nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Clipboard__1nClear
        (JNIEnv* env, jclass jclass) {

}

extern "C" JNIEXPORT jboolean JNICALL Java_org_jetbrains_jwm_Clipboard__1nRegisterFormat
        (JNIEnv* env, jclass jclass, jstring formatId) {
   return 0;
}