/**
 * @file log.h
 * @brief 日志模块
 * @author huang
 * @date 2021-12-29
 */
#ifndef __LOG_H__
#define __LOG_H__


#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include "singleton.h"
#include "util.h"
#include "thread.h"

/**
 * @brief 使用流方式将日志级别为level的日志写入logger
 */
#define __LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent( \
            logger, level, __FILE__, __LINE__, 0, sylar::GetThreadId(), \
            sylar::GetFiberId(), time(NULL), sylar::Thread::GetName()))).getStringStream()

#define __LOG_DEBUG(logger) __LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define __LOG_INFO(logger)  __LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define __LOG_WARN(logger)  __LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define __LOG_ERROR(logger) __LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define __LOG_FATAL(logger) __LOG_LEVEL(logger, sylar::LogLevel::FATAL)

/**
 * @brief 以格式化方式将日志级别写入logger
 * @param[in] logger    格式器
 * @param[in] level     日志级别
 * @param[in] fmt       格式器
 */
#define __LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
       sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent( \
            logger, level, __FILE__, __LINE__, 0, sylar::GetThreadId(), \
            sylar::GetFiberId(), time(NULL), "Unknow"))).getEvent()->format(fmt, __VA_ARGS__)

#define __LOG_FMT_DEBUG(logger, fmt, ...)   __LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define __LOG_FMT_INFO(logger, fmt, ...)    __LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)
#define __LOG_FMT_WARN(logger, fmt, ...)    __LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)
#define __LOG_FMT_ERROR(logger, fmt, ...)   __LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)
#define __LOG_FMT_FATAL(logger, fmt, ...)   __LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

#define __LOG_ROOT sylar::LoggerMgr::GetInstance()->getRoot()

#define __LOG_NAME(name) sylar::LoggerMgr::GetInstance()->getLogger(name)

namespace sylar {

class Logger;
class LoggerManager;

class LogLevel {
public:
    /**
     * @brief 枚举日志级别
     */
    enum Level 
    {
        UNKNOW  = 0,
        DEBUG   = 1,
        INFO    = 2,
        WARN    = 3,
        ERROR   = 4,
        FATAL   = 5,
    };
    
    /**
     * @brief 日志级别转文本类型
     * @param[in] level 日志级别
     */
    static const char* ToString(LogLevel::Level level);

    /**
     * @brief 文本转换为日志级别
     * @param[in] str   日志级别文本
     */
    static LogLevel::Level FromString(const std::string& str);
};

/**
 * @brief 日志事件
 */
class LogEvent 
{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    /**
     * @brief 构造函数
     * @param[in] logger        日志器
     * @param[in] level         日志级别
     * @param[in] file          文件名
     * @param[in] line          行号
     * @param[in] elapse        程序启动耗时
     * @param[in] thread_id     线程id
     * @param[in] fiber_id      协程id
     * @param[in] time          日志时间
     * @param[in] thread_name   线程名称
     */
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
            ,const char* file, int32_t line, uint32_t elapse
            ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
            ,const std::string& thread_name);

    /**
     * @brief 返回文件名
     */
    const char* getFile() const { return m_file;}
    
    /**
     * @brief 返回行号
     */
    int32_t getLine() const { return m_line;}
    
    /**
     * @brief 返回耗时
     */
    uint32_t getElapse() const { return m_elapse;}
    
    /**
     * @brief 返回线程Id
     */
    uint32_t getThreadId() const { return m_threadId;}
    
    /**
     * @brief 返回协程Id
     */
    uint32_t getFiberId() const { return m_fiberId;}

    /**
     * @brief 返回时间
     */
    uint64_t getTime() const { return m_time;}
    
    /**
     * @brief 返回线程名
     */
    const std::string& getThreadName() const { return m_threadName;}

    /**
     * @brief 返回日志流内容 
     */
    const std::string getContent() const { return m_ss.str();}

    /**
     * @brief 返回日志等级
     */
    LogLevel::Level getLevel() const { return m_level;}

    /**
     * @brief 返回日志器
     */
    std::shared_ptr<Logger> getLogger() const { return m_logger;}

