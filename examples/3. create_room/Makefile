# === Compiler and Flags ===
CXX = g++
CXXFLAGS = -std=c++17 -I ../../Sockum/include -I ../../Sockum/include/utils -I ../../Sockum/external/UColor_C/include
LDFLAGS = -L ../../Sockum/build -lSockum -lsodium

# === Source Files ===
OBJ_SERVER = create_room_server.cpp
OBJ_CLIENT1 = create_room_client1.cpp
OBJ_CLIENT2 = create_room_client2.cpp

# === Output Binaries ===
BIN_SERVER = server
BIN_CLIENT1 = client1
BIN_CLIENT2 = client2

# === Default Target ===
all: $(BIN_SERVER) $(BIN_CLIENT1) $(BIN_CLIENT2)

$(BIN_SERVER): $(OBJ_SERVER)
	$(CXX) $(OBJ_SERVER) -o $@ $(CXXFLAGS) $(LDFLAGS)

$(BIN_CLIENT1): $(OBJ_CLIENT1)
	$(CXX) $(OBJ_CLIENT1) -o $@ $(CXXFLAGS) $(LDFLAGS)

$(BIN_CLIENT2): $(OBJ_CLIENT2)
	$(CXX) $(OBJ_CLIENT2) -o $@ $(CXXFLAGS) $(LDFLAGS)

# === Clean Target ===
clean:
	rm -f $(BIN_SERVER) $(BIN_CLIENT1) $(BIN_CLIENT2)
