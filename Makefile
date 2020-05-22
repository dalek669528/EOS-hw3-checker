CC:=g++
OBJS = sockop
TARGETS = checker server command
TAGS = -lpthread

all: $(TARGETS)	

server: server.cpp $(OBJS).cpp
	$(CC) -o $@ $^ $(TAGS)

checker: checker.cpp $(OBJS).cpp
	$(CC) -o $@ $^ $(TAGS)

command: command.cpp
	$(CC) -o $@ $^

clean:
	rm $(TARGETS)
