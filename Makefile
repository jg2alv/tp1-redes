CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2

SERVER = bin/server
CLIENT = bin/client

SERVER_SRC = server.c
CLIENT_SRC = client.c

PORT = 5001

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(SERVER_SRC) -o $(SERVER)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $(CLIENT_SRC) -o $(CLIENT)

run_server_v4: $(SERVER)
	./$(SERVER) v4 $(PORT)

run_client_v4: $(CLIENT)
	./$(CLIENT) 127.0.0.1 $(PORT)

run_server_v6: $(SERVER)
	./$(SERVER) v6 $(PORT)

run_client_v6: $(CLIENT)
	./$(CLIENT) ::1 $(PORT)

clean:
	rm -rf bin/*

.PHONY: all run_server run_client run_server_v4 run_server_v6 run_client_v4 run_client_v6 clean
