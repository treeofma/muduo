#pragma once

#include <string>

#include "noncopyable.h"

//写宏的时候换行要加"\"
#define LOG_INFO(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance();\
        logger.setLogLevel(INFO);\
        char buf[1024] = {0};\
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
    } while(0)

#define LOG_ERROR(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance();\
        logger.setLogLevel(ERROR);\
        char buf[1024] = {0};\
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
    } while(0)


#define LOG_FATAL(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance();\
        logger.setLogLevel(FATAL);\
        char buf[1024] = {0};\
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
        exit(-1);\
    } while(0)

#ifdef MUDEBUG
#define LOG_DEBUG(logmsgFormat, ...) \
    do \
    { \
        Logger &logger = Logger::instance();\
        logger.setLogLevel(DEBUG);\
        char buf[1024] = {0};\
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
    } while(0)
#else
    #define LOG_DEBUG(logmsgFormat, ...)  //频繁打印DEBUG影响效率，使用宏定义不需要不打印DEBUG
#endif
//定义日志级别  INFO  ERROR  FATAL DEBUG
enum LogLevel
{
    INFO,  //普通信息
    ERROR, //错误信息
    FATAL, //core信息
    DEBUG, //调试信息
};

//输出一个日志类
class Logger : noncopyable   //设计为单例模式
{
public:
    static Logger& instance(); //获取唯一的实例对象
    void setLogLevel(int level);  //设置日志级别
    void log(std::string msg);    //写日志
private:
    int logLevel_;  
    Logger(){};
};
