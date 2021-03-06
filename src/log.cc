
#include "log.h"
#include "util.h"
#include <map>
#include <iostream>
#include <functional>
#include <string>
#include <time.h>
#include <stdarg.h>
#include "config.h"

namespace sylar 
{
    const char* LogLevel::ToString(LogLevel::Level level){
        switch(level){
#define str(name) \
            case LogLevel::name: \
                return #name;
            str(UNKNOW);
            str(DEBUG);
            str(INFO);
            str(WARN);
            str(ERROR);
            str(FATAL);
#undef str
            default:
                return "UNKNOW";
        }
        return "UNKNOW";
    }
    
    LogLevel::Level LogLevel::FromString(const std::string& str){
#define toLevel(name, lname) \
        if(str == #name || str == #lname) \
            return LogLevel::name;
        toLevel(DEBUG, debug);
        toLevel(INFO, info);
        toLevel(WARN, warn);
        toLevel(ERROR, error);
        toLevel(FATAL, fatal);
        return LogLevel::UNKNOW;
#undef toLevel
        
    }

    LogEventWrap::LogEventWrap(LogEvent::ptr event)
        :m_event(event) {}
    
    LogEventWrap::~LogEventWrap() {
        m_event->getLogger()->log(m_event->getLevel(), m_event);
    }
    
    std::stringstream& LogEventWrap::getStringStream() {
        return m_event->getStringStream();
    }

    void LogEvent::format(const char* fmt, ...) {
        va_list al;
        va_start(al, fmt);
        formatExecute(fmt, al);
        va_end(al);
    }

    void LogEvent::formatExecute(const char* fmt, va_list al) {
        char* buf = nullptr;
        int len = vasprintf(&buf, fmt, al);
        if(len != -1) {
            m_ss << std::string(buf, len);
            free(buf);
        }
    }

    void LogAppender::setFormatter(LogFormatter::ptr val){
        Mutextype::Lock lock(m_mutex);
        m_formatter = val;
        if(m_formatter){
            m_hasFormatter = true;
        } else {
            m_hasFormatter = false;
        }
    }

    void LogAppender::setFormatter(const std::string& val){
        //std::cout << "---" << val << std::endl;
        sylar::LogFormatter::ptr new_val(new sylar::LogFormatter(val));
        if(new_val->isError()){
            std::cout << "LogAppender setFormatter value=" << val 
                <<" error: invalid formatter." << std::endl;
            return;
        }
        setFormatter(new_val);
    }

    LogFormatter::ptr LogAppender::getFormatter() const {
        Mutextype::Lock lock(m_mutex);
        return m_formatter;
    }

    class MessageFormatItem : public LogFormatter::FormatItem {
        public:
            MessageFormatItem(const std::string& str = "") {}
            void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                os << event->getContent();
            }
    };
    
