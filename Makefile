export SHELL=/bin/sh
export SERVERNAME=server
export CLIENTNAME=client
export COMPILER = g++
#export COMPILER = i586-mingw32msvc-g++

.PHONY: clean

all: server client

server:
	$(COMPILER) -o $(SERVERNAME) server.cpp -lpthread

client:
	$(COMPILER) -o $(CLIENTNAME) client.cpp -lpthread

clean:
	rm -f $(SERVERNAME) $(CLIENTNAME)
