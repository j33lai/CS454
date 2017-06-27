CXX = g++
LIBDIR = -L../rpc
LIB = -lrpc
SERVEROBJECTS = server_functions.o server_function_skels.o server.o
CLIENTOBJECTS = client1.o
DEPENDS = ${OBJECTS:.o=.d}
SERVER = server
CLIENT = client

all: $(SERVER) $(CLIENT)
${SERVER} : ${SERVEROBJECTS}
	$(CXX) $(LIBDIR) $(SERVEROBJECTS) $(LIB) -o $(SERVER)
${CLIENT} : ${CLIENTOBJECTS}
	$(CXX) $(LIBDIR) $(CLIENTOBJECTS) $(LIB) -o $(CLIENT)


-include ${DEPENDS}

.PHONY: clean
clean:
	rm  -f $(SERVEROBJECTS) $(CLIENTOBJECTS) $(DEPENDS) $(SERVER) $(CLIENT)
