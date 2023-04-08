#ifndef CWEB_LOG_LOGFORMATTER_H_
#define CWEB_LOG_LOGFORMATTER_H_

#include <string>
#include <sstream>
#include <vector>

namespace cweb {

namespace log {

struct LogInfo;

class LogFormatItem {
public:
    virtual ~LogFormatItem() {};
    virtual void Format(std::ostream& os, LogInfo* info) = 0;
};

class LogFormatItemFactory {
public:
    static LogFormatItem* GetFormatItem(const std::string& type, const std::string str = "");
};

class LogFormatter {
private:
    std::string log_pattern_;
    std::vector<LogFormatItem* > format_items_;
    
    void parseLogpattern();
public:
    //[%d][%l][%T][%C][%m][%t][%c]%n
    LogFormatter(const std::string& pattern = "[%d][%l][%T][%m][%t][%c]%n");
    
    std::string Format(LogInfo* loginfo);
};

}
}


#endif
