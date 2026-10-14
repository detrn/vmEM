CC = gcc

CFLAGS = -std=c11 -Iinclude -Wall -Wextra -Wpedantic -Wconversion \
         -Wshadow -g3 -O1 \
         -fsanitize=address,undefined \
         -fno-omit-frame-pointer

LDFLAGS = -fsanitize=address,undefined

TARGET = minivm16

OBJS = src/vm/main.o \
       src/vm/cpu.o \
       src/vm/vm.o \
       src/vm/debugger.o \
       src/assembler/lexer.o \
       src/assembler/parser.o \
       src/assembler/assembler.o

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

src/vm/main.o: src/vm/main.c \
               include/vm/vm.h \
               include/vm/cpu.h \
               include/vm/debug.h \
               include/assembler/lexer.h \
               include/assembler/token.h
	$(CC) $(CFLAGS) -c src/vm/main.c -o src/vm/main.o

src/vm/cpu.o: src/vm/cpu.c \
              include/vm/cpu.h \
              include/vm/vm.h \
              include/vm/instructions.h
	$(CC) $(CFLAGS) -c src/vm/cpu.c -o src/vm/cpu.o

src/vm/vm.o: src/vm/vm.c \
             include/vm/vm.h
	$(CC) $(CFLAGS) -c src/vm/vm.c -o src/vm/vm.o

src/vm/debugger.o: src/vm/debugger.c \
                   include/vm/debug.h \
                   include/vm/vm.h
	$(CC) $(CFLAGS) -c src/vm/debugger.c -o src/vm/debugger.o

src/assembler/lexer.o: src/assembler/lexer.c \
                       include/assembler/lexer.h \
                       include/assembler/token.h
	$(CC) $(CFLAGS) -c src/assembler/lexer.c -o src/assembler/lexer.o

src/assembler/parser.o: src/assembler/parser.c \
			include/assembler/lexer.h \
			include/assembler/parser.h \
			include/assembler/token.h \
			include/assembler/statement.h
	$(CC) $(CFLAGS) -c src/assembler/parser.c -o src/assembler/parser.o 

src/assembler/assembler.o: src/assembler/assembler.c \
	include/assembler/statement.h \
	include/assembler/parser.h \
	include/assembler/lexer.h 
	$(CC) $(CFLAGS) -c src/assembler/assembler.c -o src/assembler/assembler.o 

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
