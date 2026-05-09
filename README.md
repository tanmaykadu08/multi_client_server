# Multi-Client Server — C++ TCP Socket Programming

A Windows-based **TCP client-server application** written in C++ using the **Winsock2 API**. This project demonstrates low-level socket programming concepts including socket creation, binding, listening, accepting connections, and bidirectional message exchange over TCP.

---

## 📁 Project Structure

```
multi_client_server/
├── project 1/               # Single-client interactive chat server
│   ├── client/
│   │   ├── client.cpp       # TCP client source
│   │   └── client.exe       # Prebuilt client executable
│   └── server/
│       ├── server.cpp       # TCP server source
│       └── server.exe       # Prebuilt server executable
└── project 2/               # (In progress / extended version)
    └── multi_client_server/
```

---

## 🚀 Features

- **TCP Socket Communication** over IPv4 using Winsock2
- **Bidirectional messaging** — both client and server can send and receive messages interactively
- **Graceful shutdown** — typing `exit` on either end terminates the connection cleanly
- **Error handling** — socket errors are reported via `WSAGetLastError()` at every critical step
- Runs on **Windows** (uses `winsock2.h` and `ws2_32.lib`)

---

## ⚙️ How It Works

### Server (`server.cpp`)

1. Initializes Winsock with `WSAStartup`
2. Creates a TCP socket (`SOCK_STREAM`)
3. Binds to `0.0.0.0:8081` (all available network interfaces)
4. Listens for incoming connections (backlog of 5)
5. Accepts a single client connection
6. Enters a **receive → respond** loop:
   - Reads a message from the client
   - Prompts the server operator to type a response
   - Sends the response back
   - Typing `exit` breaks the loop
7. Closes all sockets and calls `WSACleanup`

### Client (`client.cpp`)

1. Initializes Winsock with `WSAStartup`
2. Creates a TCP socket
3. Connects to the server at `127.0.0.1:8081`
4. Enters a **send → receive** loop:
   - Prompts the user for a message to send
   - Sends the message to the server
   - Waits for and prints the server's response
   - Typing `exit` closes the connection
5. Closes the socket cleanly

---

## 🛠️ Building from Source

### Prerequisites

- Windows OS
- A C++ compiler with Winsock support:
  - **MSVC** (Visual Studio) — recommended
  - **MinGW / g++** (with `-lws2_32` flag)

### Compile with MSVC (Developer Command Prompt)

```bash
# Compile the server
cl /EHsc server.cpp /link ws2_32.lib /out:server.exe

# Compile the client
cl /EHsc client.cpp /link ws2_32.lib /out:client.exe
```

### Compile with g++ (MinGW)

```bash
# Compile the server
g++ server.cpp -o server.exe -lws2_32

# Compile the client
g++ client.cpp -o client.exe -lws2_32
```

---

## ▶️ Running the Application

> **Important:** Start the server **before** the client.

1. Open a terminal and run the server:
   ```bash
   ./server.exe
   # Output: Server is listening on port 8081...
   ```

2. Open a second terminal and run the client:
   ```bash
   ./client.exe
   # Output: Connected to server!
   ```

3. **On the client terminal** — type a message and press Enter:
   ```
   Enter a message to send to the server (type 'exit' to quit): Hello!
   ```

4. **On the server terminal** — the message appears, and you can respond:
   ```
   Received data: Hello!
   you can type a response to the client:
   Hi there!
   ```

5. **On the client terminal** — the server's response is displayed:
   ```
   Message from server: Hi there!
   ```

6. Type `exit` on either end to terminate the session.

---

## 🔌 Configuration

| Parameter    | Value       | Location        |
|--------------|-------------|-----------------|
| Server IP    | `127.0.0.1` | `client.cpp:21` |
| Port         | `8081`      | Both files      |
| Buffer Size  | `1024` bytes | Both files     |
| Listen Backlog | `5`       | `server.cpp:34` |

To change the port or IP, edit the corresponding values in `server.cpp` and `client.cpp` before recompiling.

---

## 📚 Key Concepts Demonstrated

| Concept | API Used |
|---|---|
| Socket initialization | `WSAStartup`, `WSACleanup` |
| Socket creation | `socket(AF_INET, SOCK_STREAM, 0)` |
| Address binding | `bind()` with `sockaddr_in` |
| Listening for connections | `listen()` |
| Accepting a connection | `accept()` |
| Connecting to a server | `connect()` |
| Sending data | `send()` |
| Receiving data | `recv()` |
| Error reporting | `WSAGetLastError()` |
| Safe struct initialization | `memset()` |

---

## 📌 Notes & Limitations

- **Project 1** currently supports only **one client at a time** — the server accepts a single connection and blocks until that session ends.
- **Project 2** is intended to extend this to support **multiple simultaneous clients** (e.g., using threads or `select()`).
- The client calls `recv()` twice in the receive block — the first call reads the data; the second is redundant and may block. This is a known issue to be fixed.
- This project is designed for **learning purposes** and is not production-ready.

---

## 🧠 References

- [Winsock2 Documentation — Microsoft Docs](https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
