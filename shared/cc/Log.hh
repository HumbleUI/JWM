#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <memory>

namespace jwm {

    class LogEntry {
    public:
        LogEntry(std::wstring message, std::string file,
                 std::string function, unsigned long int line, bool verbose);

        const std::wstring &getMessage() const { return _message; }
        const std::string &getFile() const { return _file; }
        const std::string &getFunction() const { return _function; }
        unsigned long int getLine() const { return _line; }
        bool isVerbose() const { return _verbose; }

    private:
        friend class Log;

        // Uses wchar_t string for correct unicode handling
        // NOTE: linux/mac (utf-32) and win (utf-16)
        std::wstring _message;
        std::string _file;
        std::string _function;
        unsigned long int _line;
        bool _verbose;
    };

    /**
     * Log entry listener.
     * Allows to track when new entries are added into the log.
     */
    class LogListenerProxy;

    class Log {
    public:
        Log() = default;
        Log(const Log&) = delete;
        Log(Log&&) = delete;
        ~Log() = default;

        void log(const LogEntry& entry);
        void setVerbose(bool verbose);
        void setListener(class LogListenerProxy *listener);

        void enable(bool enabled);
        bool checkLevel(bool verbose) const;

    public:
        static Log& getInstance();

    private:
        class LogListenerProxy* _listener = nullptr;
        bool _verbose = false;
        bool _enabled = false;
    };

    /** Used to compose complex messages in '<<' operator style */
    class LogBuilder: public std::wstringstream {
    public:
        LogBuilder(std::string file, std::string function,
                   unsigned long int line, bool verbose, Log& log);

        void commit();

    private:
        std::string _file;
        std::string _function;
        unsigned long int _line;
        bool _verbose;
        Log& _log;
    };

}

#define JWM_LEVEL_VERBOSE true
#define JWM_LEVEL_LOG false

// Macro to check if message can be logged, if can than
// create builder and assemble message
#define JWM_LOG_CAT(verbose, message)                       \
    do {                                                    \
        auto& __log = ::jwm::Log::getInstance();            \
        if (__log.checkLevel(verbose)) {                    \
            LogBuilder __builder(                           \
                __FILE__,                                   \
                __FUNCTION__,                               \
                static_cast<unsigned long int>(__LINE__),   \
                verbose,                                    \
                __log                                       \
            );                                              \
            __builder << message;                           \
            __builder.commit();                             \
        } \
    }  while (false);

// Log macro proxy for logging global instance verbose (debug) messages
#define JWM_VERBOSE(message)        \
    JWM_LOG_CAT(JWM_LEVEL_VERBOSE, message)

// Log macro proxy for logging global instance log (default) messages
#define JWM_LOG(message)            \
    JWM_LOG_CAT(JWM_LEVEL_LOG, message)
