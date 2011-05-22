CC = gcc
AR = /usr/bin/ar
CFLAGS = -g -Wall 
LIBFLAGS = 
ARCHIVE = ../../lib/libwn.a
OBJS = wn_str.o wn_cfg.o wn_cfgload.o wn_err.o wn_url.o wn_fmt.o wn_net.o wn_mail.o wn_tmpl.o

all: $(OBJS)
	( cd db; make )
	( cd parsers; make )
	$(AR) -r $(ARCHIVE) $(OBJS) db/*.o db/base/*.o db/wrappers/*.o parsers/*.o

clean:
	( cd parsers; make clean )
	( cd db; make clean )
	rm -f $(OBJS) $(ARCHIVE) 
