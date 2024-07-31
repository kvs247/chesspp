#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <mutex>

class Logger
{
public:
    static Logger &getInstance();
    bool log(const std::string &message);
    bool isOpen() const { return log_file.is_open(); }

    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

private:
    Logger();
    ~Logger();

    std::ofstream log_file;
    std::mutex mutex;

    static Logger *instance;
    static std::mutex instance_mutex;
};

extern Logger &logger;

#endif
