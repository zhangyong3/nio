CC=g++ -g
LIB=../thread.o -pthread
INC=-I../

loadtest: loadtest.o
	$(CC) -o $@ $^ $(LIB)

%.o : %.cpp
	$(CC) -c $< $(INC)

clean:
	rm -rf loadtest.o loadtest
