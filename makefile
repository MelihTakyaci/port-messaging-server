# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -pthread -Iinclude
LDFLAGS = -pthread -lcrypto -lwebsockets -lssl

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Sources for the server executable
SERVER_SRCS = $(SRCDIR)/server.c $(SRCDIR)/logging.c $(SRCDIR)/config.c $(SRCDIR)/client.c $(SRCDIR)/ssl_utils.c
SERVER_OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SERVER_SRCS))

# Sources for the client executable (eğer ayrı bir main fonksiyonu varsa)
CLIENT_SRCS = $(SRCDIR)/client_app.c $(SRCDIR)/logging.c $(SRCDIR)/config.c
CLIENT_OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(CLIENT_SRCS))

.PHONY: all clean

all: server

# Build server executable
server: $(SERVER_OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/server $(SERVER_OBJS) $(LDFLAGS)
	@echo "Built server executable in $(BINDIR)/server"

# Build client executable
client: $(CLIENT_OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/client $(CLIENT_OBJS) $(LDFLAGS)
	@echo "Built client executable in $(BINDIR)/client"

# Generic rule to build object files for any .c file in SRCDIR
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

tests/client_test: tests/client_test.c
	$(CC) -o tests/client_test tests/client_test.c

test: tests/client_test
	./tests/client_test


# Clean build artifacts
clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(BINDIR)/server
	@echo "Cleaned build artifacts (but kept TLS keys)"

