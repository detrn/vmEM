CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

TARGET = minivm16
OBJS = src/main.o src/cpu.o src/vm.o src/debugger.o

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

src/main.o: src/main.c include/vm.h include/cpu.h include/debug.h
	$(CC) $(CFLAGS) -c src/main.c -o src/main.o

src/cpu.o: src/cpu.c include/cpu.h include/vm.h include/instructions.h
	$(CC) $(CFLAGS) -c src/cpu.c -o src/cpu.o

src/vm.o: src/vm.c include/vm.h
	$(CC) $(CFLAGS) -c src/vm.c -o src/vm.o

src/debugger.o: src/debugger.c include/debug.h include/vm.h
	$(CC) $(CFLAGS) -c src/debugger.c -o src/debugger.o

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
