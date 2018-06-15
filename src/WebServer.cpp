#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include"WebServer.h" 

packet::packet(char* _data, bool* _is_active, SOCKET _socket, int _id):
    data(_data),
    is_active(_is_active),
    client_socket(_socket),
    id(_id) {}
// C'tor
HTTP_server::HTTP_server() {
    WORD wVersionRequested;
	WSADATA wsaData;
	int ret;
    SO_REUSEADDR_ON = 1;
	// WinSock initialize  
	wVersionRequested = MAKEWORD(2, 2);// WinSock DLL ver.
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret != 0)
	{
		printf("WSAStartup() failed!\n");
	}
	// confirm WinSock DLL sorport ver. 2.2：  
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		printf("Invalid Winsock version!\n");
	}
    socket_init();
}

void HTTP_server::socket_init() {
    // server initialize
    memset(&sa_server, 0, sizeof(sa_server));
	sa_server.sin_family = AF_INET;
	sa_server.sin_addr.s_addr = htonl(INADDR_ANY);
	sa_server.sin_port = htons(SERVER_PORT);

    // create socket
    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_socket == INVALID_SOCKET) printf("cannot create socket\n");
	else printf("successfully create socket\n");
	setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR,
		(char*)&SO_REUSEADDR_ON, sizeof(SO_REUSEADDR_ON));//在套接字级别上打开地址复用

	// binding
	int ret = bind(listen_socket, (sockaddr*)&sa_server,
		sizeof(sa_server));
	if (ret == SOCKET_ERROR) printf("bind error\n");
	else printf("successfully bind\n");

	// listening
	ret = listen(listen_socket, QUEUE_SIZE);
	if (ret == SOCKET_ERROR) printf("listen failed\n");
	else printf("successfully listen\n");
}

void send_msg(std::string path, packet msg) {
    std::ifstream in(path, std::ios::binary);
    int sp;
	if (!in) {
		strcpy(msg.data, "HTTP/1.1 404 Not Found\n");
		printf("file no open\n");
	}
    else {
		in.seekg(0, std::ios_base::end);
		sp = in.tellg();
		char length[20];
		sprintf(length, "%d", sp);
		strcpy(msg.data, "HTTP/1.1 200 OK\n");
		strcat(msg.data, "Content-Type: text/html;charset=ISO-8859-1\nContent-Length: ");
		strcat(msg.data, length);
		strcat(msg.data, "\n\n");
		int total_size = 0;
		int ret = send(msg.client_socket, msg.data, strlen(msg.data), 0);
		if (ret == SOCKET_ERROR) {
			printf("send failed\n");
			*msg.is_active = false;
			return;
		}
		else printf("send success\n");
		
		char buffer[100];
		int s = sp + strlen(msg.data) + 1;

		int len = sp;
		total_size = 0;
		in.clear();
		in.seekg(0, std::ios_base::beg);

		while (len > 0) {
			memset(buffer, 0, sizeof(buffer));
			int size = sizeof(buffer) < len ? sizeof(buffer) : len;
			total_size += size;
			len -= size;
			in.read(buffer, size);
			int ret = send(msg.client_socket, buffer, size, 0);

			if (ret == SOCKET_ERROR) {
				printf("send failed\n");
				*msg.is_active = false;
				return;
			}
		}
	}
}

