export SHELL=/bin/sh
export SERVERNAME=server
export CLIENTNAME=client

.PHONY: clean

all: server client

server:
	g++ -o $(SERVERNAME) server.cpp

client:
	g++ -o $(CLIENTNAME) client.cpp

clean:
	rm -f $(SERVERNAME) $(CLIENTNAME)