    class LevelFormatItem : public LogFormatter::FormatItem {
        public:
            LevelFormatItem(const std::string& str = "") {}
            void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                os << LogLevel::ToString(level);
            }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem {
        public:
            ElapseFormatItem(const std::string& str = "") {}
            void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                os << event->getElapse();
            }
    };

    class NameFormatItem : public LogFormatter::FormatItem {
        public:
            NameFormatItem(const std::string& str = "") {}
            void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                os << event->getLogger()->getName();
            }
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem {
        public:
            ThreadIdFormatItem(const std::string& str = "") {}
            void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                os << event->getThreadId();
            }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem {
        public:
            FiberIdFormatItem(const std::string& str = "") {}
            void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                os << event->getFiberId();
            }
    };

    class ThreadNameFormatItem : public LogFormatter::FormatItem {
        public:
            ThreadNameFormatItem(const std::string& str = "") {}
            void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                os << event->getThreadName();
            }
    };
    
    class DateTimeFormatItem : public LogFormatter::FormatItem {
        public:
            DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
                :m_time_format(format){
                    if(m_time_format.empty())
                        m_time_format = "%Y-%m-%d %H:%M:%S";
                }

            void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                struct tm tm;
                time_t time = event->getTime();
                localtime_r(&time,&tm);
                char buf[64];
                strftime(buf, sizeof(buf), m_time_format.c_str(), &tm);
                os<<buf;
            }
        private:
            std::string m_time_format;
    };
    
    class FilenameFormatItem : public LogFormatter::FormatItem {
        public:
            FilenameFormatItem(const std::string& str = "") {}
            void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                os<<event->getFile();
            }
    };

    class LineFormatItem : public LogFormatter::FormatItem {
        public:
            LineFormatItem(const std::string& str = "") {}
            void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                os<<event->getLine();
            }
    };

   class NewLineFormatItem : public LogFormatter::FormatItem {
        public:
            NewLineFormatItem(const std::string& str = "") {}
            void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                os<<std::endl;
            }
    };

    class StringFormatItem : public LogFormatter::FormatItem {
        public:
            StringFormatItem(const std::string& str) 
                :m_string(str) {}
            void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                os<<m_string;
            }
        private:
            std::string m_string;
    };
    
    class TabFormatItem : public LogFormatter::FormatItem {
        public:
            TabFormatItem(const std::string& str = "") {}
            void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
                os<<"\t";
            }
    };

    LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
            ,const char* file, int32_t line, uint32_t elapse
            ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
            ,const std::string& thread_name) 
        :m_file(file),
        m_line(line),
        m_elapse(elapse),
        m_threadId(thread_id),
        m_fiberId(fiber_id),
        m_time(time),
        m_threadName(thread_name),
        m_logger(logger),
        m_level(level)
    {}

    Logger::Logger(const std::string& name)
        :m_name(name), m_level(LogLevel::DEBUG){
            m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));

        /*if(name == "root") {
            addAppender(LogAppender::ptr(new StdoutLogAppender));
        }*/
    }

    void Logger::setFormatter(LogFormatter::ptr val) {
        
        Mutextype::Lock lock(m_mutex);
        m_formatter = val;
        
        for(auto& i:m_appenders) {
            Mutextype::Lock lockappender(i->m_mutex);
            if(!i->m_hasFormatter){
                i->m_formatter = m_formatter;
            }
        }

    }

    void Logger::setFormatter(const std::string& val){
        // std::cout << "---" << val << std::endl;
        sylar::LogFormatter::ptr new_val(new sylar::LogFormatter(val));
        if(new_val->isError()){
            std::cout << "Logger setFormatter name=" << m_name
                << " value=" << val <<" invalid formatter."
                << std::endl;
            return;
        }
        setFormatter(new_val);
    }

    LogFormatter::ptr Logger::getFormatter() const {
        Mutextype::Lock lock(m_mutex);
        return m_formatter;
    }

    void Logger::addAppender(LogAppender::ptr appender){
        Mutextype::Lock lock(m_mutex);
        if(!appender->getFormatter()) {
            // ?????????Appender?????????formatter???????????????Logger???formatter????????????hasformatter?????????
            //appender??????formatter?????????
            Mutextype::Lock lockappender(appender->m_mutex);
            appender->m_formatter = m_formatter;
        }
        m_appenders.push_back(appender);
    }

    void Logger::delAppender(LogAppender::ptr appender){
        //??????
        Mutextype::Lock lock(m_mutex);
        for(auto it = m_appenders.begin(); it != m_appenders.end(); ++it){
            if(*it == appender){
                m_appenders.erase(it);
                break;
            }
        }
    }

    void Logger::clearAppenders() {
        Mutextype::Lock lock(m_mutex);
        m_appenders.clear();
    }
     
    void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
        if(level >= m_level){
            Mutextype::Lock lock(m_mutex);
            if(!m_appenders.empty()){
                //appender???????????????appenders????????????
                for(auto& i : m_appenders){
                    i->log(shared_from_this(), level, event);
                }
            } else if (m_root){
                // ??????appenders???????????????????????????
                m_root->log(level, event);
            }
        }
    }
    
    void Logger::debug(LogEvent::ptr event) {
        log(LogLevel::DEBUG, event);
    }

    void Logger::info(LogEvent::ptr event) {
        log(LogLevel::INFO, event);
    }

    void Logger::warn(LogEvent::ptr event) {
        log(LogLevel::WARN, event);
    }

    void Logger::error(LogEvent::ptr event) {
        log(LogLevel::ERROR, event);
    }

    void Logger::fatal(LogEvent::ptr event) {
        log(LogLevel::FATAL, event);
    }
    
    FileLogAppender::FileLogAppender(const std::string& filename)
        :m_filename(filename) {
            reopen();
        }

    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        if(level >= m_level) {
            uint64_t now = event->getTime();
            if(now >= (m_lastTime + 3)) {
                reopen();
                m_lastTime = now;
            }
            //??????
            Mutextype::Lock lock(m_mutex);
            if(!m_formatter->format(m_filestream, logger, level, event)){
                std::cout << "error" << std::endl;
            }
        }
    }

    std::string FileLogAppender::toYAMLString() {
        Mutextype::Lock lock(m_mutex);
        YAML::Node node;
        node["type"] = "FileLogAppender";
        node["file"] = m_filename;
        if(m_level != LogLevel::UNKNOW) {
            // ???????????????appender??????????????????????????????
            node["level"] = LogLevel::ToString(m_level);
        }
        if(m_hasFormatter && m_formatter) {
            node["formatter"] = m_formatter->getPattern();
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    bool FileLogAppender::reopen() {
        //??????
        Mutextype::Lock lock(m_mutex);
        if(m_filestream){
            m_filestream.close();
        }
        return FSUtil::OpenForWrite(m_filestream, m_filename, std::ios::app);
    }

    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        if(level >= m_level) {
            // ??????
            Mutextype::Lock lock(m_mutex);
            m_formatter->format(std::cout, logger, level, event);
        }
    }

    std::string StdoutLogAppender::toYAMLString() {
        Mutextype::Lock lock(m_mutex);
        YAML::Node node;
        node["type"] = "StdoutAppender";
        if(m_level != LogLevel::UNKNOW) {
            // ???????????????appender??????????????????????????????
            node["level"] = LogLevel::ToString(m_level);
        }
        if(m_hasFormatter && m_formatter) {
            node["formatter"] = m_formatter->getPattern();
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    LogFormatter::LogFormatter(const std::string& pattern)
        :m_pattern(pattern){
            init();
        }

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        std::stringstream ss;
        for(auto& i : m_items) {
            i->format(ss, logger, level, event);
        }
        return ss.str();
    }

    std::ostream& LogFormatter::format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        for(auto& i : m_items) {
            i->format(ofs, logger, level, event);
        }
        return ofs;
    }
    
    void LogFormatter::init() {
        static std::map<char, std::function<FormatItem::ptr(const std::string& str)> > str_2_formatItem = {
#define XX(str, C) \
        {str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

        XX('m', MessageFormatItem),           //m:??????
        XX('p', LevelFormatItem),             //p:????????????
        XX('r', ElapseFormatItem),            //r:???????????????
        XX('c', NameFormatItem),              //c:????????????
        XX('t', ThreadIdFormatItem),          //t:??????id
        XX('n', NewLineFormatItem),           //n:??????
        XX('d', DateTimeFormatItem),          //d:??????
        XX('f', FilenameFormatItem),          //f:?????????
        XX('l', LineFormatItem),              //l:??????
        XX('T', TabFormatItem),               //T:Tab
        XX('F', FiberIdFormatItem),           //F:??????id
        XX('N', ThreadNameFormatItem),        //N:????????????
        XX('%', StringFormatItem),            //%:?????????

#undef XX
        };

        
        //??????????????????????????????
        m_items.clear();
        //???????????????????????????
        std::vector<std::pair<char, std::string>> vec;
        // std::vector<FormatItem::ptr> m_items;
        // ????????????????????????/??????
        // ??????????????????????????????%??????1????????????????????????
        // ???????????????????????????????????????%??????????????????i???????????????

        size_t i = 0;
        while (i < m_pattern.size()) {
            // ????????????while???????????????????????????/?????????+??????
            std::string subs = "";
            char fmt = '%';
            if (m_pattern[i] == '%' && i + 1 < m_pattern.size()) {
                fmt = m_pattern[i + 1];
                if (!(str_2_formatItem.count(fmt))) {
                    // ????????? fmt???????????????????????????str_2_formatItem?????????????????????????????????????????????
                    m_error = true;
                    std::cout << "Errors found after: " << m_pattern.substr(i) << std::endl;
                    break;
                }
                if (m_pattern[i + 1] == '%')
                    subs = "%";
                if (m_pattern[i + 1] == 'd' && i + 2 < m_pattern.size() && m_pattern[i + 2] == '{') {
                    // ?????????????????????????????????
                    size_t n = i + 3;
                    while (n < m_pattern.size() && m_pattern[n] != '}') ++n;
                    subs = m_pattern.substr(i + 3, n - i - 3);
                    i = n + 1;
                } else {
                    i = i + 2;
                }
            } else {
                // ?????????%????????????????????????????????????????????????????????????????????????%
                size_t n = i;
                while (n < m_pattern.size() && m_pattern[n] != '%') ++n;
                subs = m_pattern.substr(i, n - i);
                i = n;
            }
            vec.push_back(std::make_pair(fmt, subs));
        }
        for (auto& p : vec) {
            m_items.push_back(str_2_formatItem[p.first](p.second));
        }
    }
   
    LoggerManager::LoggerManager() {
        // ?????????Manager??????Logger
        m_root.reset(new Logger);
        // ????????????????????????root?????????????????????
        m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
        // ????????????????????????????????????map??????????????????????????????getLogger????????????root?????????
        m_Loggers[m_root->m_name] = m_root;

        init();
    }

    std::string Logger::toYAMLString() {
        Mutextype::Lock lock(m_mutex);
        YAML::Node node;
        node["name"] = m_name;
        if(m_level != LogLevel::UNKNOW) {
            node["level"] = LogLevel::ToString(m_level);
        }
            
        if(m_formatter) {
            node["formatter"] = m_formatter->getPattern();
        }
        for(auto& i : m_appenders) {
            node["appenders"].push_back(YAML::Load(i->toYAMLString()));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    Logger::ptr LoggerManager::getLogger(const std::string& name) {
        Mutextype::Lock lock(m_mutex);
        auto it = m_Loggers.find(name);
        if(it != m_Loggers.end()){
            return it->second;
        }
        
        Logger::ptr logger(new Logger(name));
        logger->m_root = m_root;
        m_Loggers[name] = logger;
        return logger;
    }
    
    struct LogAppenderDefine {
        
        int type = 0;   // 1 file, 2 stdout
        LogLevel::Level level = LogLevel::UNKNOW;
        std::string formatter;
        std::string file;
        bool operator==(const LogAppenderDefine& oth) const {
            // ????????????Appender????????????
            return type == oth.type
                && level == oth.level
                && formatter == oth.formatter
                && file == oth.file;
        }
    };

    struct LogDefine {
        std::string name;
        LogLevel::Level level;
        std::string formatter;
        std::vector<LogAppenderDefine> appenders;
        bool operator==(const LogDefine& oth) const {
            // ????????????Appender????????????
            return name == oth.name
                && level == oth.level
                && formatter == oth.formatter
                && appenders == oth.appenders;
        }
        bool operator<(const LogDefine& oth) const {
            // ????????????set?????????????????????????????????????????????<???????????????????????????????????????
            return name < oth.name;
        }
    };

    /**
     * @brief Lexical?????????????????????, string 2 LogDefine
     */
    template<>
    class LexicalCast<std::string, LogDefine> {
        public:
            LogDefine operator()  (const std::string& v) {
                YAML::Node node = YAML::Load(v);
                LogDefine ld;
                if(!node["name"].IsDefined()) {
                    std::cout << "Log config error: name is null, " << node
                            << std::endl;
                } else {
                    ld.name = node["name"].as<std::string>();
                    ld.level = LogLevel::FromString(node["level"].IsDefined() ? node["level"].as<std::string>() : "");
                    if(node["formatter"].IsDefined()) {
                        ld.formatter = node["formatter"].as<std::string>();
                    }
                    if(node["appenders"].IsDefined()) {
                        for(size_t n = 0; n < node["appenders"].size(); ++n) {
                            auto a = node["appenders"][n];
                            if(!a["type"].IsDefined()) {
                                std::cout << "Log appender config error: type is null, " << a
                                        << std::endl;
                                continue;
                            }
                            std::string type = a["type"].as<std::string>();
                            LogAppenderDefine lad;
                            if(type == "FileLogAppender") {
                                lad.type = 1;
                                if(!a["file"].IsDefined()) {
                                    std::cout << "Log appender config error: filename is null, " << a
                                        << std::endl;
                                 continue;
                                }
                                lad.file = a["file"].as<std::string>();
                            } else if (type == "StdoutLogAppender") {
                                lad.type = 2;
                            } else {
                                std::cout << "Log appender config error: type is invalid, " << a
                                    << std::endl;
                                continue;
                            }
                            if(a["formatter"].IsDefined()) {
                                lad.formatter = a["formatter"].as<std::string> ();
                            }
                            ld.appenders.push_back(lad);
                        }
                    }
                }
                return ld;
            }
    };

    /**
     * @brief Lexical?????????????????????, LogDefine 2 string
     */
    template<>
    class LexicalCast<LogDefine, std::string> {
        public:
            std::string operator()  (const LogDefine& v) {
                YAML::Node node;
                node["name"] = v.name;
                node["level"] = LogLevel::ToString(v.level);
                if(!v.formatter.empty()) {
                    node["formatter"] = v.formatter;
                }
                for(auto& a : v.appenders) {
                    YAML::Node na;
                    if(a.type == 1) {
                        na["type"] = "FileLogAppender";
                        na["file"] = a.file;
                    } else if (a.type == 2) {
                        na["type"] = "StdoutLogAppender";
                    }

                    if(a.level != LogLevel::UNKNOW) // ????????????appender???????????????level
                        na["level"] = LogLevel::ToString(a.level);
                    if(!v.formatter.empty()) {
                        na["formatter"] = a.formatter;
                    }
                    node["appenders"].push_back(na);
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
            }
    };



    // ???set????????????Log??????
    ConfigVar<std::set<LogDefine> >::ptr g_log_defines = 
        sylar::Config::Lookup("logs", std::set<LogDefine>(), "Logs config"); 

    struct LogIniter {
        LogIniter() {
            g_log_defines->addListener([](const std::set<LogDefine>& old_value, 
                const std::set<LogDefine>& new_value){
                // ??????
                __LOG_INFO(__LOG_ROOT) << "on_logger_conf_changed";
                for(auto& i : new_value) {
                    // ??????????????????logger i
                    auto it = old_value.find(i);    // ????????????????????????i
                    Logger::ptr logger;
                    if(it == old_value.end()) {
                        // ??????logger
                        logger = __LOG_NAME(i.name);    // ???????????????i????????????logger
                    } else {
                        if(!(i == *it)) {   // ????????????i?????????????????????????????????
                            // ??????logger
                            logger = __LOG_NAME(i.name);
                        } else {
                            continue;   // ?????????i???????????????????????????????????????
                        }
                    }

                    logger->setLevel(i.level);
                    if(!i.formatter.empty()) { 
                        logger->setFormatter(i.formatter);
                    }
                    
                    logger->clearAppenders();
                    for(auto& a : i.appenders) {
                        LogAppender::ptr ap;
                        if(a.type == 1) {
                            ap.reset(new FileLogAppender(a.file));
                        } else if(a.type == 2) {
                            ap.reset(new StdoutLogAppender);
                        }
                        ap->setLevel(a.level);
                        if(!a.formatter.empty()) { 
                            ap->setFormatter(a.formatter);
                        }
                        logger->addAppender(ap);
                    }
                }
                // ???????????????
                for(auto& i : old_value) {
                    auto it = new_value.find(i);
                    if(it == new_value.end()) {

                        auto logger = __LOG_NAME(i.name);   // ??????????????????
                        logger->setLevel((LogLevel::Level) 0); // ????????????????????????????????????????????????????????????????????????????????????
                        logger->clearAppenders();   // ?????????Appenders;
                    }
                }

            });
        }
    };

    static LogIniter __log_init;

    std::string LoggerManager::toYAMLString() {
        Mutextype::Lock lock(m_mutex);
        YAML::Node node;
        for(auto& i : m_Loggers) {
            node.push_back(YAML::Load(i.second->toYAMLString()));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }

    void LoggerManager::init() {

    }
}
    
