FLAGS = -std=c++11 -Wall -Werror -Wextra -pedantic -ldl -g
OBJ = server.o

all: server test

%.o: %.cpp
	g++ $(FLAGS) -c -o $@ $^

server: $(OBJ) libfs_server.o
	g++ $(FLAGS) $^ -o $@
test: test.cpp libfs_client.o
	g++ $(FLAGS) -o $@ $^

clean:
	rm -rf $(OBJ) test server