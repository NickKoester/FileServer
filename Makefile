FLAGS = -std=c++11 -Wall -Werror -Wextra -pedantic -ldl -g
OBJ = server.o BlockManager.o Session.o SessionManager.o requests.o Path.o

all: server test

%.o: %.cpp
	g++ $(FLAGS) -c -o $@ $^

server: $(OBJ) libfs_server.o
	g++ $(FLAGS) $^ -o $@
test: test.cpp libfs_client.o
	g++ $(FLAGS) -o $@ $^

clean:
	rm -rf $(OBJ) test server
