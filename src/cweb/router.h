#ifndef CWEB_CWEB_ROUTER_H_
#define CWEB_CWEB_ROUTER_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace cweb {

class Context;
typedef std::function<void(std::shared_ptr<Context>)> ContextHandler;
class Node {
private:
    std::string pattern_;
    std::string part_;
    std::vector<Node *> children_;
    bool iswild_;
    ContextHandler handler_;
    
public:
    friend class Trie;
    friend class Router;
    Node(std::string part = "/", bool iswild = false): part_(part), iswild_(iswild) {}
    
    virtual ~Node() {
        for(Node *child : children_) {
            delete child;
        }
    }
    
    void InsertChild(Node* child) { children_.push_back(child); }
    
    Node* FindChild(const std::string& part) {
        if(children_.size() > 0) {
            for(auto child : children_) {
                if(child->part_ == part || child->iswild_) {
                    return child;
                }
            }
        }
        return nullptr;
    }
    
    std::vector<Node *> FindChildren(const std::string& part) {
        std::vector<Node *> nodes(0);
        for(auto child : children_) {
            if(child->part_ == part || child->iswild_) {
                nodes.push_back(child);
            }
        }
        return nodes;
    }
};

/*
 前缀树：
                        [/]
                     /        \
              [api]              [group]
            /   |    \              |
      [sayhi] [echo] [dynamic]   [sayhi]
                         |
                      [:param]
 */

class Trie {
private:
    Node* root_;
    
public:
    Trie() { root_ = new Node(); }
    ~Trie() { delete root_; }
    
    //注册路由
    void Insert(const std::string& pattern, ContextHandler handler) {
        std::vector<std::string> parts;
        ParsePattern(pattern, parts);
        insert(parts, root_, 0, pattern, handler);
    }
    
    //匹配路由
    Node *Search(const std::string& pattern) {
        std::vector<std::string> parts;
        ParsePattern(pattern, parts);
        return search(parts, root_, 0);
    }
    
    Node *Search(std::vector<std::string>& parts) {
        return search(parts, root_, 0);
    }
    
private:
    void insert(std::vector<std::string>& parts, Node* node, int high, const std::string& pattern, ContextHandler handler) {
        if(parts.size() == high) {
            node->pattern_ = pattern;
            node->handler_ = std::move(handler);
            return;
        }
   
        std::string part = parts[high];
        Node *child = node->FindChild(part);
        
        if(child == nullptr) {
            child = new Node(part, part[0] == ':');
            node->InsertChild(child);
        }
        
        insert(parts, child, high + 1, pattern, handler);
    }
    
    Node *search(std::vector<std::string> &parts, Node *node, int high) {
        if(parts.size() == high) {
            if(node->pattern_ == "") {
                return nullptr;
            }
            return node;
        }
        
        std::string part = parts[high];
        std::vector<Node *> children = node->FindChildren(part);
        
        for(auto child : children) {
            Node *res = search(parts, child, high + 1);
            if(res != nullptr) {
                return res;
            }
        }
        
        return nullptr;
    }
    
public:
    static void Split(const std::string& s, char delim, std::vector<std::string>& parts) {
        int begin = 0, end = 0;
        if(s[0] == delim) {
            begin = 1;
            end = 1;
        }
        
        for(; end < s.size(); ++end) {
            if(s[end] == delim) {
                parts.push_back(s.substr(begin, end - begin));
                begin = end + 1;
                end = end + 1;
            }
        }
        
        parts.push_back(s.substr(begin, s.size() - begin + 1));
    }
    
    static void ParsePattern(const std::string& pattern , std::vector<std::string>& parts) {
        Split(pattern, '/', parts);
    }
};

class Router {
private:
    std::unordered_map<std::string, Trie*> roots_;
    bool findRoute(Context *c);

public:
    virtual ~Router() {
        for(auto it = roots_.begin(); it != roots_.end(); ++it) {
            delete it->second;
        }
    }
    
    void AddRouter(const std::string& method, const std::string& pattern, ContextHandler handler);
    
    void Handle(std::shared_ptr<Context> c);
};


}

#endif
