# 🔐 Multi-Client Encrypted Messaging Server in C

A lightweight, modular, and scalable port-based messaging server built in C.  
This project demonstrates secure multi-threaded client handling, TLS/SSL encryption, SHA256-based logging, rate limiting, and efficient broadcast communication — written with performance and maintainability in mind.

---

## 📁 Project Structure

```
.
├── bin/         # Compiled binaries and certificates
├── certs/       # (Optional) TLS certificate & key storage
├── include/     # Header files
├── obj/         # Object files (compiled .o)
├── src/         # Source code
├── tests/       # Unit tests
└── makefile     # Build automation
```

---

## 🚀 Features

- ✅ Multi-client handling via POSIX threads
- ✅ Full TLS/SSL encryption using OpenSSL
- ✅ SHA256 hashing of messages for audit logs
- ✅ Per-client rate limiting (anti-spam)
- ✅ Broadcast support to all connected clients
- ✅ Modular architecture (`client`, `server`, `logging`, `config`)
- ✅ `make`-based build system
- ✅ Testable design with unit test support

---

## ⚙️ Build & Run

```bash
# Clone and enter the project
git clone https://github.com/your-username/port-messaging-server.git
cd port-messaging-server

# Build the project
make

# Generate self-signed certs (if not already present)
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes

# Move certs to bin/ or use a certs/ folder
mv cert.pem key.pem bin/

# Run the server (ensure cert.pem and key.pem are in the working directory)
./bin/server
```

---

## 🧪 Running Tests

```bash
make test     # Compile and run all tests
./tests/client_test   # Run specific test executable
```

---

## 🧱 Module Overview

| File                  | Responsibility                            |
|-----------------------|--------------------------------------------|
| `server.c / server.h` | Core server logic, thread mgmt, SSL_CTX    |
| `client.c / client.h` | Client handler, SSL communication          |
| `ssl_utils.c / .h`    | TLS context creation, cert config          |
| `logging.c / .h`      | Timestamped SHA256 log system              |
| `config.c / .h`       | CLI and config parsing                     |

---

## 📜 Sample Log Output

```
[2025-04-03 15:31:00] Client connected from 127.0.0.1:53994  
[2025-04-03 15:31:05] Received from 127.0.0.1:53994: Hello  
[2025-04-03 15:31:05] Hash of received message: 66a045...  
[2025-04-03 15:31:13] Server is shutting down gracefully  
```

---

## 🔐 Security Notes

- All communication is encrypted using **TLS v1.3** via OpenSSL.
- Messages are **not visible on the network**, even when sniffed via `tcpdump` or Wireshark.
- Server logs decrypted messages only after they pass TLS decryption.

---

## 📌 Future Improvements

- 📦 Persistent client message queues (for offline users)
- 🛡 End-to-end AES/RSA encryption (on top of TLS)
- 🌐 Optional web dashboard for monitoring and logs
- 🔄 Message history storage via SQLite or PostgreSQL

Melih Takyaci
Email: melihtakyacib@gmail.com
GitHub: @MelihTakyaci

## 👨‍💻 Author

**Melih Takyaci**  
📧 Email: [melihtakyaci@gmail.com](mailto:melihtakyaci@gmail.com)  
💻 GitHub: [@MelihTakyaci](https://github.com/MelihTakyaci)

---

## ⭐ Star the Repo

If you found this project useful, consider giving it a ⭐ on GitHub!  
It motivates future updates and helps others discover the project.
