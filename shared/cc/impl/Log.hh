#pragma once
#include <string>
#include <sstream>
#include <ctime>
#include <functional>
#include <vector>
#include <deque>

namespace jwm {

    enum class LogLevel {
        /**
         * Debug message log level.
         * Must be used for debug/development, for displaying debug information about the library.
         *
         * @note Will be disabled by default in release builds.
         */
        Debug = 0,

        /**
         * Warning message log level.
         * Must be used for displaying warning messages in debug/development modes.
         *
         * @note Will be disabled by default in release builds.
         */
        Warning = 1,

        /**
         * Error message log level.
         * Must be used for displaying non-fatal errors in development/debug/release builds.
         * This kind of error can appear primary because of invalid/incorrect input or
         * command sequence from the user. This types of errors must be handled and
         * system must recover after this with no side effects.
         *
         * @note Will be enabled by default in release builds.
         */
        Error = 2,

        /**
         * Fatal error message log level.
         * Must be used for displaying fatal errors in development/debug/release builds.
         * This is kind of error which leads to application crash, no recovery is possible.
         *
         * @note Will be enabled by default in release builds.
         */
        Fatal = 3
    };

    static const wchar_t* logLevelToStr(LogLevel level) {
        switch (level) {
            case LogLevel::Debug:
                return L"LogLevel::Debug";
            case LogLevel::Warning:
                return L"LogLevel::Warning";
            case LogLevel::Error:
                return L"LogLevel::Error";
            case LogLevel::Fatal:
                return L"LogLevel::Fatal";
            default:
                return L"LogLevel::Unknown";
        }
    };

    class LogEntry {
    public:
        LogEntry(std::wstring message, std::string file,
                 std::string function, std::time_t time,
                 unsigned long int line, LogLevel level);

        const std::wstring &getMessage() const { return _message; }
        const std::string &getFile() const { return _file; }
        const std::string &getFunction() const { return _function; }
        const std::time_t &getTime() const { return _time; }
        unsigned long int getLine() const { return _line; }
        LogLevel getLevel() const { return _level; }

    private:
        friend class Log;

        // Uses wchar_t string for correct unicode handling
        // NOTE: linux/mac (utf-8) and win (utf-16)
        std::wstring _message;
        std::string _file;
        std::string _function;
        std::time_t _time;
        unsigned long int _line;
        LogLevel _level;
    };

    /**
     * Log entry observer.
     * Allows to track when new entries are added into the log.
     */
    using LogEntryObserver = std::function<void(const LogEntry&)>;

    class Log {
    public:
        static const std::size_t ENTRIES_TO_KEEP = 100;
        static const LogLevel DEFAULT_LEVEL = LogLevel::Debug;

    public:
        Log() = default;
        Log(const Log&) = delete;
        Log(Log&&) = delete;
        ~Log() = default;

        void log(LogEntry&& entry);
        void setLevel(LogLevel level);
        void setEntriesToKeep(std::size_t numEntries);
        void addEntryObserver(LogEntryObserver func);

        void enableLogging(bool enable);
        bool checkLevel(LogLevel level) const;

    public:
        static Log& getInstance();

    private:
        void _eraseEntries();

    private:
        std::vector<LogEntryObserver> _entryObservers;
        std::deque<LogEntry> _entries;
        std::size_t _entriesToKeep = ENTRIES_TO_KEEP;
        LogLevel _level = DEFAULT_LEVEL;
        bool _enabled = true;
    };

    class LogBuilder: public std::wstringstream {
    public:
        LogBuilder(std::string file, std::string function,
                   unsigned long int line, LogLevel level, Log& log);

        void commit();

    private:
        std::string _file;
        std::string _function;
        std::time_t _time;
        unsigned long int _line;
        LogLevel _level;
        Log& _log;
    };

}

// Macro to check if message can be logged, if can than
// create builder and assemble message
#define JWM_LOG(logLevel, message)                          \
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

// Log macro proxy for logging global instance debug messages
#define JWM_DEBUG(message)      \
    JWM_LOG(::jwm::LogLevel::Debug, message)

// Log macro proxy for logging global instance warning messages
#define JWM_WARNING(message)    \
    JWM_LOG(::jwm::LogLevel::Warning, message)

// Log macro proxy for logging global instance error messages
#define JWM_ERROR(message)      \
    JWM_LOG(::jwm::LogLevel::Error, message)

// Log macro proxy for logging global instance fatal messages
#define JWM_FATAL(message)      \
    JWM_LOG(::jwm::LogLevel::Fatal, message)