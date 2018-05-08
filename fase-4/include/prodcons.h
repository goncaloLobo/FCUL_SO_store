#ifndef PRODCONS_H_GUARD
#define PRODCONS_H_GUARD

// Nomes usados na criacao dos semaforos
#define STR_SEM_STOCK_MUTEX			"sem_stock_mutex"
#define STR_SEM_PEDIDO_FULL 		"sem_pedido_full"
#define STR_SEM_PEDIDO_EMPTY 		"sem_pedido_empty"
#define STR_SEM_PEDIDO_MUTEX 		"sem_pedido_mutex"
#define STR_SEM_ENCOMENDA_FULL 		"sem_encomenda_full"
#define STR_SEM_ENCOMENDA_EMPTY 	"sem_encomenda_empty"
#define STR_SEM_ENCOMENDA_MUTEX 	"sem_encomenda_mutex"
#define STR_SEM_RECIBO_FULL 		"sem_recibo_full"
#define STR_SEM_RECIBO_EMPTY 		"sem_recibo_empty"
#define STR_SEM_RECIBO_MUTEX 		"sem_recibo_mutex"

struct prodcons {
	// semáforos de controlo do acesso ao buffer entre lojas e clientes
	sem_t *recibos_r_full, *recibos_r_empty, *recibos_r_mutex;
	// semáforos de controlo do acesso ao buffer entre assistentes e lojas
	sem_t *pedido_e_full, *pedido_e_empty, *pedido_e_mutex;
	// semáforos de controlo do acesso ao buffer entre clientes e assistentes
	sem_t *pedido_p_full, *pedido_p_empty, *pedido_p_mutex;
	// semaforo para exclusao mutua no acesso ao stock
	sem_t *stock_mutex;
};

sem_t * semaforo_criar(char*, int);
void prodcons_criar_acordar();
void prodcons_criar_stock();
void prodcons_criar_buffers();
void prodcons_criar_clientes();
void prodcons_terminar(char*, void*);
void prodcons_destruir();
void prodcons_pedido_p_produzir_inicio();
void prodcons_pedido_p_produzir_fim();
void prodcons_pedido_p_consumir_inicio();
void prodcons_pedido_p_consumir_fim();
void prodcons_pedido_e_produzir_inicio();
void prodcons_pedido_e_produzir_fim();
void prodcons_pedido_e_consumir_inicio();
void prodcons_pedido_e_consumir_fim();
void prodcons_recibo_r_produzir_inicio();
void prodcons_recibo_r_produzir_fim();
void prodcons_recibo_r_consumir_inicio();
void prodcons_recibo_r_consumir_fim();
void prodcons_buffers_inicio();
void prodcons_buffers_fim();
int prodcons_atualizar_stock(int);

#endif
