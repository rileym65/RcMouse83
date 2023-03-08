PROJECT=mouse
CC=gcc
DEFS=
LIBS=
LIBDIR=
INCS=
INCDIR=
OBJS=\
	main.o

$(PROJECT): $(OBJS)
	$(CC) $(DEFS) $(LIBPATH) $(OBJS) $(LIBS) -o $(PROJECT)

.c.o:
	$(CC) $(DEFS) $(INCDIR) $(INCS) -c $<

doc:
	makedoc mouse.d
	cat mouse.doc | sed -e 's/&/|/' > x.x
	rm mouse.doc
	mv x.x mouse.doc

clean:
	-rm *.o
	-rm $(PROJECT)

