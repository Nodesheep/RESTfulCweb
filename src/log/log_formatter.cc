#include "log_formatter.h"
#include "log_info.h"
#include <unordered_map>
#include <functional>
#include <tuple>
#include <sys/time.h>
#include <cassert>

namespace cweb {
namespace log {

class StringFormatItem : public LogFormatItem {
private:
    std::string content_;
public:
    StringFormatItem(const std::string& str = "") : content_(str) {}
    virtual void Format(std::ostream& os, LogInfo* info) override {
        os << content_;
    }
};

class NewLineFormatItem : public LogFormatItem {
public:
    NewLineFormatItem(const std::string& str = "") {}
    virtual void Format(std::ostream& os, LogInfo* info) override {
        os << "\r\n";
    }
};

class DataFormatItem : public LogFormatItem {
public:
    DataFormatItem(const std::string& str = "") {}
    virtual void Format(std::ostream& os, LogInfo* info) override {
        char timeStr[128];
        struct timeval tv;
        gettimeofday(&tv, NULL);
        time_t seconds = tv.tv_sec;
        struct tm* tm = localtime(&seconds);
        strftime(timeStr, 128, "%Y-%m-%d %H:%M:%S", tm);
        os << std::string(timeStr);
    }
};

class LevelFormatItem : public LogFormatItem {
public:
    LevelFormatItem(const std::string& str = "") {}
    virtual void Format(std::ostream& os, LogInfo* info) override {
        switch (info->log_level) {
            case LOGLEVEL_DEBUG :
                os << "DEBUG";
                break;
            case LOGLEVEL_WARN :
                os << "WARN";
                break;
            case LOGLEVEL_INFO:
                os << "INFO";
                break;
            case LOGLEVEL_ERROR:
                os << "ERROR";
                break;
            case LOGLEVEL_FATAL:
                os << "FATAL";
                break;
            default:
                os << "UNKNOW";
                break;
        }
    }
};

class ThreadIDFormatItem : public LogFormatItem {
public:
    ThreadIDFormatItem(const std::string& str = "") {}
    virtual void Format(std::ostream& os, LogInfo* info) override {
        os << info->thread_id;
    }
};

class CoroutineIDFormatItem : public LogFormatItem {
public:
    CoroutineIDFormatItem(const std::string& str = "") {}
    virtual void Format(std::ostream& os, LogInfo* info) override {
        //os << info->coroutine_id;
    }
};

class ModuleFormatItem : public LogFormatItem {
public:
    ModuleFormatItem(const std::string& str = "") {}
    virtual void Format(std::ostream& os, LogInfo* info) override {
        os << info->log_module;
    }
};

class TagFormatItem : public LogFormatItem {
public:
    TagFormatItem(const std::string& str = "") {}
    virtual void Format(std::ostream& os, LogInfo* info) override {
        os << info->log_tag;
    }
};

class ContentFormatItem : public LogFormatItem {
public:
    ContentFormatItem(const std::string& str = "") {}
    virtual void Format(std::ostream& os, LogInfo* info) override {
        os << info->log_content;
    }
};


LogFormatItem* LogFormatItemFactory::GetFormatItem(const std::string& type, const std::string str) {

    static std::unordered_map<std::string, std::function<LogFormatItem*(const std::string& str)> > format_item_factory = {
#define ITEM(type, I) {#type, [](const std::string& str) {return new I(str);}}
        ITEM(s, StringFormatItem),
        ITEM(d, DataFormatItem),
        ITEM(n, NewLineFormatItem),
        ITEM(l, LevelFormatItem),
        ITEM(T, ThreadIDFormatItem),
        ITEM(C, CoroutineIDFormatItem),
        ITEM(m, ModuleFormatItem),
        ITEM(t, TagFormatItem),
        ITEM(c, ContentFormatItem)
#undef ITEM
    };
    
    if(format_item_factory.find(type) != format_item_factory.end()) {
        return format_item_factory[type](str);
    }
    
    return nullptr;
}

LogFormatter::LogFormatter(const std::string& pattern) : log_pattern_(pattern) {
    parseLogpattern();
}

std::string LogFormatter::Format(LogInfo *loginfo) {
    std::stringstream os;
    for(auto item : format_items_) {
        item->Format(os, loginfo);
    }
    return os.str();
}

void LogFormatter::parseLogpattern() {
    
    size_t start = 0;
    std::vector<std::tuple<std::string, std::string> > item_infos;

    for(size_t i = 0 ;i < log_pattern_.size(); ++i) {
        if(log_pattern_[i] != '%') {
            continue;
        }

        if(i + 1 < log_pattern_.size() && log_pattern_[i + 1] == '%') {
            continue;
        }

        item_infos.push_back(std::make_tuple("s", log_pattern_.substr(start, i - start)));

        ++i;

        if(i < log_pattern_.size()) {
            item_infos.push_back(std::make_tuple(log_pattern_.substr(i, 1), ""));
        }

        start = i + 1;
    }

    for(auto& item_info : item_infos) {
        
        LogFormatItem* item = LogFormatItemFactory::GetFormatItem(std::get<0>(item_info), std::get<1>(item_info));
        
        assert(item != nullptr);
        
        format_items_.push_back(item);
    }

}
    
}
}
