#include <jni.h>

int main(int argc, char* argv[]);

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_App__1nRunEventLoop
  (JNIEnv* env, jclass jclass) {
    main(0, nullptr);
}