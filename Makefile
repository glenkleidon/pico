includepath = include
VPATH = test src/auth src/utils src/http include 
export C_INCLUDE_PATH = ./include
OBJDIR = obj/

all: server

clean:
	@rm -rf ./obj/*.o
	@rm -rf ./bin/*

server: objectdir _server

_server:  main.c httpd.o httpresponse.o picoutils.o authenticate.o base64.o
	gcc  -g -o bin/server $< $(wildcard obj/*.o)

tests: objectdir _tests

_tests: test_cases.c httpresponse.o testmethods.o base64.o authenticate.o picoutils.o
	gcc -g -o ./bin/test_cases $< $(wildcard obj/*.o)

responses: objectdir _responses

_responses: test_responses.c httpresponse.o picoutils.o 
	gcc -g -o ./bin/test_responses $< $(wildcard obj/*.o)

objectdir:
	mkdir -p $(OBJDIR) && mkdir -p bin/

## Compile with debugging.
%.o: %.c 
	gcc -g -c $< -o $(OBJDIR)$@

