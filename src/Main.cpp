#include "WebServer.h"

#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")
#pragma comment(lib,"ws2_32.lib")

int main() {
    HTTP_server s;
    s.run();
    system("pause");
    return 0;
}

/*
http://127.0.0.1:3457/net/test.html

http://127.0.0.1:3457/net/noimg.html

http://127.0.0.1:3457/net/test.txt
*/