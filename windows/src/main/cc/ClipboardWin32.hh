#pragma once
#include <PlatformWin32.hh>
#include <impl/JNILocal.hh>
#include <unordered_map>
#include <string>

namespace jwm {

    class ClipboardWin32 {
    public:
        static constexpr wchar_t* DF_TEXT = L"text/plain";
        static constexpr wchar_t* DF_RTF = L"text/rtf";
        static constexpr wchar_t* DF_HTML = L"text/html";
        static constexpr wchar_t* DF_URL = L"text/url";
        static constexpr wchar_t* DF_BUILT_IN_PREFIX = L"win/built-in/format-";

    public:
        ClipboardWin32();
        ClipboardWin32(const ClipboardWin32&) = delete;
        ClipboardWin32(ClipboardWin32&&) = delete;
        ~ClipboardWin32() = default;

        jobject get(jobjectArray formats);
        jobjectArray getFormats();
        void set(jobjectArray entries);
        void clear();
        bool registerFormat(jstring formatId);

    private:
        bool _emptyClipboard();
        void _registerDefaultFormats();
        UINT _getOrRegisterNativeID(const wchar_t* formatName);
        UINT _getOrRegisterNativeID(const char* formatName);
        bool _openClipboard();
        void _closeClipboard();
        void _getFormatStringId(jobject format, std::wstring& formatIdStr) const;
        void _getStringStringId(jstring formatId, std::wstring& formatIdStr) const;
        bool _getDefaultFormatName(UINT nativeId, std::wstring& formatStrId) const;

    private:
        friend class ClipboardAccess;
        class AppWin32& _app;

        std::unordered_map<std::wstring, UINT> _defaultFormats;
        std::unordered_map<std::wstring, UINT> _registeredFormats;
    };

}