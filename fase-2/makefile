OBJ_LIST = assistente cliente controlo escalonador ficheiro loja main memoria prodcons tempo
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin
LOG_DIR = testes/log
OUT_DIR = testes/out
FLAGS = -I $(INC_DIR) -g -Wall

define create-object
	
	gcc -c $(SRC_DIR)/$(1).c -o $(OBJ_DIR)/$(1).o $(FLAGS)
endef

all: sostore

create-all-objects:
	$(foreach obj,$(OBJ_LIST),$(call create-object,$(obj)))

sostore: create-all-objects
	gcc $(OBJ_DIR)/*.o -o $(BIN_DIR)/sostore -lpthread -lrt

clean:
	rm -f $(foreach obj,$(OBJ_LIST),$(OBJ_DIR)/$(obj).o) $(BIN_DIR)/sostore
	make cleanoutput

cleanoutput:
	rm -f $(LOG_DIR)/*.log
	rm -f $(OUT_DIR)/*
