#include <jni.h>
#include <AppKit/NSPasteboard.h>
#import <Foundation/Foundation.h>
#include "ClipboardMac.hh"
#include <StringUTF16.hh>
#include <impl/Library.hh>
#include <impl/JNILocal.hh>
#include <cstring>

using namespace jwm;

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_Clipboard__1nSet
        (JNIEnv *env, jclass jclass, jobjectArray entries) {
    NSLog(@"Clipboard.set");

    jsize size = env->GetArrayLength(entries);
    for (jsize i = 0; i < size; ++i) {
        jobject entry = env->GetObjectArrayElement(entries, i);

        if (entry) {
            jobject format = classes::ClipboardEntry::getFormat(env, entry);
            jbyteArray data = classes::ClipboardEntry::getData(env, entry);
            jsize dataSize = env->GetArrayLength(data);

            StringUTF16 formatId = StringUTF16::makeFromJString(env, classes::ClipboardFormat::getFormatId(env, format));
            std::string formatStdStr = formatId.toAscii();

            // TODO add other formats
            if (formatStdStr == "text/plain") {
                NSData *nsData;
                {
                    jbyte *dataBytes = env->GetByteArrayElements(data, nullptr);
                    nsData = [NSData dataWithBytes:dataBytes length:dataSize];
                    env->ReleaseByteArrayElements(data, dataBytes, JNI_ABORT);
                }

                NSPasteboard *board = [NSPasteboard generalPasteboard];

                [board declareTypes:[NSArray arrayWithObject:NSPasteboardTypeString] owner:nil];

                BOOL ret = [[NSPasteboard generalPasteboard] setData:nsData forType:NSPasteboardTypeString];

                NSLog(@"Clipboard - setString ret = %d", ret);

                break;
            } else {
                NSLog(@"Clipboard - setData not implemented for %s", formatStdStr.c_str());
            }
        }
    }
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_humbleui_jwm_Clipboard__1nGet
        (JNIEnv *env, jclass jclass, jobjectArray formats) {
    NSLog(@"Clipboard.get");

    jsize formatsSize = env->GetArrayLength(formats);
    for (jsize i = 0; i < formatsSize; ++i) {
        jobject format = env->GetObjectArrayElement(formats, i);
        if (format) {
            StringUTF16 formatId = StringUTF16::makeFromJString(env, classes::ClipboardFormat::getFormatId(env, format));

            NSData *clipData = nullptr;
            NSUInteger dataSize = 0;
            const char *dataBuffer = nullptr;

            // TODO add other formats
            if (formatId == "text/plain") {
                NSLog(@"Clipboard - getString");

                clipData = [[NSPasteboard generalPasteboard] dataForType:NSPasteboardTypeString];

                dataSize = [clipData length];
                dataBuffer = (const char *) [clipData bytes];
            }

            if (dataBuffer == nullptr) {
                return nullptr;
            }
            JNILocal<jbyteArray> data(env, env->NewByteArray(static_cast<jsize>(dataSize)));
            jbyte *bytes = env->GetByteArrayElements(data.get(), nullptr);
            std::memcpy(bytes, dataBuffer, dataSize);

            env->ReleaseByteArrayElements(data.get(), bytes, 0);

            jobject ret = classes::ClipboardEntry::make(env, format, data.get());

            return ret;
        }
    }
    classes::Throwable::exceptionThrown(env);

    return nullptr;
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_io_github_humbleui_jwm_Clipboard__1nGetFormats
        (JNIEnv *env, jclass jclass) {
    NSLog(@"Clipboard.getFormats");

    std::vector<std::string> formats;

    // TODO add other formats
    formats.push_back("text/plain");

    std::vector<jobject> formatObjs;
    for (auto &format: formats) {
        auto js = StringUTF16(format.c_str()).toJString(env);
        formatObjs.push_back(classes::Clipboard::registerFormat(env, js.get()));
    }
    jobjectArray jniFormats = env->NewObjectArray(static_cast<jsize>(formats.size()), classes::ClipboardFormat::kCls, nullptr);

    // fill java array
    for (jsize i = 0; i < static_cast<jsize>(formatObjs.size()); ++i) {
        env->SetObjectArrayElement(jniFormats, i, formatObjs[i]);
    }

    return jniFormats;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_Clipboard__1nClear(JNIEnv *env, jclass jclass) {
    NSPasteboard *board = [NSPasteboard generalPasteboard];
    [board clearContents];
}

extern "C" JNIEXPORT jboolean JNICALL Java_io_github_humbleui_jwm_Clipboard__1nRegisterFormat
        (JNIEnv *env, jclass jclass, jstring formatId) {
    printf("registerFormat\n");
    return true;
}
