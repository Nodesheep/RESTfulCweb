#include "router.h"
#include "context.h"
#include "logger.h"

using namespace cweb::log;
namespace cweb {

void Router::AddRouter(const std::string &method, const std::string &pattern, ContextHandler handler) {
    std::vector<std::string> parts;
    Trie::ParsePattern(pattern, parts);
    
    if(roots_.find(method) == roots_.end()) {
        roots_[method] = new Trie();
    }
    
    roots_[method]->Insert(pattern, handler);
}

void Router::Handle(Context *c) {
    if(findRoute(c)) {
        LOG(LOGLEVEL_INFO, CWEB_MODULE, "router", "请求命中路由: %s", c->Path().c_str());
        c->Next();
    }else {
        LOG(LOGLEVEL_WARN, CWEB_MODULE, "router", "请求未命中路由: %s", c->Path().c_str());
        c->STRING(StatusNotFound, "NOT FOUND!");
    }
}

bool Router::findRoute(Context *c) {
    if(roots_.find(c->Method()) == roots_.end()) return false;
   
    std::vector<std::string> parts;
    Trie::ParsePattern(c->Path(), parts);
    
    Node* node = roots_[c->Method()]->Search(parts);
    
    if(node != nullptr) {
        std::vector<std::string> node_parts;
        Trie::ParsePattern(node->pattern_, node_parts);
        for(int i = 0; i < node_parts.size(); ++i) {
            if(node_parts[i][0] == ':') {
                c->params_[node_parts[i].substr(1)] = parts[i];
            }
        }
        c->AddHandler(node->handler_);
        return true;
    }
    
    return false;
}

}
