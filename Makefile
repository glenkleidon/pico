all: server

clean:
	@rm -rf *.o
	@rm -rf server
	@rm -rf test_cases

server: main.o httpd.o httpresponse.o
	gcc -g -o server $^

httpresponse.o : httpresponse.c httpresponse.h
	gcc -g -c -o httpresponse.o httpresponse.c

main.o: main.c httpd.h
	gcc -g -c -o main.o main.c

httpd.o: httpd.c httpd.h
	gcc -g -c -o httpd.o httpd.c

base64.o: base64.c base64.h
	gcc -g -c -o base64.o base64.c

testmethods.o: testmethods.c testmethods.h
	gcc -g -c -o testmethods.o testmethods.c

tests: test_cases.c httpresponse.o testmethods.o base64.o
	gcc -g -o test_cases $^
