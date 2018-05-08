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
#include "cliente.h"
#include "memoria.h"
#include "prodcons.h"
#include "controlo.h"
#include "ficheiro.h"
#include "tempo.h"
#include "so.h"
#include "loja.h"
#include "assistente.h"

struct indicadores Ind;     // indicadores do funcionamento do SO_Store
struct configuracao Config; // configuração da execução do SO_Store

/* main_cliente recebe como parâmetro o nº de clientes a criar */
void main_cliente(int quant)
{
    //==============================================
    // CRIAR PROCESSOS
    //
    // criar um número de processos cliente igual a quant através de um ciclo com n=0,1,...
	// após a criação de cada processo, o processo filho realiza duas atividades:
	// - chama a função cliente_executar passando como parâmetro o valor da variável de controlo do ciclo n=0,1,...
	// - chama a função exit passando como parâmetro o valor devolvido pela função cliente_executar
	// o processo pai guarda o pid do filho no vetor Ind.pid_clientes[n], com n=0,1,... e termina normalmente a função
    so_main_cliente(quant);
    //==============================================
}

/* main_assistente recebe como parâmetro o nº de assistentes a criar */
void main_assistente(int quant)
{
    //==============================================
    // CRIAR PROCESSOS
    //
    // criar um número de processos cliente igual a quant através de um ciclo com n=0,1,...
	// após a criação de cada processo, o processo filho realiza duas atividades:
	// - chama a função cliente_executar passando como parâmetro o valor da variável de controlo do ciclo n=0,1,...
	// - chama a função exit passando como parâmetro o valor devolvido pela função assistente_executar
	// o processo pai guarda o pid do filho no vetor Ind.pid_assistentes[n], com n=0,1,... e termina normalmente a função
    so_main_assistente(quant);
    //==============================================
}

/* main_loja recebe como parâmetro o nº de lojas a criar */
void main_loja(int quant)
{
    //==============================================
    // CRIAR PROCESSOS
    //
    // criar um número de processos cliente igual a quant através de um ciclo com n=0,1,...
	// após a criação de cada processo, o processo filho realiza duas atividades:
	// - chama a função cliente_executar passando como parâmetro o valor da variável de controlo do ciclo n=0,1,...
	// - chama a função exit passando como parâmetro o valor devolvido pela função loja_executar
	// o processo pai guarda o pid do filho no vetor Ind.pid_lojas[n], com n=0,1,... e termina normalmente a função
    so_main_loja(quant);
    //==============================================
}

int main(int argc, char *argv[])
{
    char *ficEntrada = NULL;
    char *ficSaida = NULL;
    char *ficLog = NULL;
    long intervalo = 0;

    //==============================================
    // TRATAR PARÂMETROS DE ENTRADA
    // parâmetro obrigatório: ficheiro_configuracao
    // parâmetros opcionais:
    //   ficheiro_resultados
    //   -l ficheiro_log
    //   -t intervalo(us)    // us: microsegundos
    //
	// ignorar parâmetros desconhecidos
	// em caso de ausência de parâmetros escrever mensagem de como utilizar o programa e terminar
	// considerar que os parâmetros apenas são introduzidos na ordem indicada pela mensagem
	// considerar que são sempre introduzidos valores válidos para os parâmetros
    intervalo = so_main_args(argc, argv, &ficEntrada, &ficSaida, &ficLog);
    //==============================================

    printf(
	"\n------------------------------------------------------------------------");
    printf(
	"\n-------------------------- Aplicacao SO_Store --------------------------");
    printf(
	"\n------------------------------------------------------------------------\n");

    // Ler dados de entrada
    ficheiro_iniciar(ficEntrada, ficSaida, ficLog);

    // criar zonas de memória e semáforos
    memoria_criar_buffers();
    prodcons_criar_buffers();
    controlo_criar();

    // Criar estruturas para indicadores e guardar stock inicial
    memoria_criar_indicadores();

    printf("\n\t\t\t*** Abrir SO_Store ***\n\n");
    controlo_abrir_sostore();

    // Registar início de operação e armar alarme
    tempo_iniciar(intervalo);

    //
    // Iniciar sistema
    //

    // Criar ASSISTENTES
    main_assistente(Config.ASSISTENTES);
    // Criar LOJAS
    main_loja(Config.LOJAS);
    // Criar CLIENTES
    main_cliente(Config.CLIENTES);

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS CLIENTES E ATUALIZAR INDICADORES
    //
    // esperar por cada cliente individualmente
	// se o cliente terminou normalmente 
	// então incrementar o indicador de produtos obtidos
    // Ind.produtos_obtidos_pelos_clientes[n], n=0,1,...
    so_main_wait_clientes();
    //==============================================

    // Desarmar alarme
    tempo_terminar(intervalo);

    printf("\n\t\t\t*** Fechar SO_Store ***\n\n");
    controlo_fechar_sostore();

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS ASSISTENTES E ATUALIZAR INDICADORES
    //
    // esperar por cada assistente individualmente
	// se a assistente terminou normalmente 
	// então atualizar o indicador de clientes atendidos
    // Ind.clientes_atendidos_pelas_assistentes[n], n=0,1,...
    // guardando nele o estado devolvido pela terminação do processo
    so_main_wait_assistentes();
    //==============================================

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DAS LOJAS E ATUALIZAR INDICADORES
    //
    // esperar por cada loja individualmente
	// se a loja terminou normalmente 
	// então atualizar o indicador de clientes atendidos
    // Ind.clientes_atendidos_pelas_lojas[n], n=0,1,...
    // guardando nele o estado devolvido pela terminação do processo
    so_main_wait_lojas();
    //==============================================

    printf(
	"------------------------------------------------------------------------\n\n");
    printf("\t\t\t*** Indicadores ***\n\n");
    so_escreve_indicadores();
    printf("\t\t\t*******************\n");

    // destruir zonas de memória e semáforos
    ficheiro_destruir();
    controlo_destruir();
    prodcons_destruir();
    memoria_destruir();

    return 0;
}
