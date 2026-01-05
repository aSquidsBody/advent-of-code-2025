CC      = clang
CFLAGS  = -g -O0 -Wall -Wextra
TARGET  = util.exe
SRC     = util.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

