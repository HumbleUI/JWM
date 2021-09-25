#include <ClipboardWin32.hh>
#include <AppWin32.hh>
#include <impl/Library.hh>
#include <Log.hh>
#include <Richedit.h>
#include <cstring>
#include <algorithm>

#ifndef CF_HTML
 #define CF_HTML "HTML Format"
#endif

#ifndef CF_URL
 #define CF_URL "URL Format"
#endif

namespace jwm {
    class ClipboardAccess {
    public:
        explicit ClipboardAccess(ClipboardWin32* clipboardWin32) : _clipboardWin32(clipboardWin32) {
            _isOpen = _clipboardWin32->_openClipboard();
        }
        ~ClipboardAccess() {
            _clipboardWin32->_closeClipboard();
        }

        bool isOpen() const { return _isOpen; }

        ClipboardWin32* _clipboardWin32;
        bool _isOpen = false;
    };
}

jwm::ClipboardWin32::ClipboardWin32() : _app(AppWin32::getInstance()) {
    _registerDefaultFormats();
}

void jwm::ClipboardWin32::_registerDefaultFormats() {
    _defaultFormats.emplace(DF_TEXT, CF_UNICODETEXT);
    _defaultFormats.emplace(DF_RTF, _getOrRegisterNativeID(CF_RTF));
    _defaultFormats.emplace(DF_HTML, _getOrRegisterNativeID(CF_HTML));
    _defaultFormats.emplace(DF_URL, _getOrRegisterNativeID(CF_URL));
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
                JWM_LOG("Unregistered clipboard format '" << formatId << "'");
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

            // Use mode=0 to release array and copy data back into java array
            env->ReleaseByteArrayElements(data.get(), bytes, 0);

            GlobalUnlock(hGlobalMem);

            JNILocal<jobject> entry(env, classes::ClipboardEntry::make(env, format, data.get()));
            return env->NewGlobalRef(entry.get());
        }
    }

    return nullptr;
}

jobjectArray jwm::ClipboardWin32::getFormats() {
    ClipboardAccess access(this);

    if (!access.isOpen())
        return nullptr;

    JNIEnv* env = _app.getJniEnv();
    std::vector<jobject> formats;

    UINT currentFormatId = 0;

    while ((currentFormatId = EnumClipboardFormats(currentFormatId)) != 0) {
        const size_t BUFFER_SIZE = 10000;
        wchar_t formatName[BUFFER_SIZE];

        int length = GetClipboardFormatNameW(currentFormatId, formatName, BUFFER_SIZE);
        jobject format;

        if (length > 0) {
            // This is registered format which has custom user-friendly name
            std::wstring formatStrId(formatName, length);

            // If it is not registered, add native id to map, so know we will know about this format
            if (_registeredFormats.find(formatStrId) == _registeredFormats.end())
                _registeredFormats.emplace(formatStrId, currentFormatId);

            JNILocal<jstring> formatId(env, env->NewString(reinterpret_cast<const jchar *>(formatName), length));
            format = classes::Clipboard::registerFormat(env, formatId.get());
        }
        else {
            // This is built-in os format (they do not have user-friendly string names)
            std::wstring formatStrId;

            // If it is known format, we will get its name
            // otherwise we create its string name
            if (!_getDefaultFormatName(currentFormatId, formatStrId)) {
                wchar_t builtInFormatName[BUFFER_SIZE];
                _snwprintf_s(builtInFormatName, BUFFER_SIZE, L"%ls%u", DF_BUILT_IN_PREFIX, currentFormatId);
                formatStrId = std::move(std::wstring(builtInFormatName));
            }

            // Register this string mapping if required
            if (_registeredFormats.find(formatStrId) == _registeredFormats.end())
                _registeredFormats.emplace(formatStrId, currentFormatId);

            JNILocal<jstring> formatId(env, env->NewString(reinterpret_cast<const jchar *>(formatStrId.c_str()), static_cast<jsize>(formatStrId.length())));
            format = classes::Clipboard::registerFormat(env, formatId.get());
        }

        formats.push_back(format);
    }

    // Fill java array with format objects
    if (!formats.empty()) {
        jobjectArray jniFormats = env->NewObjectArray(static_cast<jsize>(formats.size()), classes::ClipboardFormat::kCls, nullptr);

        for (jsize i = 0; i < static_cast<jsize>(formats.size()); i++) {
            env->SetObjectArrayElement(jniFormats, i, formats[i]);
        }

        return jniFormats;
    }

    // No available formats, return null as said in method doc
    return nullptr;
}

