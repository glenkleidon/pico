includepath =./include
VPATH = ./test ./src/auth ./src/utils ./src/http ./include
export C_INCLUDE_PATH = ./include

all: server

clean:
	@rm -rf *.o
	@rm -rf ./bin/*

server: main.o httpd.o httpresponse.o picoutils.o
	gcc  -g -o bin/server $^

tests: test_cases.c httpresponse.o testmethods.o base64.o authenticate.o picoutils.o
	gcc -g -o ./bin/test_cases $^
