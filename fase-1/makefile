SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin
LOG_DIR = testes/log
OUT_DIR = testes/out
FLAGS = -I $(INC_DIR) -g -Wall
ALL_OBJ = $(OBJ_DIR)/assistente.o $(OBJ_DIR)/cliente.o $(OBJ_DIR)/controlo.o $(OBJ_DIR)/escalonador.o $(OBJ_DIR)/ficheiro.o $(OBJ_DIR)/loja.o $(OBJ_DIR)/main.o $(OBJ_DIR)/memoria.o $(OBJ_DIR)/prodcons.o $(OBJ_DIR)/tempo.o $(OBJ_DIR)/so.o
SO_OBJ = $(OBJ_DIR)/so.o

all: sostore

$(OBJ_DIR)/assistente.o: $(SRC_DIR)/assistente.c
	gcc -c $(SRC_DIR)/assistente.c -o $(OBJ_DIR)/assistente.o $(FLAGS)

$(OBJ_DIR)/cliente.o: $(SRC_DIR)/cliente.c
	gcc -c $(SRC_DIR)/cliente.c -o $(OBJ_DIR)/cliente.o $(FLAGS)

$(OBJ_DIR)/controlo.o: $(SRC_DIR)/controlo.c
	gcc -c $(SRC_DIR)/controlo.c -o $(OBJ_DIR)/controlo.o $(FLAGS)

$(OBJ_DIR)/escalonador.o: $(SRC_DIR)/escalonador.c
	gcc -c $(SRC_DIR)/escalonador.c -o $(OBJ_DIR)/escalonador.o $(FLAGS)

$(OBJ_DIR)/ficheiro.o: $(SRC_DIR)/ficheiro.c
	gcc -c $(SRC_DIR)/ficheiro.c -o $(OBJ_DIR)/ficheiro.o $(FLAGS)

$(OBJ_DIR)/loja.o: $(SRC_DIR)/loja.c
	gcc -c $(SRC_DIR)/loja.c -o $(OBJ_DIR)/loja.o $(FLAGS)

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c
	gcc -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o $(FLAGS)

$(OBJ_DIR)/memoria.o: $(SRC_DIR)/memoria.c
	gcc -c $(SRC_DIR)/memoria.c -o $(OBJ_DIR)/memoria.o $(FLAGS)

$(OBJ_DIR)/prodcons.o: $(SRC_DIR)/prodcons.c
	gcc -c $(SRC_DIR)/prodcons.c -o $(OBJ_DIR)/prodcons.o $(FLAGS)

$(OBJ_DIR)/tempo.o: $(SRC_DIR)/tempo.c
	gcc -c $(SRC_DIR)/tempo.c -o $(OBJ_DIR)/tempo.o $(FLAGS)

sostore: $(ALL_OBJ)
	gcc $(ALL_OBJ) -o $(BIN_DIR)/sostore -lpthread -lrt

clean:
	rm -f $(filter-out $(SO_OBJ),$(ALL_OBJ)) $(BIN_DIR)/sostore
	make cleanoutput

cleanoutput:
	rm -f $(LOG_DIR)/*.log
	rm -f $(OUT_DIR)/*
