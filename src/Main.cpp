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