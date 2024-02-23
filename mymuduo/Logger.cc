#include "Logger.h" 

#include "Timestamp.h"

Logger& Logger::instance()//获取唯一的实例对象
{
    static Logger logger;
    return logger;
}
void Logger::setLogLevel(int level)  //设置日志级别
{
    logLevel_ = level;
}
void Logger::log(std::string msg)    //写日志  [级别信息] 时间 ： msg
{
    switch(logLevel_) 
    {
    case INFO:
        std::cout << "[INFO]";
        break;
    case ERROR:
        std::cout << "[ERROR]";
        break;
    
    case FATAL:
        std::cout << "[FATAL]";
        break;
    case DEBUG:
        std::cout << "[DEBUG]";
        break;
    }

    //打印时间和msg
    std::cout << Timestamp::now().toString() << msg << std::endl;


}