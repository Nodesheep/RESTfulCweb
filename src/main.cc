#include "cweb.h"
#include "context.h"
#include <iostream>
using namespace cweb;

int main() {
    Cweb c("127.0.0.1", 6666);
    c.GET("/api/sayhi", [](Context* c) {
        c->STRING(StatusOK, "hi");
    });
    c.Run(1);
    return 0;
}
