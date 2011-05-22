CC = gcc
LEX = flex
OPTIM = -g 
CFLAGS = -Wall -I../../include -I/usr/local/include/mysql $(OPTIM)
DBLIBFLAGS =  /usr/local/lib/mysql/libmysqlclient.a -lm
LIBFLAGS = ../../lib/libwn.a
PARSEOBJ = wnparse.o
PARSEPRG = ../../bin/wnparse
DIFFOBJ = wndiff.o
DIFFPRG = ../../bin/wndiff
MAILOBJ = wnmail.o
MAILPRG = ../../bin/wnmail
PRUNEOBJ = wnprune.o
PRUNEPRG = ../../bin/wnprune
CLEANOBJ = wnclean.o
CLEANPRG = ../../bin/wnclean
SHOBJ = wnsh.o
SHPRG = ../../bin/wnsh
TESTOBJ = wntest.o
TESTPRG = ../../bin/wntest

all: wnparse wndiff wnmail wnprune wnclean
	(cd control; make)
	(cd letter; make)

wnparse: $(PARSEOBJ)
	$(CC) $(OPTIM) -o $(PARSEPRG) $(PARSEOBJ) $(ARCHIVE) $(LIBFLAGS) $(DBLIBFLAGS)

wndiff: $(DIFFOBJ)
	$(CC) $(OPTIM) -o $(DIFFPRG) $(DIFFOBJ) $(ARCHIVE) $(LIBFLAGS) $(DBLIBFLAGS)

wnmail: $(MAILOBJ)
	$(CC) $(OPTIM) -o $(MAILPRG) $(MAILOBJ) $(ARCHIVE) $(LIBFLAGS) $(DBLIBFLAGS)

wnprune: $(PRUNEOBJ)
	$(CC) $(OPTIM) -o $(PRUNEPRG) $(PRUNEOBJ) $(ARCHIVE) $(LIBFLAGS) $(DBLIBFLAGS)

wnclean: $(CLEANOBJ)
	$(CC) $(OPTIM) -o $(CLEANPRG) $(CLEANOBJ) $(ARCHIVE) $(LIBFLAGS) $(DBLIBFLAGS)

wnsh: $(SHOBJ)
	$(CC) $(OPTIM) -o $(SHPRG) $(SHOBJ) $(ARCHIVE) $(LIBFLAGS) $(DBLIBFLAGS)

wntest: $(TESTOBJ)
	$(CC) $(OPTIM) -o $(TESTPRG) $(TESTOBJ) $(ARCHIVE) $(LIBFLAGS) $(DBLIBFLAGS)

clean:
	rm -f $(PARSEOBJ) $(PARSEPRG) $(DIFFOBJ) $(DIFFPRG) $(MAILOBJ) $(MAILPRG) $(PRUNEOBJ) $(PRUNEPRG) $(CLEANOBJ) $(CLEANPRG)
	(cd control; make clean)
	(cd letter; make clean)
