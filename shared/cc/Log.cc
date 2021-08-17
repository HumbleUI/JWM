#include "Log.hh"
#include "impl/JNILocal.hh"
#include "impl/Library.hh"
#include "StringUTF16.hh"
#include <cassert>
#include <memory>
#include <jni.h>

namespace jwm {
    class LogListenerProxy {
    public:
        LogListenerProxy(JNIEnv* env, jobject listener) {
            _env = env;
            _listener = listener;
        }

        ~LogListenerProxy() {
            _env->DeleteGlobalRef(_listener);
            _env = nullptr;
            _listener = nullptr;
        }

        void notify(const LogEntry& entry) const {
            std::wstringstream finalMessageBuilder;
            const std::string& file = entry.getFile();

#ifdef WIN32
            const char sep[] = "\\";
#else
            const char sep[] = "/";
#endif

            auto pos = file.find_last_of(sep);
            finalMessageBuilder
                << (pos == std::string::npos? file.c_str(): file.substr(pos + 1).c_str()) << ":"
                << entry.getLine() << " "
                << entry.getMessage();

            std::wstring finalMessage = finalMessageBuilder.str();

#ifdef WIN32
            // In win wchar_t string is utf-16 encoded, no conversion for jni jstring is required
            JNILocal<jstring> jmessage(_env, _env->NewString(reinterpret_cast<const jchar*>(finalMessage.c_str()), static_cast<jsize>(finalMessage.length())));
#else
            // In linux/mac wchar_t string is utf-32 encoded, required conversion to utf-16 before jni jstring creation
            StringUTF16 stringUtf16(reinterpret_cast<const uint32_t*>(finalMessage.c_str()));
            JNILocal<jstring> jmessage = stringUtf16.toJString(_env);
#endif
            classes::Consumer::accept(_env, _listener, jmessage.get());
        }

    private:
        jobject _listener;
        JNIEnv* _env;
    };
}

jwm::LogEntry::LogEntry(std::wstring message, std::string file,
                        std::string function, unsigned long line, jwm::LogLevel level)
    : _message(std::move(message)),
      _file(std::move(file)),
      _function(std::move(function)),
      _line(line),
      _level(level) {

}

void jwm::Log::log(const jwm::LogEntry &entry) {
    if (checkLevel(entry.getLevel())) {
        if (_listener)
            _listener->notify(entry);
    }
}

void jwm::Log::setLevel(jwm::LogLevel level) {
    _level = level;
}

void jwm::Log::setListener(class LogListenerProxy *listener) {
    if (_listener)
        delete _listener;

    _listener = listener;
}

void jwm::Log::enable(bool enabled) {
    _enabled = enabled;
}

bool jwm::Log::checkLevel(jwm::LogLevel level) const {
    return _enabled && level >= _level;
}

jwm::Log &jwm::Log::getInstance() {
    static Log gLog;
    return gLog;
}

jwm::LogBuilder::LogBuilder(std::string file, std::string function,
                            unsigned long line, jwm::LogLevel level, jwm::Log &log)
    : _file(std::move(file)),
      _function(std::move(function)),
      _line(line),
      _level(level),
      _log(log) {

}

void jwm::LogBuilder::commit() {
    std::wstring message = this->str();
    LogEntry entry(
        std::move(message),
        std::move(_file),
        std::move(_function),
        _line,
        _level
    );

    _log.log(entry);
    this->clear();
}

// JNI

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Log__1nSetVerbose
    (JNIEnv* env, jclass jclass, jboolean enabled) {
    jwm::Log& log = jwm::Log::getInstance();
    log.setLevel(enabled? jwm::LogLevel::Verbose: jwm::LogLevel::Log);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Log__1nSetListener
        (JNIEnv* env, jclass jclass, jobject listener) {
    jwm::Log& log = jwm::Log::getInstance();
    log.enable(listener != nullptr);
    log.setListener(listener? new jwm::LogListenerProxy(env, env->NewGlobalRef(listener)): nullptr);
}