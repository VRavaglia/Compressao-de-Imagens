CC = cc
CCOPTS = -g


#CFLAGS = $(CCOPTS)
CFLAGS = $(CCOPTS)

#OBJTS = bit_output.o arithmetic_encode.o fixed_model.o
OBJTSE = bit_output.o arithmetic_encode.o adaptive_model.o
OBJTSD = bit_output.o arithmetic_decode.o adaptive_model.o

# LIBS = 

# ===========<<<  Main >>>=============



.c.o:	; $(CC) -c $(CFLAGS) -o $@ $*.c


all: acode adecode

acode : encode.o $(OBJTS)
	$(CC) $(CFLAGS) encode.o $(OBJTSE) -o acode $(LIBS)

adecode : decode.o $(OBJTS)
	$(CC) $(CFLAGS) decode.o $(OBJTSD) -o adecode $(LIBS)

clean:
	rm ./*.o

