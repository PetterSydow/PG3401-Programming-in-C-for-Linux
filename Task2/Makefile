# Makefile - Exam PG3401 TASK 2

# Her henviser den til "include"-mappen
INCLDIR = ./include
CC = gcc
CFLAGS = -O2
CFLAGS += -I$(INCLDIR) -std=c89 -Wall

# Her henviser den til "obj"-mappen
OBJDIR = obj

# Her henviser den til header-filer som finnes i include-mappen
_DEPS = task2_abun.h task2_cube.h task2_def.h task2_fib.h task2_kvad.h task2_odd.h task2_perf.h task2_prim.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

# Her legger du til filene som skal kompileres (avslutt med .o og ikke .c - filnavn.o)
_OBJS = main.o task2_abun.o task2_cube.o task2_def.o task2_fib.o task2_kvad.o task2_odd.o task2_perf.o task2_prim.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Her må du bytte ut "hello" med filnavnet til kompilerte filen gcc lager 
# som du deretter kan kjøre etterpå med ./programnavn (per nå er det ./hello)
program2: $(OBJS)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~
