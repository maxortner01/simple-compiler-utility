SRC_DIR = ../src/source
OBJ_DIR = ../obj
INC_DIR = ,../src/headers
OUT_DIR = ../bin
LIB_DIR =

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

SRC_FILES := $(call rwildcard, $(SRC_DIR), *.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
CURRENT_PATH := $(shell pwd)

EXE_NAME = main
EXTENSION = 

empty:=
comma:= ,
space:=$(empty) $(empty)

main: $(OBJ_FILES)
	g++ -o $(OUT_DIR)/$(EXE_NAME)$(EXTENSION) $^ -static

clean:
	rm $(OBJ_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ -c -o $@ $< $(subst $(comma),$(space)-I,$(INC_DIR))
	
run:
	$(OUT_DIR)/$(EXE_NAME)$(EXTENSION)