    /**
     * @brief 返回日志流
     */
    std::stringstream& getStringStream() { return m_ss;}

    /**
     * @brief 格式化写入日志内容
     */

    void format(const char* fmt, ...);
    void formatExecute(const char* fmt, va_list al);
private:
    // 文件名
    const char* m_file = nullptr;
    // 行号
    int32_t m_line = 0;
    // 程序启动后经过的时间
    uint32_t m_elapse = 0;
    // 线程ID
    uint32_t m_threadId = 0;
    // 协程ID
    uint32_t m_fiberId = 0;
    // 时间戳
    uint64_t m_time = 0;
    // 线程名
    std::string m_threadName;
    // 日志流
    std::stringstream m_ss;
    // 日志器
    std::shared_ptr<Logger> m_logger;
    // 日志等级
    LogLevel::Level m_level;
};

/**
 * @brief 日志事件装饰器
 */
class LogEventWrap
{
public:
    
    /**
     * @brief 构造函数
     * @param[in] event
     */
    LogEventWrap(LogEvent::ptr event);
    
    /**
     * @brief 析构函数，释放时将event写入logger中
     */
    ~LogEventWrap();
    
    /**
     * @brief 获取日志事件
     */
    LogEvent::ptr getEvent() const {  return m_event;}

    /**
     * @brief 获取日志流
     */
    std::stringstream& getStringStream();

private:
    LogEvent::ptr m_event;
};

/**
 * @brief 日志格式化器
 */
class LogFormatter
{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    /**
     * @brief 格式化器构造函数
     * @param[in] pattern 格式模板
     * @details
     *  %m 消息
     *  %p 日志级别
     *  %r 累计毫秒数
     *  %c 日志名
     *  %t 线程id
     *  %n 换行
     *  %d 时间
     *  %f 文件名
     *  %l 行号
     *  %T 制表符
     *  %N 线程名称
     *
     *  默认格式 “%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T[%p]%T[%c]%T%f:%l%T%m%n”
     */
    LogFormatter(const std::string& pattern);
    
    /**
     * @brief 返回格式化日志文本
     * @param[in] logger    日志器
     * @param[in] level     日志级别
     * @param[in] event     日志事件
     */
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

    /**
     * @brief 日志内容格式化
     */
    class FormatItem
    {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        /**
         * @brief 析构函数
         */
        virtual ~FormatItem() {}

        /**
         * @brief 格式化日志,纯虚方法
         * param[in, out]   os          日志输出流
         * param[in]        logger      日志器
         * param[in]        level       日志级别
         * param[in]        event       日志事件
         */
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };
    
    /**
     * @brief 初始化，解析日志模板
     */
    void init();
    
    bool isError() const { return m_error;}

    /**
     * @brief 返回日志模板
     */
    const std::string getPattern() const { return m_pattern;}

private:
    //日志模板格式
    std::string m_pattern;
    // 解析后格式
    std::vector<FormatItem::ptr> m_items;
    // 解析是否有错误
    bool m_error = false;
    
};

/**
 * @brief 日志输出地
 */
class LogAppender
{
friend class Logger;

public:
    typedef std::shared_ptr<LogAppender> ptr;
    typedef Spinlock Mutextype;

    /**
     * @brief 析构函数
     */
    virtual ~LogAppender() {}
    
    /**
     * @brief 写入日志
     * @param[in] logger    日志器
     * @param[in] level     日志级别
     * @param[in] event     日志事件
     */
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    /**
     * @brief 将日志输出地的配置转成YAML String
     */
    virtual std::string toYAMLString() = 0;

    /**
     * @brief 更改日志格式器
     */
    void setFormatter(LogFormatter::ptr val);

    /**
     * @brief 更改日志格式器
     */
    void setFormatter(const std::string& val);

    /**
     * @brief 获取日志格式器
     */
    LogFormatter::ptr getFormatter() const;

    /**
     * @brief 设置日志级别
     */
    void setLevel(LogLevel::Level val) { m_level = val;}

