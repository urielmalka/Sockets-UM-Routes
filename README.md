
# 🧦 Sockum

**Sockum** is a lightweight, modular C++ framework for building efficient TCP-based client-server applications.  
It provides a clean and extensible foundation for creating scalable networked systems with minimal overhead.

## ✨ Features

- 🔁 **Multi-Client Support**  
  Handle multiple simultaneous TCP client connections with built-in thread-safe management.

- 🔧 **Route-Based Request Handling**  
  Easily define routes (like REST endpoints) and bind them to specific handlers for organized and modular logic.

- 📦 **Simple Serialization**  
  Built-in helpers for serializing and deserializing structured data for smooth communication between clients and server.

- 🧩 **Modular & Extensible Architecture**  
  Designed to be easily extended — plug in custom features without modifying the core framework.

- 📁 **File Transfer Support**  
  Send and receive files over the network with simple APIs for file streaming and chunk handling.

- 🏠 **Room-Based Communication**  
  Create and manage logical "rooms" for group communication — ideal for chat applications, multiplayer games, and collaborative environments.

---

## 📁 Project Structure

- `./Sockum` – Server & Client implementation.

---


## 🛠️ Compilation

To compile the project:

```bash
mkdir build
cd build
cmake ..
make
```

This will generate all necessary executables.

---

## 🚀 Running the Example Applications

1. Return to the main project directory, enter the examples folder, and select the desired example:

```bash
make
```

2. In the first terminal, start the server:

```bash
./server
```

3. In a second terminal, run the first client:

```bash
./client
```

💡 Note: Some examples support multiple clients.

Each client will connect to the server, send messages, and receive route-based responses.

---

## 🧹 Clean Up

To remove the compiled files:

```bash
make clean
```

---
