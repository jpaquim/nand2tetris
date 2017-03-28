CFLAGS = -Wall -Wextra
EXE = main
SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=%.o)

$(EXE): $(OBJ)

$(OBJ): *.h

clean:
	$(RM) $(OBJ) $(EXE)
