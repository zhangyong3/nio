OBJ=thread.o tcpserver.o bytebuffer.o session.o 
CC=g++ -g
LIB=-lpthread

libnio.a : $(OBJ)
	ar -rus $@ $^

%.o: %.cpp
	$(CC) -c $<

clean:
	rm -rf $(OBJ) libnio.a
