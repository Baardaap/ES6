CC=gcc
CFLAGS=
DEPS =
OBJ = helloWorld.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
	
.PHONY: clean

clean: 
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
