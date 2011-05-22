CC = gcc
LEX = flex
OPTIM = -g
CFLAGS = -Wall -I../../../include -I/usr/local/include/mysql $(OPTIM)
DBLIBFLAGS = /usr/local/lib/mysql/libmysqlclient.a -lm 
LIBFLAGS = ../../../lib/libwn.a 
OBJ = wnl_letter.o wnl_main.o
PRG = ../../../bin/wnletter

all: wnletter

wnletter: $(OBJ)
	$(CC) $(OPTIM) -o $(PRG) $(OBJ) $(LIBFLAGS) $(DBLIBFLAGS)

clean:
	rm -f $(OBJ) $(PRG)
