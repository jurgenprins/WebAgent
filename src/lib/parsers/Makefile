CC = gcc
LEX = lex
CFLAGS = -g -Wall 
LIBFLAGS = 
OBJS = wn_html.o 

all: lex lib 

lex:
	$(LEX) -own_html.c wn_html.l

lib: $(OBJS)

clean:
	rm -f $(OBJS) wn_html.c 
