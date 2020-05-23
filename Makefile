CC:=g++
OBJS = sockop
TARGETS = checker server
TAGS = -lpthread

all: $(TARGETS)	

# server: server.cpp $(OBJS).c
# 	$(CC) -o $@ $^ $(TAGS)

checker: checker.cpp $(OBJS).c
	$(CC) -o $@ $^ $(TAGS)

clean:
	rm $(TARGETS) result.txt
