#include "logger.h"
#include <iostream>

namespace latinex
{

void Logger::debug(const std::string& clazz, const std::string& message)
{
    std::cout << "[Debug] " << clazz << ": " << message << std::endl;
}

void Logger::warn(const std::string& clazz, const std::string& message)
{
    std::cout << "[Warning] " << clazz << ": " << message << std::endl;
}

void Logger::error(const std::string& clazz, const std::string& message)
{
    std::cerr << "[Error] " << clazz << ": " << message << std::endl;
}

} // namespace latinex
