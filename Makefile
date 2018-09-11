# Targets for running a Dockerized testing environment
include ./makefiles/docker.mk

### Uncomment this to run Clang's static analyzer while building; this makes the build slower.
ANALYZER:=scan-build

### Compiler settings
CC:=clang
CFLAGS :=-std=gnu11 -g -lm
WARNINGS :=-Weverything -Werror
INCLUDES :=-I common/include
LIBS := common/src/*.c
COMPILE:=$(ANALYZER) $(CC) $(CFLAGS) $(WARNINGS) $(INCLUDES) $(LIBS)

### Uncomment this if you want to run the tests; this makes the build slower.
# RUN_TESTS:=true

### Uncomment this to do memory leak analysis while running; this makes tests run (slightly) slower
VALGRIND := valgrind -q --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=42

### Binary cleanup
setup:
	-mkdir bin

clean:
	-rm -r bin/*

.PHONY: client server

client: clean
	-rm serialized_data.bin
	$(COMPILE) -I $@/include/ $@/src/*.c -o bin/$@
	echo "set THIS_IS_A_TEST_MESSAGE IT_IS_PRETTY_GREAT" | $(VALGRIND) bin/$@
	hexdump -C serialized_data.bin

server: clean client
	$(COMPILE) -I $@/include/ $@/src/*.c -o bin/$@
	$(VALGRIND) ./bin/$@
