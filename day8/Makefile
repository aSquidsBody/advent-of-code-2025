CC      = clang
CFLAGS  = -g -O0 -Wall -Wextra
TARGET  = c.exe
SRC     = ../heap.c ../logging.c c/main.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

