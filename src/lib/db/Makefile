CC = gcc
CFLAGS = -g -Wall -I/usr/local/include/mysql
LIBFLAGS = 
OBJS = wrappers.o 

all: $(OBJS)
	( cd base; make)
	( cd wrappers; make )

clean:
	( cd wrappers; make clean )
	( cd base; make clean)
	rm -f $(OBJS)
