#include "Logger.hpp"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>

namespace {

std::string nowString()
{
    using namespace std::chrono;

    const auto now = system_clock::now();
    const auto time = system_clock::to_time_t(now);
    const auto ms = duration_cast<milliseconds>(
        now.time_since_epoch()
    ) % 1000;

    std::tm tm{};

#if defined(_WIN32)
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << '.'
        << std::setw(3)
        << std::setfill('0')
        << ms.count();

    return oss.str();
}

std::string shortFileName(const char* file)
{
    return std::filesystem::path(file).filename().string();
}

}

Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

Logger::~Logger()
{
    shutdown();
}

void Logger::init(
    const std::string& filePath,
    LogLevel minLevel,
    bool consoleEnabled
)
{
    std::lock_guard<std::mutex> lock(mutex_);

    minLevel_ = minLevel;
    consoleEnabled_ = consoleEnabled;

    if (file_.is_open()) {
        file_.close();
    }

    const auto parentPath =
        std::filesystem::path(filePath).parent_path();

    if (!parentPath.empty()) {
        std::filesystem::create_directories(parentPath);
    }

    file_.open(filePath, std::ios::app);

    if (!file_.is_open()) {
        std::cerr << "[LOGGER] Failed to open log file: "
                  << filePath
                  << std::endl;
    }
}

void Logger::shutdown()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (file_.is_open()) {
        file_.flush();
        file_.close();
    }
}

void Logger::setLevel(LogLevel level)
{
    std::lock_guard<std::mutex> lock(mutex_);
    minLevel_ = level;
}

void Logger::log(
    LogLevel level,
    const char* file,
    int line,
    const char* function,
    const std::string& message
)
{
    if (static_cast<int>(level) < static_cast<int>(minLevel_)) {
        return;
    }

    std::ostringstream lineStream;

    lineStream
        << '[' << nowString() << ']'
        << '[' << toString(level) << ']'
        << '[' << shortFileName(file) << ':' << line << ']'
        << '[' << function << "] "
        << message;

    const std::string output = lineStream.str();

    std::lock_guard<std::mutex> lock(mutex_);

    if (consoleEnabled_) {
        if (
            level == LogLevel::Error ||
            level == LogLevel::Fatal
        ) {
            std::cerr << output << std::endl;
        } else {
            std::cout << output << std::endl;
        }
    }

    if (file_.is_open()) {
        file_ << output << '\n';

        if (
            level == LogLevel::Error ||
            level == LogLevel::Fatal
        ) {
            file_.flush();
        }
    }
}

std::string toString(LogLevel level)
{
    switch (level) {
    case LogLevel::Trace: return "TRACE";
    case LogLevel::Debug: return "DEBUG";
    case LogLevel::Info:  return "INFO";
    case LogLevel::Warn:  return "WARN";
    case LogLevel::Error: return "ERROR";
    case LogLevel::Fatal: return "FATAL";
    default:              return "UNKNOWN";
    }
}

std::string toString(ErrorCode code)
{
    switch (code) {
    case ErrorCode::OK:                   return "OK";
    case ErrorCode::FILE_NOT_FOUND:       return "FILE_NOT_FOUND";
    case ErrorCode::RESOURCE_LOAD_FAILED: return "RESOURCE_LOAD_FAILED";
    case ErrorCode::DATA_FORMAT_ERROR:    return "DATA_FORMAT_ERROR";
    case ErrorCode::INVALID_ENUM_VALUE:   return "INVALID_ENUM_VALUE";
    case ErrorCode::DUPLICATE_ID:         return "DUPLICATE_ID";
    case ErrorCode::INVALID_CARD_ID:      return "INVALID_CARD_ID";
    case ErrorCode::CARD_NOT_IN_HAND:     return "CARD_NOT_IN_HAND";
    case ErrorCode::NOT_ENOUGH_ENERGY:    return "NOT_ENOUGH_ENERGY";
    case ErrorCode::INVALID_TARGET:       return "INVALID_TARGET";
    case ErrorCode::CARD_UNPLAYABLE:      return "CARD_UNPLAYABLE";
    case ErrorCode::INVALID_SCENE_STATE:  return "INVALID_SCENE_STATE";
    case ErrorCode::NULL_POINTER:         return "NULL_POINTER";
    case ErrorCode::UNKNOWN_ERROR:        return "UNKNOWN_ERROR";
    default:                              return "UNKNOWN_ERROR_CODE";
    }
}

std::string toString(SceneType sceneType)
{
    switch (sceneType) {
    case SceneType::None:            return "None";
    case SceneType::Menu:            return "Menu";
    case SceneType::CharacterSelect: return "CharacterSelect";
    case SceneType::Map:             return "Map";
    case SceneType::Combat:          return "Combat";
    case SceneType::Reward:          return "Reward";
    case SceneType::Event:           return "Event";
    case SceneType::Campfire:        return "Campfire";
    case SceneType::Shop:            return "Shop";
    case SceneType::CardRemove:      return "CardRemove";
    case SceneType::End:             return "End";
    default:                         return "UnknownScene";
    }
}

std::string toString(BattleResult result)
{
    switch (result) {
    case BattleResult::Ongoing: return "Ongoing";
    case BattleResult::Victory: return "Victory";
    case BattleResult::Defeat:  return "Defeat";
    default:                    return "UnknownBattleResult";
    }
}
