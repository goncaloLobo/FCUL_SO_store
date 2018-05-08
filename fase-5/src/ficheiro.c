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
#include <ctype.h>
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "so.h"
#include "memoria.h"
#include "escalonador.h"
#include "ficheiro.h"
#include "prodcons.h"
#include "tempo.h"

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuracao Config;
//==============================================

struct ficheiros Ficheiros; // informação sobre nomes e handles de ficheiros

void ficheiro_iniciar(char *fic_entrada, char *fic_saida, char *fic_log) {
	//==============================================
	// GUARDAR NOMES DOS FICHEIROS NA ESTRUTURA Ficheiros
	//
	//so_ficheiro_iniciar(fic_entrada, fic_saida, fic_log);
	//==============================================

	Ficheiros.entrada = fic_entrada;
	Ficheiros.saida = fic_saida;
	Ficheiros.log = fic_log;

	//==============================================
	// ABRIR FICHEIRO DE ENTRADA
	// em modo de leitura
	//so_ficheiro_abrir_ficheiro_entrada();
	//==============================================

	Ficheiros.h_entrada = fopen(fic_entrada, "r");
	if (Ficheiros.h_entrada == NULL) {
        perror("Erro ao abrir ficheiro de entrada.\n");
        exit(1);
	}

	// parse do ficheiro de teste
	// esta funcao prepara os campos da estrutura Config (char *)
	// com os dados do ficheiro de entrada
	if (ini_parse_file(Ficheiros.h_entrada, handler, &Config) < 0) {
		printf("Erro a carregar o ficheiro de teste'\n");
		exit(1);
	}

	// agora e' preciso inicializar os restantes campos da estrutura Config

	//==============================================
	// CONTAR PRODUTOS
	// usar strtok para percorrer Config.lista_produtos
	// guardar resultado em Config.PRODUTOS
	//so_ficheiro_contar_produtos();
	//==============================================
	
	int i, contador = 0;
	char* token;
	char listaAux[strlen(Config.lista_produtos)];

	// fazemos uma copia da lista de produtos
	strcpy(listaAux, Config.lista_produtos);

	// e partimos a copia para contar os produtos
	// pois iremos precisar da original mais a frente
	token = strtok(listaAux, " ");

	while (token != '\0') {
		contador++;
		token = strtok(NULL, " ");
	}

	Config.PRODUTOS = contador;


	// iniciar memoria para o vetor com o stock por PRODUTO e semaforo
	memoria_criar_stock();
	prodcons_criar_stock();

	//==============================================
	// LER STOCK DE CADO PRODUTO
	// percorrer Config.lista_produtos e
	// guardar cada valor no vetor Config.stock
	//so_ficheiro_ler_stock();
	//==============================================

	token = strtok(Config.lista_produtos, " ");

	for (i = 0; i < Config.PRODUTOS; i++) {
		Config.stock[i] = atoi(token);
		token = strtok(NULL, " ");
	}

	//==============================================
	// CONTAR CLIENTES
	// usar strtok para percorrer Config.lista_clientes
	// guardar resultado em Config.CLIENTES
	//so_ficheiro_contar_clientes();
	//==============================================

	contador = 0;

	token = strtok(Config.lista_clientes, " ");

	while (token != '\0') {
		contador++;
		token = strtok(NULL, " ");
	}

	Config.CLIENTES = contador;

	//==============================================
	// CONTAR ASSISTENTES
	// usar strtok para percorrer Config.lista_assistentes
	// guardar resultado em Config.ASSISTENTES
	//so_ficheiro_contar_assistentes();
	//==============================================

	contador = 0;

	token = strtok(Config.lista_assistentes, " ");

	while (token != '\0') {
		contador++;
		token = strtok(NULL, " ");
	}

	Config.ASSISTENTES = contador;

	//==============================================
	// CONTAR LOJAS
	// usar strtok para percorrer Config.lista_lojas
	// guardar resultado em Config.LOJAS
	//so_ficheiro_contar_lojas();
	//==============================================

	// ao contrario das outras listas, esta vamos a dividir
	// nas virgulas que encontra, pois sao estas que delimitam
	// cada loja na lista 

	contador = 0;

	token = strtok(Config.lista_lojas, ",");

	while (token != '\0') {
		contador++;
		token = strtok(NULL, ",");
	}

	Config.LOJAS = contador;
	

	so_ficheiro_ler_especialidades();

	//==============================================
	// LER CAPACIDADES DOS BUFFERS
	// usar strtok para percorrer Config.lista_buffers
	// guardar primeiro tamanho em Config.BUFFER_PEDIDO
	// guardar segundo tamanho em Config.BUFFER_ENCOMENDA
	// guardar terceiro tamanho em Config.BUFFER_RECIBO
	//so_ficheiro_ler_capacidades();
	//==============================================

	// percorre os valores no Config.lista_buffers
	// o primeiro tamanho é guardado em Config.BUFFER_PEDIDO
	// e deve ser convertido de char* para int (funcao atoi)

	token = strtok(Config.lista_buffers, " ");
	Config.BUFFER_PEDIDO = atoi(token);
	token = strtok(NULL, " ");
	Config.BUFFER_ENCOMENDA = atoi(token);
	token = strtok(NULL, " ");
	Config.BUFFER_RECIBO = atoi(token);

	//==============================================
	// ABRIR FICHEIRO DE SAIDA (se foi especificado)
	// em modo de escrita
	//so_ficheiro_abrir_ficheiro_saida();
	//==============================================

	if (fic_saida != NULL)
		Ficheiros.h_saida = fopen(fic_saida, "w");

	//==============================================
	// ABRIR FICHEIRO DE LOG (se foi especificado)
	// em modo de escrita
	//so_ficheiro_abrir_ficheiro_log();
	//==============================================

	if (fic_log != NULL)
		Ficheiros.h_log = fopen(fic_log, "w");

}

