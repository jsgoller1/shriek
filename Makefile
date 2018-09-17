# Targets for running a Dockerized testing environment
include ./makefiles/docker.mk

### Uncomment this to run Clang's static analyzer while building; this makes the build slower.
ANALYZER:=scan-build

### Uncomment this to do memory leak analysis while running; this makes tests run (slightly) slower
VALGRIND := valgrind -q --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=42

### Uncomment this if you want to produce verbose debugging output during execution
DEBUG:=-DLOG_USE_COLOR

### Compiler settings
CC:=clang
CFLAGS :=-std=gnu11 -g -lm # NOTE: -v shows all paths Clang searches for headers
WARNINGS :=-Weverything -Werror
INCLUDES :=-I common/include
LIBS := common/src/*.c
COMPILE:=$(ANALYZER) $(CC) $(CFLAGS) $(WARNINGS) $(DEBUG) $(INCLUDES) $(LIBS)

### Binary cleanup
setup:
	-mkdir bin

clean:
	-rm -r bin/*

.PHONY: client server tests

client: clean
	reset
	-rm serialized_data.bin
	$(COMPILE) -I $@/include/ $@/src/*.c -o bin/$@
	$(VALGRIND) bin/$@

server: clean
	reset
	$(COMPILE) -I $@/include/ $@/src/*.c -o bin/$@
	$(VALGRIND) ./bin/$@

tests: clean
	reset
	$(COMPILE) -I $@/include/ $@/log_test.c -o bin/log_test
	$(VALGRIND) ./bin/log_test

tcpdump:
	tcpdump -i lo -v -X -w capture.cap

shriek: clean client server