    /**
     * @brief 获取日志级别
     */
    LogLevel::Level getLevel() const {return m_level;}

protected:
    // 日志级别
    LogLevel::Level m_level = LogLevel::DEBUG;
    // 是否有自己的日志格式器
    bool m_hasFormatter = false;
    mutable Mutextype m_mutex;
    LogFormatter::ptr m_formatter;

};

/**
 * @brief 日志器
 */
class Logger : public std::enable_shared_from_this<Logger>
{
friend class LoggerManager;
public:
    typedef std::shared_ptr<Logger> ptr;
    typedef Spinlock Mutextype;
    
    /**
     * @brief 构造函数
     * param[in] name 日志器名称
     */
    Logger(const std::string& name = "root");

    /**
     * @brief 写日志
     * @param[in] level 日志级别
     * @param[in] event 日志事件
     */
    void log(LogLevel::Level level, LogEvent::ptr event);

    /**
     * @brief 写debug级别日志
     */
    void debug(LogEvent::ptr event);

    /**
     * @brief 写info级别日志
     */
    void info(LogEvent::ptr event);

    /**
     * @brief 写warn级别日志
     */
    void warn(LogEvent::ptr event);

    /**
     * @brief 写error级别日志
     */
    void error(LogEvent::ptr event);

    /**
     * @brief 写fatal级别日志
     */
    void fatal(LogEvent::ptr event);

    /**
     * @brief 添加日志目标
     * @param[in] appender 日志目标
     */
    void addAppender(LogAppender::ptr appender);

    /**
     * @brief 删除日志目标
     * @param[in] appender 日志目标
     */
    void delAppender(LogAppender::ptr appender);

    /**
     * @brief 清空日志目标
     */
    void clearAppenders();

    /**
     * @brief 返回日志级别
     */
    LogLevel::Level getLevel() const { return m_level;}

    /**
     * @brief 设置日志级别
     */
    void setLevel(LogLevel::Level val) {m_level = val;}

    /**
     * @brief 返回日志名
     */
    const std::string& getName() const { return m_name;}

    /**
     * @brief 设置日志格式器
     */
    void setFormatter(LogFormatter::ptr val);

    /**
     * @brief 设置日志格式模板
     */
    void setFormatter(const std::string& val);

    /**
     * @brief 获取日志格式器
     */
    LogFormatter::ptr getFormatter() const;

    /**
     * @brief 日志器配置转换成YAML String
     */
    std::string toYAMLString();

private:
    // 日志名
    std::string m_name;
    // 日志级别
    LogLevel::Level m_level;
    // 锁
    mutable Mutextype m_mutex;
    // 日志目标集合
    std::list<LogAppender::ptr> m_appenders;
    // 日志格式器
    LogFormatter::ptr m_formatter;
    // 主日志器
    Logger::ptr m_root;
};

class StdoutLogAppender : public LogAppender
{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYAMLString() override;
private:
};

class FileLogAppender : public LogAppender 
{
public:
    typedef std::shared_ptr<FileLogAppender> ptd;
    FileLogAppender(const std::string& filename);
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYAMLString() override;

    /**
     * @brief 重打开文件
     * @return 成功返回true
     */
    bool reopen();
private:
    // 文件路径
    std::string m_filename;
    // 文件流
    std::ofstream m_filestream;
    // 上次打开时间
    uint64_t m_lastTime = 0;
};

/**  
 * @brief 日志管理器
 */
class LoggerManager {
public:
    typedef Spinlock Mutextype;

    /**
     * @brief 构造函数
     */
    LoggerManager();

    /**
     * @brief 获取控制器
     * @param[in] name 日志器名称
     */
    Logger::ptr getLogger(const std::string& name);
    
    /**
     * @brief 初始化
     */
    void init();

    /**
     * @brief 返回主日志器
     */
    Logger::ptr getRoot() const { return m_root; }

    /**
     * @brief 将所有日志器配置转换成YAML String
     */
    std::string toYAMLString();

private:
    // 锁
    mutable Mutextype m_mutex;
    // 日志器容器
    std::map<std::string, Logger::ptr> m_Loggers;
    // 主日志器
    Logger::ptr m_root;
};

typedef sylar::Singleton<LoggerManager> LoggerMgr;

}

#endif       


