# ---- Project ----
SRC    := nuguri.c

# ---- Toolchain ----
CC     := gcc
CSTD   ?= -std=c11
WARN   := -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable
DEFS   :=
CFLAGS ?= $(CSTD) $(WARN) $(DEFS)
LDFLAGS?=
LIBS   ?= -lm

# ---- OS별 분기 ----
ifeq ($(OS),Windows_NT)
    EXE    := nuguri.exe
    RUNBIN := .\$(EXE)
else
    EXE    := nuguri
    RUNBIN := ./$(EXE)
endif

.PHONY: all
all: $(EXE)

$(EXE): $(SRC)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LIBS)

.PHONY: run
run: all
	$(RUNBIN)

.PHONY: clean
ifeq ($(OS),Windows_NT)
clean:
	- if exist "$(EXE)" del /Q "$(EXE)"
else
clean:
	- rm -f "$(EXE)"
endif
