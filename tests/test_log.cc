/*
 * @Description: 日志系统测试
 * @Version: 1.0
 * @Autor: huang
 * @Date: 2021-12-29 12:53:54
 * @LastEditors: huang
 * @LastEditTime: 2021-12-29 13:10:23
 */
#include <iostream>
#include "../src/log.h"
#include "../src/util.h"
#include <memory.h>
int main(int argc, char** argv) {
    auto logger = std::make_shared<sylar::Logger>();
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));

    sylar::FileLogAppender::ptr file_appender(new sylar::FileLogAppender("./log.txt"));
    //ylar::LogFormatter::ptr fmt(new sylar::LogFormatter("%d%T%p%T%m%n"));
    //file_appender->setFormatter(fmt);
    file_appender->setLevel(sylar::LogLevel::INFO);
    logger->addAppender(file_appender);

    sylar::LogEvent::ptr event(new sylar::LogEvent(logger, sylar::LogLevel::ERROR, __FILE__, __LINE__, 32, 0, 12345, 54321, "Thread name"));
    event->getStringStream() << "hello sylar log";
    logger->log(sylar::LogLevel::ERROR, event);
    std::cout << "hello sylar log" << std::endl;

    __LOG_DEBUG(logger) << "test macro";
    __LOG_ERROR(logger) << "test macro error";

    __LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

    auto Mgr = sylar::LoggerMgr::GetInstance()->getLogger("xx");
    __LOG_INFO(Mgr) << "xxx";
    return 0;
}
