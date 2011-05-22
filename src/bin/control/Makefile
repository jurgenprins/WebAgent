CC = gcc
LEX = flex
OPTIM = -O2
CFLAGS = -Wall -I../../../include -I/usr/local/include/mysql $(OPTIM)
DBLIBFLAGS = /usr/local/lib/mysql/libmysqlclient.a -lm 
LIBFLAGS = ../../../lib/libwn.a 
OBJ = wnc_doc.o wnc_main.o
PRG = ../../../bin/wncontrol

all: wncontrol

wncontrol: $(OBJ)
	$(CC) $(OPTIM) -o $(PRG) $(OBJ) $(LIBFLAGS) $(DBLIBFLAGS)

clean:
	rm -f $(OBJ) $(PRG)
