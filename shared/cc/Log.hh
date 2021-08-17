#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <memory>

namespace jwm {

    enum class LogLevel {
        /**
         * Verbose message log level.
         * Must be used for debug/development, for displaying any expanded information about the library.
         *
         * @note Will be disabled by default in release builds.
         */
        Verbose = 0,

        /**
         * Default message log level.
         * Must be used for displaying important/error/warning messages.
         *
         * @note Will be enabled by default in release builds.
         */
        Log = 1
    };

    class LogEntry {
    public:
        LogEntry(std::wstring message, std::string file,
                 std::string function, unsigned long int line, LogLevel level);

        const std::wstring &getMessage() const { return _message; }
        const std::string &getFile() const { return _file; }
        const std::string &getFunction() const { return _function; }
        unsigned long int getLine() const { return _line; }
        LogLevel getLevel() const { return _level; }

    private:
        friend class Log;

        // Uses wchar_t string for correct unicode handling
        // NOTE: linux/mac (utf-32) and win (utf-16)
        std::wstring _message;
        std::string _file;
        std::string _function;
        unsigned long int _line;
        LogLevel _level;
    };

    /**
     * Log entry listener.
     * Allows to track when new entries are added into the log.
     */
    class LogListenerProxy;

    class Log {
    public:
        static const LogLevel DEFAULT_LEVEL = LogLevel::Log;

    public:
        Log() = default;
        Log(const Log&) = delete;
        Log(Log&&) = delete;
        ~Log() = default;

        void log(const LogEntry& entry);
        void setLevel(LogLevel level);
        void setListener(class LogListenerProxy *listener);

        void enable(bool enabled);
        bool checkLevel(LogLevel level) const;

    public:
        static Log& getInstance();

    private:
        LogLevel _level = DEFAULT_LEVEL;
        class LogListenerProxy* _listener = nullptr;
        bool _enabled = false;
    };

    /** Used to compose complex messages in '<<' operator style */
    class LogBuilder: public std::wstringstream {
    public:
        LogBuilder(std::string file, std::string function,
                   unsigned long int line, LogLevel level, Log& log);

        void commit();

    private:
        std::string _file;
        std::string _function;
        unsigned long int _line;
        LogLevel _level;
        Log& _log;
    };

}

// Macro to check if message can be logged, if can than
// create builder and assemble message
#define JWM_LOG_CAT(logLevel, message)                      \
    do {                                                    \
        auto& __log = ::jwm::Log::getInstance();            \
        if (__log.checkLevel(logLevel)) {                   \
            LogBuilder __builder(                           \
                __FILE__,                                   \
                __FUNCTION__,                               \
                static_cast<unsigned long int>(__LINE__),   \
                logLevel,                                   \
                __log                                       \
            );                                              \
            __builder << message;                           \
            __builder.commit();                             \
        } \
    }  while (false);

// Log macro proxy for logging global instance verbose (debug) messages
#define JWM_VERBOSE(message)        \
    JWM_LOG_CAT(::jwm::LogLevel::Verbose, message)

// Log macro proxy for logging global instance log (default) messages
#define JWM_LOG(message)            \
    JWM_LOG_CAT(::jwm::LogLevel::Log, message)