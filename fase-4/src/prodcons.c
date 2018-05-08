#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h> //mmap
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "so.h"
#include "controlo.h"
#include "prodcons.h"

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuracao Config;
//==============================================

struct prodcons ProdCons;

//******************************************
// SEMAFORO_CRIAR
//
sem_t * semaforo_criar(char * name, int value) {
	//==============================================
	// FUNÇÃO GENÉRICA DE CRIAÇÃO DE UM SEMÁFORO
	//
	//return so_semaforo_criar(name, value);
	//==============================================
	sem_t *semaforo;
	semaforo = sem_open(name, O_CREAT, 0xFFFFFFFF, value); 
	if(semaforo == SEM_FAILED){ 
		perror("Erro na criacao do semaforo"); 
		exit(1); 
	} 
	return semaforo;
}

void prodcons_criar_stock() {
	//==============================================
	// CRIAR MUTEX PARA CONTROLAR O ACESSO AO STOCK
	//
	// utilizar a função genérica semaforo_criar
	//so_prodcons_criar_stock();
	//==============================================
    ProdCons.stock_mutex = semaforo_criar(STR_SEM_STOCK_MUTEX, 1);
}

void prodcons_criar_buffers() {
	//==============================================
	// CRIAR SEMAFOROS PARA CONTROLAR O ACESSO AOS 3 BUFFERS
	//
	// utilizar a função genérica semaforo_criar
	//so_prodcons_criar_buffers();
	//==============================================
    ProdCons.recibos_r_full = semaforo_criar(STR_SEM_RECIBO_FULL, 0);
	ProdCons.recibos_r_empty = semaforo_criar(STR_SEM_RECIBO_EMPTY, Config.BUFFER_RECIBO);
	ProdCons.recibos_r_mutex = semaforo_criar(STR_SEM_RECIBO_MUTEX, 1);

	ProdCons.pedido_p_full = semaforo_criar(STR_SEM_PEDIDO_FULL, 0);
	ProdCons.pedido_p_empty = semaforo_criar(STR_SEM_PEDIDO_EMPTY, Config.BUFFER_PEDIDO);
	ProdCons.pedido_p_mutex = semaforo_criar(STR_SEM_PEDIDO_MUTEX, 1);

	ProdCons.pedido_e_full = semaforo_criar(STR_SEM_ENCOMENDA_FULL, 0);
	ProdCons.pedido_e_empty = semaforo_criar(STR_SEM_ENCOMENDA_EMPTY, Config.BUFFER_ENCOMENDA);
	ProdCons.pedido_e_mutex = semaforo_criar(STR_SEM_ENCOMENDA_MUTEX, 1);
}

void semaforo_terminar(char * name, void * ptr) {
	//==============================================
	// FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE UM SEMÁFORO
	//
	//so_semaforo_terminar(name, ptr);
	//==============================================
    if (sem_close(ptr) == -1)  
		perror(name); 
	
	if(sem_unlink(name) == -1) 
		perror(name);
}

void prodcons_destruir() {
	//==============================================
	// DESTRUIR SEMÁFORO E RESPETIVO NOME
	//
	// utilizar a função genérica semaforo_terminar
	//so_prodcons_destruir();
	//==============================================
	semaforo_terminar(STR_SEM_RECIBO_FULL, ProdCons.recibos_r_full);
	semaforo_terminar(STR_SEM_RECIBO_EMPTY, ProdCons.recibos_r_empty);
	semaforo_terminar(STR_SEM_RECIBO_MUTEX, ProdCons.recibos_r_mutex);

	semaforo_terminar(STR_SEM_PEDIDO_FULL, ProdCons.pedido_p_full);
	semaforo_terminar(STR_SEM_PEDIDO_EMPTY, ProdCons.pedido_p_empty);
	semaforo_terminar(STR_SEM_PEDIDO_MUTEX, ProdCons.pedido_p_mutex);

	semaforo_terminar(STR_SEM_ENCOMENDA_FULL, ProdCons.pedido_e_full);
	semaforo_terminar(STR_SEM_ENCOMENDA_EMPTY, ProdCons.pedido_e_empty);
	semaforo_terminar(STR_SEM_ENCOMENDA_MUTEX, ProdCons.pedido_e_mutex);

	semaforo_terminar(STR_SEM_STOCK_MUTEX, ProdCons.stock_mutex);
}

//******************************************
void prodcons_pedido_p_produzir_inicio() {
	//==============================================
	// CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE PRODUTO
	//
	//so_prodcons_pedido_p_produzir_inicio();
	//==============================================
    /*if (sem_wait(ProdCons.pedido_p_full) == -1) {
		perror("sem wait pedido_p"); 
		exit(2);
	}
		
	if (sem_post(ProdCons.pedido_p_empty) == -1) { 
		perror("sem post pedido_p"); 
		exit(2);
	}*/
	sem_wait(ProdCons.pedido_p_empty);
	sem_wait(ProdCons.pedido_p_mutex);
}

