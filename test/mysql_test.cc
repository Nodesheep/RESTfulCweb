#include "mysql.h"
#include "logger.h"
#include "json.h"

using namespace cweb::db;
using namespace cweb::log;

/*
int main() {
    
    if(!MySQLPoolSingleton::GetInstance()->Init()) {
        LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "mysql", "init error");
        return 0;
    }
    
    std::shared_ptr<MySQL> sql = MySQLPoolSingleton::GetInstance()->GetConnection();
    
    MySQLReplyPtr r = sql->Cmd("SELECT * FROM person");
    
    int rows = r->Rows();
    
    Json::Value root;
    root["cout"] = rows;
    for(int i = 0; i < rows; ++i) {
        r->Next();
        Json::Value p;
        p["id"] = r->IntValue(0);
        p["name"] = r->StringValue(1);
        p["age"] = r->IntValue(2);
        root["people"].append(p);
    }
    
    std::stringstream body;
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["emitUTF8"] = true;
    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(root, &body);

    LOG(LOGLEVEL_DEBUG, CWEB_MODULE, "mysql", "mysql res: %s", body.str().c_str());

    return 0;
}*/
