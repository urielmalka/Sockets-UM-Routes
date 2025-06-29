

# 🧦 Sockum

**Sockum** is a lightweight, modular C++ framework for TCP-based client-server communication. It enables easy construction of networked applications by supporting:

- 🔁 Multiple simultaneous clients
- 🔧 Custom route-based request handling
- 📦 Simple serialization for structured data exchange
- 🧩 Modular architecture for extensibility

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
