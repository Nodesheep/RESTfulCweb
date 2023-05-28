#include "mysql.h"
#include "logger.h"

using namespace cweb::log;

namespace cweb {
namespace db {

MySQLReply::MySQLReply(MYSQL_RES* res) {
    if(res) {
        data_.reset(res, mysql_free_result);
    }
}

int MySQLReply::Rows() {
    return (int)mysql_num_rows(data_.get());
}

int MySQLReply::Columns() {
    return (int)mysql_num_fields(data_.get());
}

std::string MySQLReply::ColumnName(int idx) {
    MYSQL_FIELD* filed = mysql_fetch_field_direct(data_.get(), idx);
    return filed->name;
}

bool MySQLReply::Next() {
    current_row_ = mysql_fetch_row(data_.get());
    current_row_len_ = mysql_fetch_lengths(data_.get());
    return current_row_;
}

bool MySQLReply::isNull(int idx) {
    return current_row_[idx] == nullptr;
}

int MySQLReply::IntValue(int idx) {
    return std::stoi(current_row_[idx]);
}

uint MySQLReply::UIntValue(int idx) {
    return std::stoi(current_row_[idx]);
}

int64_t MySQLReply::Int64Value(int idx) {
    return std::stoll(current_row_[idx]);
}

uint64_t MySQLReply::UInt64Value(int idx) {
    return std::stoull(current_row_[idx]);
}

float MySQLReply::FloatValue(int idx) {
    return std::stof(current_row_[idx]);
}

double MySQLReply::DoubleValue(int idx) {
    return std::stod(current_row_[idx]);
}

std::string MySQLReply::StringValue(int idx) {
    return std::string(current_row_[idx], current_row_len_[idx]);
}

std::string MySQLReply::BlobValue(int idx) {
    return std::string(current_row_[idx], current_row_len_[idx]);
}

bool MySQL::Connect(const std::string &host, int port, const std::string &user, const std::string &password, const std::string &dbname, int timeout_ms) {
    MYSQL* mysql = mysql_init(nullptr);
    if(!mysql) {
        LOG(LOGLEVEL_ERROR, CWEB_MODULE, "mysql", "connect init error");
        return false;
    }
    
    if(timeout_ms > 0) mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_ms);
    
    if(!mysql_real_connect(mysql, host.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, NULL, 0)) {
        LOG(LOGLEVEL_ERROR, CWEB_MODULE, "mysql", "connect error");
        mysql_close(mysql);
        return false;
    }
    
    mysql_.reset(mysql, mysql_close);
    return true;
}

bool MySQL::Use(const std::string &dbname) {
    return mysql_select_db(mysql_.get(), dbname.c_str()) == 0;
}

/*
 row[0]   row[1]    row[2]
 "1"      "Alice"   "100"
 "2"      "Bob"     "200"
 "3"      "Carol"   "300"
 */
MySQLReplyPtr MySQL::Cmd(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    
    char* buf = nullptr;
    if(vasprintf(&buf, format, ap) < 0) {
        LOG(LOGLEVEL_ERROR, CWEB_MODULE, "mysql", "cmd format error");
        return nullptr;
    }
    MySQLReplyPtr r = cmd(buf);
    va_end(ap);
    free(buf);
    
    return r;
}

MySQLReplyPtr MySQL::Cmd(const std::string &sql) {
    return cmd(sql.c_str());
}

MySQLReplyPtr MySQL::cmd(const char *sql) {
    if(mysql_query(mysql_.get(), sql) != 0) {
        LOG(LOGLEVEL_ERROR, CWEB_MODULE, "mysql", "cmd query error");
        return nullptr;
    }
    
    MYSQL_RES* res = mysql_store_result(mysql_.get());
    if(!res) {
        LOG(LOGLEVEL_ERROR, CWEB_MODULE, "mysql", "cmd result store error");
        return nullptr;
    }
    
    MySQLReplyPtr rt(new MySQLReply(res));
    
    return rt;
}

bool MySQLPool::Init() {
    connections_ = std::make_shared<LockfreeQueue<std::shared_ptr<MySQL>>>(config_.capcity);
    for(int i = 0; i < config_.capcity; ++i) {
        std::shared_ptr<MySQL> sql = std::make_shared<MySQL>();
        if(!sql->Connect(config_.host, config_.port, config_.user, config_.password, config_.dbname)) {
            LOG(LOGLEVEL_ERROR, CWEB_MODULE, "mysql", "mysql pool init error");
            return false;
        }
        ReleaseConnection(sql);
    }
    
    return true;
}

}
}
