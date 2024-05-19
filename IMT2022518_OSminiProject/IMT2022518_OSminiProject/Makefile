# Compiler
CC = gcc

# Directories
BINDIR = bin
DBDIR = db
SRCDIR = src

# Source files
SERVER_SRCS = $(SRCDIR)/server.c $(SRCDIR)/server_functions.c
CLIENT_SRCS = $(SRCDIR)/client.c
CREATE_ADMIN_SRC = $(SRCDIR)/create_admin.c

# Executable names
SERVER_EXE = $(BINDIR)/server
CLIENT_EXE = $(BINDIR)/client
CREATE_ADMIN_EXE = $(BINDIR)/create_admin

# Database files
DB_FILES = $(DBDIR)/books.bin $(DBDIR)/issues.bin $(DBDIR)/users.bin

# Compilation flags
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -lpthread -lrt

.PHONY: all server client create_admin clean

# Default target
all: server client create_admin

# Rule to create directories if they don't exist
$(shell mkdir -p $(BINDIR) $(DBDIR))

# Rule to create database files if they don't exist
$(DB_FILES):
	touch $@

# Rule to compile server
server: $(SERVER_EXE) $(DB_FILES)

$(SERVER_EXE): $(SERVER_SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to compile client
client: $(CLIENT_EXE) $(DB_FILES)

$(CLIENT_EXE): $(CLIENT_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile create_admin
create_admin: $(CREATE_ADMIN_EXE)

$(CREATE_ADMIN_EXE): $(CREATE_ADMIN_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Clean target
clean:
	rm -rf $(BINDIR) $(DBDIR)