CC=clang
CDBG=gdb
OBJ=tedit

all:
	$(CC)  -Wall -Wno-typedef-redefinition -I/usr/include -I/usr/local/include -L/usr/local/lib -lm -lcurses -g -fno-limit-debug-info -o $(OBJ) *.c
clean:
	@rm -f $(OBJ) $(OBJ).core
debug:
	$(CDBG) ./$(OBJ) $(OBJ).core
