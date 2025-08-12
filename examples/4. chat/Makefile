# === Compiler and Flags ===
CXX = g++
CXXFLAGS = -std=c++17 -I ../../Sockum/include -I ../../Sockum/include/utils -I ../../Sockum/external/UColor_C/include
LDFLAGS = -L ../../Sockum/build -lSockum -lsodium

# === Source Files ===
OBJ_SERVER = server.cpp
OBJ_CLIENT1 = client.cpp

# === Output Binaries ===
BIN_SERVER = server
BIN_CLIENT = client

# === Default Target ===
all: $(BIN_SERVER) $(BIN_CLIENT)

$(BIN_SERVER): $(OBJ_SERVER)
	$(CXX) $(OBJ_SERVER) -o $@ $(CXXFLAGS) $(LDFLAGS)

$(BIN_CLIENT): $(OBJ_CLIENT)
	$(CXX) $(OBJ_CLIENT) -o $@ $(CXXFLAGS) $(LDFLAGS)

# === Clean Target ===
clean:
	rm -f $(BIN_SERVER) $(BIN_CLIENT) 
