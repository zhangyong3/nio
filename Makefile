OBJ=thread.o tcpserver.o bytebuffer.o session.o 
CC=g++ -g
LIB=-lpthread

echo :  echo.o $(OBJ)
	$(CC) -o $@ $^ $(LIB)

loadtest: thread.o loadtest.o
	$(CC) -o $@ $^ $(LIB)

%.o: %.cpp
	$(CC) -c $<

clean:
	rm -rf $(OBJ) echo echo.o
