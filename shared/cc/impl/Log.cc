#include "Log.hh"
#include <cassert>
#include <cstdio>

jwm::LogEntry::LogEntry(std::wstring message, std::string file,
                        std::string function, std::time_t time,
                        unsigned long line, jwm::LogLevel level)
    : _message(std::move(message)),
      _file(std::move(file)),
      _function(std::move(function)),
      _time(time),
      _line(line),
      _level(level) {

}

void jwm::Log::log(jwm::LogEntry &&entry) {
    if (checkLevel(entry.getLevel())) {
        _entries.push_back(std::move(entry));
        auto& pushed = _entries.back();

        for (auto& observer: _entryObservers)
            observer(pushed);

        _eraseEntries();
    }
}

void jwm::Log::setLevel(jwm::LogLevel level) {
    _level = level;
}

void jwm::Log::setEntriesToKeep(std::size_t numEntries) {
    assert(numEntries > 0);
    _entriesToKeep = numEntries;
}

void jwm::Log::addEntryObserver(jwm::LogEntryObserver func) {
    _entryObservers.push_back(std::move(func));
}

void jwm::Log::enableLogging(bool enable) {
    _enabled = enable;
}

bool jwm::Log::checkLevel(jwm::LogLevel level) const {
    return _enabled && level >= _level;
}

void jwm::Log::_eraseEntries() {
    while (_entries.size() > _entriesToKeep)
        _entries.pop_front();
}

jwm::Log &jwm::Log::getInstance() {
    static Log gLog;
    static bool firstTime = true;

    if (firstTime) {
        // Default settings
        gLog.setEntriesToKeep(Log::ENTRIES_TO_KEEP);
        gLog.setLevel(Log::DEFAULT_LEVEL);

        // Configure stdout/stderr sink observer
        gLog.addEntryObserver([](const LogEntry& entry){
            auto level = entry.getLevel();
            auto output = level >= LogLevel::Error? stderr: stdout;
            char buffer[128];
            std::tm timeStruct{};

#ifdef WIN32
            const wchar_t formatStr[] = L"[%ls][%hs](file %hs; line %lu; function %hs) what: %ls\n";
            localtime_s(&timeStruct, &entry.getTime());
#else
            const wchar_t formatStr[] = L"[%ls][%s](file %s; line %lu; function %s) what: %ls\n";
            localtime_r(&entry.getTime(), &timeStruct);
#endif

            std::strftime(buffer, sizeof(buffer), "%a %b %e %T %Y", &timeStruct);
            fwprintf(output, formatStr,
                     logLevelToStr(level), buffer,
                     entry.getFile().c_str(), entry.getLine(),
                     entry.getFunction().c_str(), entry.getMessage().c_str());
        });

        firstTime = false;
    }

    return gLog;
}

jwm::LogBuilder::LogBuilder(std::string file, std::string function,
                            unsigned long line, jwm::LogLevel level, jwm::Log &log)
    : _file(std::move(file)),
      _function(std::move(function)),
      _time(std::time(nullptr)),
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
        _time,
        _line,
        _level
    );

    _log.log(std::move(entry));
    this->clear();
}
