CC = g++
CFLAG = -std=c++11
OBJ = WebServer

$(OBJ) : src/Main.cpp src/WebServer.cpp src/WebServer.h
	$(CC) $(CFLAG) src/main.cpp src/WebServer.cpp -lwsock32 -o $(OBJ)

.PHONE : clean
clean : 
	rm *.exe