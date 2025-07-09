CC ?= gcc
CFLAGS ?= -Wall
TARGET ?= life
SRC = life.c

# Detect platform
ifeq ($(OS),Windows_NT)
  RM    = del /Q
  EXEEXT = .exe
else
  RM    = rm -f
  EXEEXT =
endif


.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean: 
	$(RM) $(TARGET)$(EXEEXT)
