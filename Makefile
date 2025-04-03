CC = g++
CFLAGS = -I. -I/path_to/U_Color_C	
OBJ_SERVER = test_server.cpp 
OBJ_CLIENT1 = test_client1.cpp
OBJ_CLIENT2 = test_client2.cpp 

all: server client1 client2

server: $(OBJ_SERVER)
	$(CC) -o $@ $^ $(CFLAGS)

client1: $(OBJ_CLIENT1)
	$(CC) -o $@ $^ $(CFLAGS)

client2: $(OBJ_CLIENT2)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f server client1 client2