void jwm::ClipboardWin32::set(jobjectArray entries) {
    ClipboardAccess access(this);

    if (!access.isOpen())
        return;

    // Clear clipboard content
    // and mark us as new owner
    if (!_emptyClipboard())
        return;

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
                JWM_LOG("Unregistered clipboard format '" << formatId << "'");
                continue;
            }

            UINT nativeId;
            HGLOBAL hGlobalMem = GlobalAlloc(GMEM_MOVEABLE, dataSize * sizeof(jbyte) + sizeof(jchar));

            if (!hGlobalMem)
                continue;

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

    if (!access.isOpen())
        return;

    _emptyClipboard();
}

bool jwm::ClipboardWin32::registerFormat(jstring formatId) {
    std::wstring formatIdStr;
    _getStringStringId(formatId, formatIdStr);

    if (_defaultFormats.find(formatIdStr) == _defaultFormats.end())
        // User cannot register default formats as custom
        return false;

    if (_registeredFormats.find(formatIdStr) != _registeredFormats.end())
        // All right, format already registered
        return true;

    if (formatIdStr.find(DF_BUILT_IN_PREFIX) != std::wstring::npos)
        // Cannot register custom format, which clash built-in reserved prefix
        return false;

    // Register new one
    _registeredFormats.emplace(formatIdStr, _getOrRegisterNativeID(formatIdStr.c_str()));

    return true;
}

bool jwm::ClipboardWin32::_emptyClipboard() {
    if (!EmptyClipboard()) {
        JWM_LOG("Failed to empty clipboard");
        return false;
    }

    return true;
}

UINT jwm::ClipboardWin32::_getOrRegisterNativeID(const wchar_t *formatName) {
    return RegisterClipboardFormatW(formatName);
}

UINT jwm::ClipboardWin32::_getOrRegisterNativeID(const char *formatName) {
    return RegisterClipboardFormatA(formatName);
}

bool jwm::ClipboardWin32::_openClipboard() {
    WindowManagerWin32& man = _app.getWindowManager();
    HWND hHelperWindow = man.getHelperWindow();

    if (!OpenClipboard(hHelperWindow)) {
        JWM_LOG("Failed to open clipboard");
        return false;
    }

    return true;
}

void jwm::ClipboardWin32::_closeClipboard() {
    if (!CloseClipboard())
        JWM_LOG("Failed to close clipboard");
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

bool jwm::ClipboardWin32::_getDefaultFormatName(UINT nativeId, std::wstring &formatStrId) const {
    for (const auto& entry: _defaultFormats) {
        if (entry.second == nativeId) {
            formatStrId = entry.first;
            return true;
        }
    }

    return false;
}

// JNI

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_Clipboard__1nSet
        (JNIEnv* env, jclass jclass, jobjectArray entries) {
    jwm::AppWin32& app = jwm::AppWin32::getInstance();
    jwm::ClipboardWin32& clipboard = app.getClipboard();
    clipboard.set(entries);
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_humbleui_jwm_Clipboard__1nGet
        (JNIEnv* env, jclass jclass, jobjectArray formats) {
    jwm::AppWin32& app = jwm::AppWin32::getInstance();
    jwm::ClipboardWin32& clipboard = app.getClipboard();
    return clipboard.get(formats);
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_io_github_humbleui_jwm_Clipboard__1nGetFormats
        (JNIEnv* env, jclass jclass) {
    jwm::AppWin32& app = jwm::AppWin32::getInstance();
    jwm::ClipboardWin32& clipboard = app.getClipboard();
    return clipboard.getFormats();
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_Clipboard__1nClear
        (JNIEnv* env, jclass jclass) {
    jwm::AppWin32& app = jwm::AppWin32::getInstance();
    jwm::ClipboardWin32& clipboard = app.getClipboard();
    clipboard.clear();
}

extern "C" JNIEXPORT jboolean JNICALL Java_io_github_humbleui_jwm_Clipboard__1nRegisterFormat
        (JNIEnv* env, jclass jclass, jstring formatId) {
    jwm::AppWin32& app = jwm::AppWin32::getInstance();
    jwm::ClipboardWin32& clipboard = app.getClipboard();
    return clipboard.registerFormat(formatId);
}