#include <jni.h>
#include <AppKit/NSPasteboard.h>
#include "ClipboardMac.hh"

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_Clipboard__1nSet
        (JNIEnv *env, jclass jclass, jobjectArray entries) {
    printf("set\n");
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_humbleui_jwm_Clipboard__1nGet
        (JNIEnv *env, jclass jclass, jobjectArray formats) {
    printf("get\n");
    return nullptr;
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_io_github_humbleui_jwm_Clipboard__1nGetFormats
        (JNIEnv *env, jclass jclass) {
    printf("getFormats\n");
    return nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_Clipboard__1nClear(JNIEnv *env, jclass jclass) {
    NSPasteboard* board = [NSPasteboard generalPasteboard];
    [board clearContents];
}

extern "C" JNIEXPORT jboolean JNICALL Java_io_github_humbleui_jwm_Clipboard__1nRegisterFormat
        (JNIEnv *env, jclass jclass, jstring formatId) {
    printf("registerFormat\n");
    return true;
}