void handle_msg(packet msg) {
    int i = 0, cnt = 0;
    bool flag = false, is_post = false;
    std::string str = "", type = "", data = "";
    std::cout << "This is a thread#" << msg.id << "  ";
    std::cout << "The data of msg: " << std::endl << msg.data;

    if (strcmp(msg.data, "") == 0 || strcmp(msg.data, "\n") == 0) {
        *msg.is_active = false;
		return;
    }
	// parse Head of HTTP
	while(true) {
		if (msg.data[i] == '\n' && msg.data[i + 2] == '\n') break;// end of the head
		if (msg.data[i] == ' ') {
			if (flag) {
				data = str;
				flag = false;
				break;
			}
			else if (str == "GET") {
				type = str;
				flag = true;
			}
			else if (str == "POST") 
				type = str;
			str.clear();
		}
		else if (msg.data[i] == '\n');
		else {
			str +=msg.data[i];
		}
		i++;
	}
	// response to quest
	if (type=="POST") {
	
		bool login_flag = false;
		bool pass_flag = false;
		std::string name = "";
		std::string passwd = "";
		str = "";
		for (int j = i+3; j <= strlen(msg.data); j++) {
			if (msg.data[j] == '&' || msg.data[j] == '=' || j ==strlen(msg.data)) {
				std::cout << str << std::endl;
				if (login_flag) {
					if (str == ACCOUNT) {
						name = str;
						passwd = PW;
					}
					else {
						passwd = "";
					}
					login_flag = false;
				}
				else if (pass_flag) {
					
					if (str == passwd && str != "") {
						std::cout << "str="<<str << " " <<"pw="<< passwd << std::endl;
						char response[200];
						strcpy(response, "<html><body>Nice to meet u,");
						strcat(response, name.c_str());
						strcat(response, "!</body></html>\n");
						int len = strlen(response);
						char length[20];
						sprintf(length, "%d", len);
						strcpy(msg.data, "HTTP/1.1 200 OK\n");
						strcat(msg.data, "Content-Type: text/html;charset=utf-8\nContent-Length: ");
						strcat(msg.data, length);
						strcat(msg.data, "\n\n");
						strcat(msg.data, response);
						printf("%s\n", msg.data);
						int r = send(msg.client_socket, msg.data, 10000, 0);

						if (r == SOCKET_ERROR) {
							printf("send failed\n");
							*msg.is_active = false;
							return;
						}
						printf("send success\n");
						*msg.is_active = false;
						return;
					}
					else {
						std::cout << "str=" << str << " " << "pw=" << passwd << std::endl;
						char response[200];
						strcpy(response, "<html><body>Login failed</body></html>\n");
						int len = strlen(response);
						char length[20];
						sprintf(length, "%d", len);
						strcpy(msg.data, "HTTP/1.1 200 OK\n");
						strcat(msg.data, "Content-Type: text/html;charset=utf-8\nContent-Length: ");
						strcat(msg.data, length);
						strcat(msg.data, "\n\n");
						strcat(msg.data, response);
						printf("%s\n", msg.data);
						int r = send(msg.client_socket, msg.data, 10000, 0);

						if (r == SOCKET_ERROR) {
							printf("send failed\n");
							*msg.is_active = false;
							return;
						}
						printf("send success\n");
						*msg.is_active = false;
						return;
					}
					pass_flag = false;
				}
				else if (str == "login") {
					login_flag = true;
				}
				else if (str == "pass") {
					pass_flag = true;
				}
				if (j == data.size())break;
				str = "";
			}
			else {
				str = str + msg.data[j];
			}
		}
		*msg.is_active = false;
		return;
	}
	else if (type=="GET" && data != "") {

		memset(msg.data, 0, sizeof(msg.data));
		if (data.substr(0, 5) == "/net/") {
			std::string str = "";
			std::string path;

			bool txt_flag = false;
			for (int i = 5; i < data.size(); i++) {
				if (data[i] == '.') {
					flag = true;
				}
				else if (flag)
					str = str + data[i];
			}
		
			if (str == "") {
				*msg.is_active = false;
				return;
			}
			std::cout <<"str="<< str << ","<<std::endl;
			if (str == "txt") 
				path = "catalog/txt/" + data.substr(5);
			else if (str == "html") {
				std::cout << "yes" << std::endl;
				path = "catalog/html/" + data.substr(5);
				std::cout << "path=" << path << std::endl;
			}
			std::cout << "str=" << str << std::endl;
			send_msg(path, msg);
		}
		else if (data.substr(0, 5) == "/img/") {
			int total_size;
			int s;
			std::string path = "catalog/img/" + data.substr(5);
			send_msg(path, msg);
		}

	}
	closesocket(msg.client_socket);
	*msg.is_active = false;
}

void wait_for_exit(bool* is_active, SOCKET listen_socket) {
	std::string str;
	while (1) {
		std::cin >> str;
		if (str == "quit" || str == "exit") {
			while (1) {
				bool flag = true;
				for (int i = 0; i < MAX; i++) {
					if (is_active[i]) {
						flag = false;
						break;
					}
				}
				if (flag) {
					closesocket(listen_socket);
					exit(0);
				}
			}
		} else printf("Exit instruction error!\n");
	}
}

void HTTP_server::run() {
    memset(is_active, false, sizeof(is_active));
    main_thread = new std::thread(wait_for_exit, is_active, listen_socket);

    int len = sizeof(sa_server);
    // waiting for conn.
    while (true) {
        printf("waiting for connection...\n");
        // accept
        client_socket = accept(listen_socket, (sockaddr*)&sa_server, &len);

		if (client_socket < 0) printf("accept failed...\n");
		else {
			printf("successfully connect\n");
			memset(buffer, 0, sizeof(buffer));

			int ret = recv(client_socket, buffer, BUFFER_SIZE, 0);

			if (ret == SOCKET_ERROR) 
				printf("receive failed...\n");
			else if (ret == 0) 
				printf("the client socket is closed...\n");
			else {
				printf("successfully receive\n");
				for (int i = 0; i < MAX; i++) {
					if (!is_active[i]) {
						is_active[i] = true;
						packet msg(buffer, &is_active[i], client_socket, i);
						threads[i] = new std::thread(handle_msg, msg);
						break;
					}
				}
			}
		}
    }
}