void ficheiro_destruir() {
	//==============================================
	// LIBERTAR ZONAS DE MEMÓRIA RESERVADAS DINAMICAMENTE
	// que podem ser: Ficheiros.entrada, Ficheiros.saida, Ficheiros.log
	//so_ficheiro_destruir();
	//==============================================

	fclose(Ficheiros.h_entrada);
	
	if (Ficheiros.h_saida != NULL) 
		fclose(Ficheiros.h_saida);

	if (Ficheiros.h_log != NULL) 
		fclose(Ficheiros.h_log);

}

void ficheiro_escrever_log_ficheiro(int etapa, int id) {
	double t_diff;

	if (Ficheiros.h_log != NULL) {

		prodcons_buffers_inicio();

		// guardar timestamp
		t_diff = tempo_ate_agora();

		//==============================================
		// ESCREVER DADOS NO FICHEIRO DE LOG
		//
		// o log deve seguir escrupulosamente o formato definido
		so_ficheiro_escrever_log_ficheiro(etapa, id, t_diff);
		//==============================================

		prodcons_buffers_fim();
	}
}

void ficheiro_escrever_linha(char * linha) {
	//==============================================
	// ESCREVER UMA LINHA NO FICHEIRO DE SAIDA
	//
	//so_ficheiro_escrever_linha(linha);
	//==============================================
	if (Ficheiros.h_saida != NULL) 
		fputs(linha, Ficheiros.h_saida);
}

int stricmp(const char *s1, const char *s2) {
	if (s1 == NULL)
		return s2 == NULL ? 0 : -(*s2);
	if (s2 == NULL)
		return *s1;

	char c1, c2;
	while ((c1 = tolower(*s1)) == (c2 = tolower(*s2))) {
		if (*s1 == '\0')
			break;
		++s1;
		++s2;
	}

	return c1 - c2;
}

int handler(void* user, const char* section, const char* name,
		const char* value) {
	struct configuracao* pconfig = (struct configuracao*) user;

#define MATCH(s, n) stricmp(section, s) == 0 && stricmp(name, n) == 0
	if (MATCH("produtos", "stock")) {
		pconfig->lista_produtos = strdup(value);
	} else if (MATCH("clientes", "produto")) {
		pconfig->lista_clientes = strdup(value);
	} else if (MATCH("assistentes", "lista")) {
		pconfig->lista_assistentes = strdup(value);
	} else if (MATCH("lojas", "especialidades")) {
		pconfig->lista_lojas = strdup(value);
	} else if (MATCH("buffers", "capacidade")) {
		pconfig->lista_buffers = strdup(value);
	} else {
		return 0; /* unknown section/name, error */
	}
	return 1;
}

