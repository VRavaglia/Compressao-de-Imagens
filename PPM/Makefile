CC = gcc
CCOPTS = -g


#CFLAGS = $(CCOPTS)
CFLAGS = $(CCOPTS)
DEPSE = arithmetic_coding.h bit_output.h arithmetic_encode.h ppm.h

#OBJTS = bit_output.o arithmetic_encode.o fixed_model.o
OBJTSE = bit_output.o arithmetic_encode.o ppm.o
OBJTSD = bit_input.o arithmetic_decode.o ppm.o
#add_executable(encode encode.c bit_output.c arithmetic_encode.c ppm.c)
#add_executable(decode decode.c bit_input.c arithmetic_decode.c ppm.c)

# LIBS = 

# ===========<<<  Main >>>=============



%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


all: acode adecode

acode : encode.o $(OBJTSE)
	$(CC) -o encode.out $(CFLAGS) encode.o $(OBJTSE) $(LIBS) -O3

adecode : decode.o $(OBJTSD)
	$(CC) -o decode.out $(CFLAGS) decode.o $(OBJTSD) $(LIBS) -O3

clean:
	rm ./*.o

