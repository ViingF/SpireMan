#ifndef SPIRELIKE_LOGGER_HPP
#define SPIRELIKE_LOGGER_HPP

#include <fstream>
#include <mutex>
#include <sstream>
#include <string>

#include "model/Types.hpp"

enum class LogLevel {
    Trace = 0,
    Debug,
    Info,
    Warn,
    Error,
    Fatal
};

class Logger {
public:
    static Logger& instance();

    void init(
        const std::string& filePath = "logs/game.log",
        LogLevel minLevel = LogLevel::Info,
        bool consoleEnabled = true
    );

    void shutdown();

    void setLevel(LogLevel level);

    void log(
        LogLevel level,
        const char* file,
        int line,
        const char* function,
        const std::string& message
    );

private:
    Logger() = default;
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    std::mutex mutex_;
    std::ofstream file_;
    LogLevel minLevel_ = LogLevel::Info;
    bool consoleEnabled_ = true;
};

std::string toString(LogLevel level);
std::string toString(ErrorCode code);
std::string toString(SceneType sceneType);
std::string toString(BattleResult result);

#define LOG_AT(level, message)                                      \
    do {                                                           \
        std::ostringstream logStream__;                            \
        logStream__ << message;                                    \
        Logger::instance().log(                                    \
            level,                                                 \
            __FILE__,                                              \
            __LINE__,                                              \
            __func__,                                              \
            logStream__.str()                                      \
        );                                                         \
    } while (false)

#define LOG_TRACE(message) LOG_AT(LogLevel::Trace, message)
#define LOG_DEBUG(message) LOG_AT(LogLevel::Debug, message)
#define LOG_INFO(message)  LOG_AT(LogLevel::Info,  message)
#define LOG_WARN(message)  LOG_AT(LogLevel::Warn,  message)
#define LOG_ERROR(message) LOG_AT(LogLevel::Error, message)
#define LOG_FATAL(message) LOG_AT(LogLevel::Fatal, message)

#endif // SPIRELIKE_LOGGER_HPP
