CC ?= gcc
CFLAGS ?= -O0 -Wall -std=gnu99

PREFIX = NetEditor
SERVERFILE = $(PREFIX)Server
CLIENTFILE = $(PREFIX)Client

#argv in program
PORT = 1212

VPATH = Server:Client

EXEC = Server/server Client/client

all: $(EXEC)


Server/server: main_server.c $(SERVERFILE).o
	$(CC) $(CFLAGS) -o $@ $^


Server/$(SERVERFILE).o: $(SERVERFILE).c $(SERVERFILE).h
	$(CC) $(CFLAGS) -c -o $@ $< 



Client/client: main_client.c $(CLIENTFILE).o
	$(CC) $(CFLAGS) -o $@ $^


Client/$(CLIENTFILE).o: $(CLIENTFILE).c $(CLIENTFILE).h
	$(CC) $(CFLAGS) -c -o $@ $< 




run_server: Server/server
	cd Server && ./Server $(PORT)

run_client: Client/client
	cd Client && ./client $(PORT)

clean:
	$(RM) Server/*.o Server/server
	$(RM) Client/*.o Server/client


