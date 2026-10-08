# Makefile - Exam PG3401 TASK 4 part II

# Her henviser den til "include"-mappen
INCLDIR = ./include
CC = gcc
CFLAGS = -O2
CFLAGS += -I$(INCLDIR) -std=c89 -Wall -pthread -g

# Her henviser den til "obj"-mappen
OBJDIR = obj

# Her henviser den til header-filer som finnes i include-mappen
_DEPS = task4_prime.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

# Her legger du til filene som skal kompileres (avslutt med .o og ikke .c - filnavn.o)
_OBJS = task4_threads.o task4_prime.o xtea.o ipc.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

program4b: $(OBJS)
	gcc -o $@ $^ $(CFLAGS)

# Ipc.c uses c99 in a for loop, trying a method to only compile this file
# with C99 standard with the rest compiling in C89 std.
$(OBJDIR)/ipc.o: ipc.c
	$(CC) -c -o $@ $< -O2 -std=c99 -Wall -I$(INCLDIR)-I

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~