//******************************************
void prodcons_pedido_p_produzir_fim() {
	//==============================================
	// CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE PRODUTO
	//
	//so_prodcons_pedido_p_produzir_fim();
	//==============================================
	sem_post(ProdCons.pedido_p_mutex);
	sem_post(ProdCons.pedido_p_full);
}

//******************************************
void prodcons_pedido_p_consumir_inicio() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE PRODUTO
	//
	//so_prodcons_pedido_p_consumir_inicio();
	//==============================================
	sem_wait(ProdCons.pedido_p_full);
	sem_wait(ProdCons.pedido_p_mutex);
}

//******************************************
void prodcons_pedido_p_consumir_fim() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE PRODUTO
	//
	//so_prodcons_pedido_p_consumir_fim();
	//==============================================
	sem_post(ProdCons.pedido_p_mutex);
	sem_post(ProdCons.pedido_p_empty);
}

//******************************************
void prodcons_pedido_e_produzir_inicio() {
	//==============================================
	// CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE ENCOMENDA
	//
	//so_prodcons_pedido_e_produzir_inicio();
	//==============================================
	sem_wait(ProdCons.pedido_e_empty);
	sem_wait(ProdCons.pedido_e_mutex);
}

//******************************************
void prodcons_pedido_e_produzir_fim() {
	//==============================================
	// CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE ENCOMENDA
	//
	//so_prodcons_pedido_e_produzir_fim();
	//==============================================
	sem_post(ProdCons.pedido_e_mutex);
	sem_post(ProdCons.pedido_e_full);
}

//******************************************
void prodcons_pedido_e_consumir_inicio() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE ENCOMENDA
	//
	//so_prodcons_pedido_e_consumir_inicio();
	//==============================================
	sem_wait(ProdCons.pedido_e_full);
	sem_wait(ProdCons.pedido_e_mutex);
}

//******************************************
void prodcons_pedido_e_consumir_fim() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE ENCOMENDA
	//
	//so_prodcons_pedido_e_consumir_fim();
	//==============================================
	sem_post(ProdCons.pedido_e_mutex);
	sem_post(ProdCons.pedido_e_empty);
}

//******************************************
void prodcons_recibo_r_produzir_inicio() {
	//==============================================
	// CONTROLAR ACESSO DE PRODUTOR AO BUFFER RECIBO
	//
	//so_prodcons_recibo_r_produzir_inicio();
	//==============================================
	sem_wait(ProdCons.recibos_r_empty);
	sem_wait(ProdCons.recibos_r_mutex);
}

//******************************************
void prodcons_recibo_r_produzir_fim() {
	//==============================================
	// CONTROLAR ACESSO DE PRODUTOR AO BUFFER RECIBO
	//
	//so_prodcons_recibo_r_produzir_fim();
	//==============================================
	sem_post(ProdCons.recibos_r_mutex);
	sem_post(ProdCons.recibos_r_full);
}

//******************************************
void prodcons_recibo_r_consumir_inicio() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER RECIBO
	//
	//so_prodcons_recibo_r_consumir_inicio();
	//==============================================
	sem_wait(ProdCons.recibos_r_full);
	sem_wait(ProdCons.recibos_r_mutex);
}

//******************************************
void prodcons_recibo_r_consumir_fim() {
	//==============================================
	// CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER RECIBO
	//
	//so_prodcons_recibo_r_consumir_fim();
	//==============================================
	sem_post(ProdCons.recibos_r_mutex);
	sem_post(ProdCons.recibos_r_empty);
}

//******************************************
void prodcons_buffers_inicio() {
	//==============================================
	// GARANTIR EXCLUSÃO MÚTUA NO ACESSO AOS 3 BUFFERS
	//
	//so_prodcons_buffers_inicio();
	//==============================================
	sem_wait(ProdCons.pedido_p_mutex);
	sem_wait(ProdCons.pedido_e_mutex);
	sem_wait(ProdCons.recibos_r_mutex);
}

//******************************************
void prodcons_buffers_fim() {
	//==============================================
	// FIM DA ZONA DE EXCLUSÃO MÚTUA NO ACESSO AOS 3 BUFFERS
	//
	//so_prodcons_buffers_fim();
	//==============================================
	sem_post(ProdCons.pedido_p_mutex);
	sem_post(ProdCons.pedido_e_mutex);
	sem_post(ProdCons.recibos_r_mutex);
}

//******************************************
int prodcons_atualizar_stock(int produto) {
	//==============================================
	// OBTER MUTEX DO STOCK E ATUALIZAR STOCK
	//
	// se stock de produto>0 então decrementá-lo de uma unidade e 
	//   função devolve 1
	// se stock de produto=0 então função devolve 0
	//return so_prodcons_atualizar_stock(produto);
	//==============================================
    int res;

	if (sem_wait(ProdCons.stock_mutex) == -1) {
		perror("sem wait mutex stock"); 
		exit(15);
	}

	if (Config.stock[produto] > 0) {
		Config.stock[produto]--;
		res = Config.stock[produto] >= 0;
	}

	if (sem_post(ProdCons.stock_mutex) == -1) { 
		perror("sem post mutex stock"); 
		exit(15);
	}

	return res;
}
