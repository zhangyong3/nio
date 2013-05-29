CC=g++ -g
LIB=-lnio -pthread -L../
INC=-I../

all : echoserver loadtest

echoserver: echo.o
	$(CC) -o $@ $^ $(LIB)

loadtest: loadtest.o
	$(CC) -o $@ $^ $(LIB)

%.o : %.cpp
	$(CC) -c $< $(INC)

clean:
	rm -rf *.o loadtest echoserver
