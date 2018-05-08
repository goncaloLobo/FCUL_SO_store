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
    // criar um número de processos cliente igual a quant.
    // após a criação de cada processo, chamar cliente_executar
    // e guardar pid do filho no vetor Ind.pid_clientes[n], com n=0,1,...
    //so_main_cliente(quant);
    //==============================================

    int i;

    for (i = 0; i < quant; i++) {
      if ((Ind.pid_clientes[i] = fork()) < 0) {
        printf("Erro ao criar um dos processos cliente.");
        exit(1);
      }
      if (Ind.pid_clientes[i] == 0)
        exit(cliente_executar(i));
    }
}

/* main_assistente recebe como parâmetro o nº de assistentes a criar */
void main_assistente(int quant)
{
    //==============================================
    // CRIAR PROCESSOS
    //
    // criar um número de processos assistente igual a quant.
    // após a criação de cada processo, chamar assistente_executar
    // e guardar pid do filho no vetor Ind.pid_assistentes[n], com n=0,1,...
    //so_main_assistente(quant);
    //==============================================

    int i;

    for (i = 0; i < quant; i++) {
      if ((Ind.pid_assistentes[i] = fork()) < 0) {
        printf("Erro ao criar um dos processos assistente.");
        exit(1);
      }
      if (Ind.pid_assistentes[i] == 0)
        exit(assistente_executar(i));
    }
}

/* main_loja recebe como parâmetro o nº de lojas a criar */
void main_loja(int quant)
{
    //==============================================
    // CRIAR PROCESSOS
    //
    // criar um número de processos loja igual a quant.
    // após a criação de cada processo, chamar loja_executar
    // e guardar pid do filho no vetor Ind.pid_lojas[n], com n=0,1,...
    //so_main_loja(quant);
    //==============================================

    int i;

    for (i = 0; i < quant; i++) {
      if ((Ind.pid_lojas[i] = fork()) < 0) {
        printf("Erro ao criar um dos processos loja.");
        exit(1);
      }
      if (Ind.pid_lojas[i] == 0)
        exit(loja_executar(i));
    }
}

int main(int argc, char *argv[])
{
    char *ficEntrada = NULL;
    char *ficSaida = NULL;
    char *ficLog = NULL;
    long intervalo = 0;
    int i, res, param;

    //==============================================
    // TRATAR PARÂMETROS DE ENTRADA
    // parâmetro obrigatório: ficheiro_configuracao
    // parâmetros opcionais:
    //   ficheiro_resultados
    //   -l ficheiro_log
    //   -t intervalo(us)    // us: microsegundos
    //
    // para qualquer parâmetro desconhecido ou falta de parâmetros
    // escrever mensagem de utilização "Como usar", dar exemplo e terminar
    //intervalo = so_main_args(argc, argv, &ficEntrada, &ficSaida, &ficLog);
    //==============================================

    void msgComoUsar() {
        printf("Utilização: sostore ficheiro_configuracao [ficheiro_resultados] -l [ficheiro_log] -t [intervalor(us)]\n");
        printf("Exemplo: sostore ../testes/in/cenario1 ..testes/out/cenario1 -l cenario1.log -t 1000\n\n");
    }

    if (argc == 1) {
      msgComoUsar();
      return -1;
    }

    ficEntrada = argv[1];

    opterr = 0;
    while ((param = getopt(argc, argv, "l:t:")) != -1) {
      switch (param) {
        case 'l':
          ficLog = optarg;
          break;
        case 't':
          intervalo = atoi(optarg);
          break;
        case '?':
          msgComoUsar();
          return -1;
      }
    }

    ficSaida = argv[optind+1];

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
    // esperar e incrementar o indicador de clientes
    // Ind.produtos_obtidos_pelos_clientes[n], n=0,1,...
    // com o estado devolvido pela terminação do processo
    //so_main_wait_clientes();
    //==============================================

    for (i = 0; i < Config.CLIENTES; i++) {
      waitpid(Ind.pid_clientes[i], &res, 0);
      if (WIFEXITED(res))
        Ind.produtos_obtidos_pelos_clientes[WEXITSTATUS(res)] += 1;
      else
        exit(0);
    }

    printf("\n\t\t\t*** Fechar SO_Store ***\n\n");
    controlo_fechar_sostore();

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS CLIENTES E ATUALIZAR INDICADORES
    //
    // esperar e incrementar o indicador de assistentes
    // Ind.clientes_atendidos_pelas_assistentes[n], n=0,1,...
    // com o estado devolvido pela terminação do processo
    //so_main_wait_assistentes();
    //==============================================

    for (i = 0; i < Config.ASSISTENTES; i++) {
        waitpid(Ind.pid_assistentes[i], &res, 0);
        Ind.clientes_atendidos_pelas_assistentes[i] = WEXITSTATUS(res);
    }

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS CLIENTES E ATUALIZAR INDICADORES
    //
    // esperar e incrementar o indicador de lojas
    // Ind.clientes_atendidos_pelas_lojas[n], n=0,1,...
    // com o estado devolvido pela terminação do processo
    //so_main_wait_lojas();
    //==============================================

    for (i = 0; i < Config.LOJAS; i++) {
        waitpid(Ind.pid_lojas[i], &res, 0);
        Ind.clientes_atendidos_pelas_lojas[i] = WEXITSTATUS(res);
    }

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
