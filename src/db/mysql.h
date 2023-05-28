#ifndef CWEB_DATABASE_MYSQL_H_
#define CWEB_DATABASE_MYSQL_H_

#include <mysql/mysql.h>
#include <string>
#include <memory>

#include "dbconnection_pool.h"
#include "cweb_config.h"
#include "singleton.h"

namespace cweb {
namespace db {

class MySQLReply {
private:
    std::shared_ptr<MYSQL_RES> data_;
    MYSQL_ROW current_row_;
    unsigned long* current_row_len_;
    
public:
    MySQLReply(MYSQL_RES* res);
    
    int Rows();
    int Columns();
    std::string ColumnName(int idx);
    bool Next();
    
    bool isNull(int idx);
    int IntValue(int idx);
    uint UIntValue(int idx);
    int64_t Int64Value(int idx);
    uint64_t UInt64Value(int idx);
    float FloatValue(int idx);
    double DoubleValue(int idx);
    std::string StringValue(int idx);
    std::string BlobValue(int idx);
};

typedef std::shared_ptr<MySQLReply> MySQLReplyPtr;

class MySQL {
private:
    std::shared_ptr<MYSQL> mysql_;
    MySQLReplyPtr cmd(const char* sql);
    
public:
    bool Connect(const std::string& host, int port, const std::string& user, const std::string& password, const std::string& dbname, int timeout_ms = 0);
    bool Use(const std::string& dbname);
    
    MySQLReplyPtr Cmd(const char* format, ...);
    MySQLReplyPtr Cmd(const std::string& sql);
};

class MySQLPool : public DBConnectionPool<MySQL> {
private:
    MysqlConfig config_;
    
public:
    MySQLPool() : DBConnectionPool<MySQL>() {}
    bool Init();
};

typedef cweb::util::Singleton<MySQLPool> MySQLPoolSingleton;

}
}

#endif
