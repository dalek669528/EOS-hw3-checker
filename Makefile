CC:=g++
OBJS = sockop
TARGETS = checker
TAGS = -pthread

all: $(TARGETS)	

checker: checker.cpp $(OBJS).c
	$(CC) -o $@ $^

clean:
	rm $(TARGETS)
