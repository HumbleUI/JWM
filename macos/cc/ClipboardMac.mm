#include <jni.h>
#include <AppKit/NSPasteboard.h>
#import <Foundation/Foundation.h>
#include <StringUTF16.hh>
#include <impl/Library.hh>
#include <impl/JNILocal.hh>

using namespace jwm;

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_Clipboard__1nSet
        (JNIEnv *env, jclass jclass, jobjectArray entries) {
    jsize size = env->GetArrayLength(entries);
    for (jsize i = 0; i < size; ++i) {
        JNILocal<jobject> entry(env, env->GetObjectArrayElement(entries, i));

        if (entry.get()) {
            JNILocal<jobject> format(env, classes::ClipboardEntry::getFormat(env, entry.get()));
            JNILocal<jbyteArray> data(env, classes::ClipboardEntry::getData(env, entry.get()));
            jsize dataSize = env->GetArrayLength(data.get());

            StringUTF16 formatId = StringUTF16::makeFromJString(env, classes::ClipboardFormat::getFormatId(env, format.get()));

            // TODO add other formats
            if (formatId == "text/plain") {
                NSData *nsData;
                {
                    jbyte *dataBytes = static_cast<jbyte*>(env->GetPrimitiveArrayCritical(data.get(), nullptr));
                    nsData = [NSData dataWithBytes:dataBytes length:dataSize];
                    env->ReleasePrimitiveArrayCritical(data.get(), dataBytes, JNI_ABORT);
                }

                NSPasteboard *board = [NSPasteboard generalPasteboard];

                [board declareTypes:[NSArray arrayWithObject:NSPasteboardTypeString] owner:nil];

                BOOL ret = [[NSPasteboard generalPasteboard] setData:nsData forType:NSPasteboardTypeString];

                break;
            } else {
                NSLog(@"Clipboard - setData not implemented for %s", formatId.toAscii().c_str());
            }
        }
    }
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_humbleui_jwm_Clipboard__1nGet
        (JNIEnv *env, jclass jclass, jobjectArray formats) {
    jsize formatsSize = env->GetArrayLength(formats);
    for (jsize i = 0; i < formatsSize; ++i) {
        JNILocal<jobject> format(env, env->GetObjectArrayElement(formats, i));
        if (format.get()) {
            StringUTF16 formatId = StringUTF16::makeFromJString(env, classes::ClipboardFormat::getFormatId(env, format.get()));

            NSData *clipData = nullptr;
            NSUInteger dataSize = 0;
            const char *dataBuffer = nullptr;

            // TODO add other formats
            if (formatId == "text/plain") {
                clipData = [[NSPasteboard generalPasteboard] dataForType:NSPasteboardTypeString];

                dataSize = [clipData length];
                dataBuffer = (const char *) [clipData bytes];
            }

            if (dataBuffer == nullptr)
                return nullptr;
            
            JNILocal<jbyteArray> data(env, env->NewByteArray(static_cast<jsize>(dataSize)));
            jbyte* bytes = static_cast<jbyte*>(env->GetPrimitiveArrayCritical(data.get(), nullptr));
            std::memcpy(bytes, dataBuffer, dataSize);
            env->ReleasePrimitiveArrayCritical(data.get(), bytes, 0);

            jobject ret = classes::ClipboardEntry::make(env, format.get(), data.get());

            return ret;
        }
    }
    classes::Throwable::exceptionThrown(env);

    return nullptr;
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_io_github_humbleui_jwm_Clipboard__1nGetFormats
        (JNIEnv *env, jclass jclass) {
    jobjectArray jniFormats = env->NewObjectArray(1, classes::ClipboardFormat::kCls, nullptr);
    env->SetObjectArrayElement(jniFormats, 0, classes::Clipboard::registerFormat(env, env->NewStringUTF("text/plain")));
    return jniFormats;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_Clipboard__1nClear(JNIEnv *env, jclass jclass) {
    NSPasteboard *board = [NSPasteboard generalPasteboard];
    [board clearContents];
}

extern "C" JNIEXPORT jboolean JNICALL Java_io_github_humbleui_jwm_Clipboard__1nRegisterFormat
        (JNIEnv *env, jclass jclass, jstring formatId) {
    return true;
}
