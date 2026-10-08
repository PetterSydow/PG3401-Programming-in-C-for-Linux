# Makefile - Exam PG3401 TASK 3

# Her henviser den til "include"-mappen
INCLDIR = ./include
CC = gcc
CFLAGS = -O2
CFLAGS += -I$(INCLDIR) -std=c89 -Wall

# Her henviser den til "obj"-mappen
OBJDIR = obj

# Her henviser den til header-filer som finnes i include-mappen
_DEPS = task3_examlist.h task3_util.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

# Her legger du til filene som skal kompileres (avslutt med .o og ikke .c - filnavn.o)
_OBJS = program3.o task3_examlist.o task3_util.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Her må du bytte ut "hello" med filnavnet til kompilerte filen gcc lager 
# som du deretter kan kjøre etterpå med ./programnavn (per nå er det ./hello)
program3: $(OBJS)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~
