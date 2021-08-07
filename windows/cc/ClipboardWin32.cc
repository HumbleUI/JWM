#include <ClipboardWin32.hh>
#include <AppWin32.hh>
#include <impl/Library.hh>
#include <Richedit.h>
#include <cstring>

#ifndef CF_HTML
 #define CF_HTML "HTML Format"
#endif

namespace jwm {
    class ClipboardAccess {
    public:
        explicit ClipboardAccess(ClipboardWin32* clipboardWin32) : _clipboardWin32(clipboardWin32) {
            _clipboardWin32->_openClipboard();
        }
        ~ClipboardAccess() {
            _clipboardWin32->_closeClipboard();
        }

        ClipboardWin32* _clipboardWin32;
    };
}

jwm::ClipboardWin32::ClipboardWin32() : _app(AppWin32::getInstance()) {
    _registerDefaultFormats();
}

void jwm::ClipboardWin32::_registerDefaultFormats() {
    _defaultFormats.emplace(DF_TEXT, CF_UNICODETEXT);
    _defaultFormats.emplace(DF_RTF, _getOrRegisterNativeID(CF_RTF));
    _defaultFormats.emplace(DF_HTML, _getOrRegisterNativeID(CF_HTML));
    _defaultFormats.emplace(DF_URL, CF_UNICODETEXT);
}

jobject jwm::ClipboardWin32::get(jobjectArray formats) {
    ClipboardAccess access(this);

    JNIEnv* env = _app.getJniEnv();
    jsize formatsCount = env->GetArrayLength(formats);

    for (jsize i = 0; i < formatsCount; i++) {
        jobject format = env->GetObjectArrayElement(formats, i);

        if (format) {
            std::wstring formatId;
            _getFormatStringId(format, formatId);

            auto defaultFormat = _defaultFormats.find(formatId);
            auto isDefault = defaultFormat != _defaultFormats.end();
            auto registeredFormat = _registeredFormats.find(formatId);
            auto isRegistered = registeredFormat != _registeredFormats.end();

            if (!isDefault && !isRegistered) {
                _app.sendError("Unregistered clipboard format");
                continue;
            }

            UINT nativeId = isDefault? defaultFormat->second: registeredFormat->second;
            HANDLE hGlobalMem;

            if (!IsClipboardFormatAvailable(nativeId))
                continue;

            hGlobalMem = GetClipboardData(nativeId);

            if (!hGlobalMem)
                continue;

            PVOID memoryPtr = GlobalLock(hGlobalMem);

            if (!memoryPtr)
                continue;

            SIZE_T memorySize = GlobalSize(hGlobalMem);
            SIZE_T copySize;
            SIZE_T nullSize = sizeof(jchar);

            if (isDefault)
                copySize = memorySize >= nullSize? memorySize - nullSize: 0;
            if (isRegistered)
                copySize = memorySize;

            JNILocal<jbyteArray> data(env, env->NewByteArray(static_cast<jsize>(copySize)));
            jbyte* bytes = env->GetByteArrayElements(data.get(), nullptr);
            std::memcpy(bytes, memoryPtr, copySize);
            env->ReleaseByteArrayElements(data.get(), bytes, 0);

            GlobalUnlock(hGlobalMem);

            JNILocal<jobject> entry(env, classes::ClipboardEntry::make(env, format, data.get()));
            return env->NewGlobalRef(entry.get());
        }
    }

    return nullptr;
}

void jwm::ClipboardWin32::set(jobjectArray entries) {
    ClipboardAccess access(this);

    // Clear clipboard content
    // and mark us as new owner
    _emptyClipboard();

    JNIEnv* env = _app.getJniEnv();
    jsize entriesCount = env->GetArrayLength(entries);

    for (jsize i = 0; i < entriesCount; i++) {
        jobject entry = env->GetObjectArrayElement(entries, i);

        if (entry) {
            jobject format = classes::ClipboardEntry::getFormat(env, entry);
            jbyteArray data = classes::ClipboardEntry::getData(env, entry);
            jsize dataSize = env->GetArrayLength(data);

            std::wstring formatId;
            _getFormatStringId(format, formatId);

            auto defaultFormat = _defaultFormats.find(formatId);
            auto isDefault = defaultFormat != _defaultFormats.end();
            auto registeredFormat = _registeredFormats.find(formatId);
            auto isRegistered = registeredFormat != _registeredFormats.end();

            if (!isDefault && !isRegistered) {
                _app.sendError("Unregistered clipboard format");
                continue;
            }

            UINT nativeId;
            HGLOBAL hGlobalMem = GlobalAlloc(GMEM_MOVEABLE, dataSize * sizeof(jbyte) + sizeof(jchar));
            PVOID memoryPtr = GlobalLock(hGlobalMem);

            if (dataSize > 0) {
                jbyte* dataBytes = env->GetByteArrayElements(data, nullptr);
                std::memcpy(memoryPtr, dataBytes, dataSize * sizeof(jbyte));
                env->ReleaseByteArrayElements(data, dataBytes, JNI_ABORT);
            }

            if (isDefault) {
                nativeId = defaultFormat->second;
                auto null = static_cast<jchar>(L'\0');
                std::memcpy(reinterpret_cast<jbyte*>(memoryPtr) + dataSize, &null, sizeof(jchar));
            }

            if (isRegistered)
                nativeId = registeredFormat->second;

            GlobalUnlock(hGlobalMem);
            SetClipboardData(nativeId, hGlobalMem);
        }
    }
}

