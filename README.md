# 🔌 Multi-Client Port Messaging Server in C

A lightweight, modular, and scalable port-based messaging server built in C. This project demonstrates multi-threaded client handling, SHA256-based logging, rate limiting, and efficient broadcast communication—all written with a focus on performance and maintainability.

---

## 📁 Project Structure

. ├── bin/ # Compiled binaries and logs ├── include/ # Header files ├── obj/ # Object files (compiled .o) ├── src/ # Source code files ├── tests/ # Unit tests and test executables └── makefile # Build automation


---

## 🚀 Features

- ✅ Multi-client connection handling using threads
- ✅ Secure SHA256-based logging system
- ✅ Rate limiting per client IP
- ✅ Broadcast support to all connected clients
- ✅ Clean and modular architecture (`client`, `server`, `config`, `logging`)
- ✅ Makefile-based build and test automation
- ✅ Cross-platform compatibility (Linux/Unix)

---

## ⚙️ Build Instructions

```bash
# Clone the repository
git clone https://github.com/your-username/port-messaging-server.git
cd port-messaging-server

# Build the project
make

# Run the server
./bin/server

🧪 Running Tests

# Compile and run tests
make test

# Or run test executable directly
./tests/client_test

🧱 Module Overview

    server.c / server.h: Core server logic, thread handling, client management

    client.c / client.h: Client-side logic and communication

    logging.c / logging.h: Secure SHA256 logging with timestamps

    config.c / config.h: Configuration parsing and default setups

📜 Sample Log Output

[2025-04-02 18:02:35] Server is starting up
[2025-04-02 18:02:35] Server listening on port 8080 with backlog size 128
[2025-04-02 18:03:16] Client connected from 127.0.0.1:47992
[2025-04-02 18:03:16] Received from 127.0.0.1:47992: Hello from test client!

📌 Future Improvements

TLS/SSL encryption support

Persistent client message queues


👨‍💻 Author

Melih Takyaci
Email: you@example.com
GitHub: @MelihTakyaci

⭐ Star the Repo

If you like this project, consider giving it a ⭐ on GitHub. It helps a lot!


---