CC = gcc
CFLAGS = -Wall -g
LIBS = -lncurses

TARGET = dedit
# 注意：不再用 wildcard *.c，因为 dedit.c 和 level_2.5.c 都有 main
DEDIT_OBJS   = dedit.o level_2.1.o level_2.2.o level_2.3.o level_2.4.o level_3.1.o
TEST_TARGET  = test_runner
TEST_OBJS    = level_2.5.o level_2.1.o level_2.2.o level_2.3.o level_2.4.o level_3.1.o

all: $(TARGET)

$(TARGET): $(DEDIT_OBJS)
	$(CC) $(DEDIT_OBJS) -o $(TARGET) $(LIBS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -o $(TEST_TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o

.PHONY: all test clean