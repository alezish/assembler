CC = gcc
CFLAGS = -Wall -ansi -pedantic -g -lm
CFILES = assembler.c data.c firstPass.c macroTable.c memory.c orders.c preAssembler.c secondPass.c stringUtils.c symbols.c

assembler.out: assembler.o data.o firstPass.o macroTable.o memory.o orders.o preAssembler.o secondPass.o stringUtils.o symbols.o
	$(CC) assembler.o data.o firstPass.o macroTable.o memory.o orders.o preAssembler.o secondPass.o stringUtils.o symbols.o -o assembler.out -lm

assembler.o: $(CFILES)
	$(CC) $(CFLAGS) -c assembler.c -o assembler.o

data.o: $(CFILES)
	$(CC) $(CFLAGS) -c data.c -o data.o

firstPass.o: $(CFILES)
	$(CC) $(CFLAGS) -c firstPass.c -o firstPass.o

macroTable.o: $(CFILES)
	$(CC) $(CFLAGS) -c macroTable.c -o macroTable.o

memory.o: $(CFILES)
	$(CC) $(CFLAGS) -c memory.c -o memory.o

orders.o: $(CFILES)
	$(CC) $(CFLAGS) -c orders.c -o orders.o -lm

preAssembler.o: $(CFILES)
	$(CC) $(CFLAGS) -c preAssembler.c -o preAssembler.o

secondPass.o: $(CFILES)
	$(CC) $(CFLAGS) -c secondPass.c -o secondPass.o

stringUtils.o: $(CFILES)
	$(CC) $(CFLAGS) -c stringUtils.c -o stringUtils.o -lm

symbols.o: $(CFILES)
	$(CC) $(CFLAGS) -c symbols.c -o symbols.o