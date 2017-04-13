FLAGS = -std=c++11 -Wall -Werror -Wextra -pedantic -ldl -g
OBJ = server.o BlockManager.o Request.o Session.o SessionManager.o requests.o Path.o serverHelpers.o

all: server test andrew spec testInvalidSession

%.o: %.cpp
	g++ $(FLAGS) -c -o $@ $^

server: $(OBJ) libfs_server.o
	g++ $(FLAGS) $^ -o $@
test: test.cpp libfs_client.o
	g++ $(FLAGS) -o $@ $^
andrew: testReadblock.cpp libfs_client.o
	g++ $(FLAGS) -o $@ $^
spec: specTest.cpp libfs_client.o
	g++ $(FLAGS) -o $@ $^
testInvalidSession: testInvalidSession.cpp libfs_client.o
	g++ $(FLAGS) -o $@ $^

clean:
	rm -rf $(OBJ) test server andrew spec