void jwm::ClipboardWin32::clear() {
    ClipboardAccess access(this);
    _emptyClipboard();
}

bool jwm::ClipboardWin32::registerFormat(jstring formatId) {
    std::wstring formatIdStr;
    _getStringStringId(formatId, formatIdStr);

    if (_defaultFormats.find(formatIdStr) == _defaultFormats.end())
        // User cannot register default formats as custom
        return false;
    else if (_registeredFormats.find(formatIdStr) != _registeredFormats.end())
        // All right, format already registered
        return true;
    else
        // Register new one
        _registeredFormats.emplace(formatIdStr, _getOrRegisterNativeID(formatIdStr.c_str()));

    return true;
}

void jwm::ClipboardWin32::_emptyClipboard() {
    if (!EmptyClipboard())
        _app.sendError("Failed to empty clipboard");
}

UINT jwm::ClipboardWin32::_getOrRegisterNativeID(const wchar_t *formatName) {
    return RegisterClipboardFormatW(formatName);
}

UINT jwm::ClipboardWin32::_getOrRegisterNativeID(const char *formatName) {
    return RegisterClipboardFormatA(formatName);
}

void jwm::ClipboardWin32::_openClipboard() {
    WindowManagerWin32& man = _app.getWindowManager();
    HWND hHelperWindow = man.getHelperWindow();

    if (!OpenClipboard(hHelperWindow))
        _app.sendError("Failed to open clipboard");
}

void jwm::ClipboardWin32::_closeClipboard() {
    if (!CloseClipboard())
        _app.sendError("Failed to close clipboard");
}

void jwm::ClipboardWin32::_getFormatStringId(jobject format, std::wstring &formatIdStr) const {
    JNIEnv* env = _app.getJniEnv();
    jstring formatId = classes::ClipboardFormat::getFormatId(env, format);

    _getStringStringId(formatId, formatIdStr);
}

void jwm::ClipboardWin32::_getStringStringId(jstring formatId, std::wstring &formatIdStr) const {
    JNIEnv* env = _app.getJniEnv();
    jsize length = env->GetStringLength(formatId);
    const jchar* chars = env->GetStringChars(formatId, nullptr);

    formatIdStr = std::move(std::wstring(reinterpret_cast<const wchar_t*>(chars), length));
    env->ReleaseStringChars(formatId, chars);
}

// JNI

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Clipboard__1nSet
        (JNIEnv* env, jclass jclass, jobjectArray entries) {
    jwm::AppWin32& app = jwm::AppWin32::getInstance();
    jwm::ClipboardWin32& clipboard = app.getClipboard();
    clipboard.set(entries);
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_Clipboard__1nGet
        (JNIEnv* env, jclass jclass, jobjectArray formats) {
    jwm::AppWin32& app = jwm::AppWin32::getInstance();
    jwm::ClipboardWin32& clipboard = app.getClipboard();
    return clipboard.get(formats);
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_org_jetbrains_jwm_Clipboard__1nGetFormats
        (JNIEnv* env, jclass jclass) {
    return nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Clipboard__1nClear
        (JNIEnv* env, jclass jclass) {
    jwm::AppWin32& app = jwm::AppWin32::getInstance();
    jwm::ClipboardWin32& clipboard = app.getClipboard();
    clipboard.clear();
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_jetbrains_jwm_Clipboard__1nRegisterFormat
        (JNIEnv* env, jclass jclass, jstring formatId) {
    jwm::AppWin32& app = jwm::AppWin32::getInstance();
    jwm::ClipboardWin32& clipboard = app.getClipboard();
    return clipboard.registerFormat(formatId);
}