#ifndef _WebServer_H_
#define _WebServer_H_

#include<cstdio>  
#include<cstdlib>  
#include<fstream>  
#include<cstring>  
#include<string>
#include<iostream>
#include <winsock.h>
#include <windows.h>
#include <thread>

#define SERVER_PORT 3457
#define BUFFER_SIZE 13000
#define QUEUE_SIZE  5
#define MAX         100
#define ACCOUNT     "test"
#define PW          "QAQ"

struct packet {
    char* data;
    bool* is_active;
    SOCKET client_socket;
    int id;
    packet(char* _data, bool* _is_active, SOCKET _socket, int _id);
};

class HTTP_server {
public:
    HTTP_server();
    void run();
private:
    int SO_REUSEADDR_ON;
    char buffer[BUFFER_SIZE];
    sockaddr_in sa_server;
    std::thread* threads[MAX];
    std::thread* main_thread;
    bool is_active[MAX];
    SOCKET listen_socket;
    SOCKET client_socket;
    friend void handle_msg(packet msg);
    friend void wait_for_exit(bool* is_active, SOCKET listen_socket);
    friend void send_msg(std::string path, packet msg);
    void socket_init();
};

#endif