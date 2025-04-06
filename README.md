

# 🧦 Sockum

`sockum` is a C++ client-server communication framework using TCP sockets. It supports multiple simultaneous clients, custom route handling, and a modular design for extending server/client functionality. The framework includes basic serialization for structured data exchange and route-based request handling.

---

## 📁 Project Structure

- `./Sockum` – Server & Client implementation.

---

---

## 🎨 Download the Color Library

Before compiling the project, make sure to clone the color library into your project directory:

```bash
git clone https://github.com/urielmalka/UColor_C.git
```

---

## 🛠️ Compilation

To compile the project, run:

```bash
make
```

This will generate the `server` and `client1` and `client2` executables.

---

## 🚀 Running the App

1. In the first terminal, start the server:

```bash
./server
```

2. In a second terminal, run the first client:

```bash
./client1
```

3. In a third terminal, run the second client:

```bash
./client2
```

---

## 🧹 Clean Up

To remove the compiled files:

```bash
make clean
```

---
