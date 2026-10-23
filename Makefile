CC = gcc

CFLAGS = -std=c11 -Iinclude -Wall -Wextra -Wpedantic -Wconversion \
         -Wshadow -g3 -O1 \
         -fsanitize=address,undefined \
         -fno-omit-frame-pointer \
         -MMD -MP

LDFLAGS = -fsanitize=address,undefined

TARGET = minivm16

SRCS = $(wildcard src/vm/*.c src/assembler/*.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

.PHONY: clean
