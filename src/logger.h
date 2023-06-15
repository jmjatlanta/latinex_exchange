#pragma once
#include <string>

namespace latinex
{

class Logger
{
    public:
    static Logger* getInstance() { static Logger logger; return &logger; }

    void debug(const std::string& clazz, const std::string& message);
    void error(const std::string& clazz, const std::string& message);
    void warn(const std::string& clazz, const std::string& message);

    private:
    Logger() {}
};

} // namespace latinex