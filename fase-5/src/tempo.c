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
#include "tempo.h"

//==============================================
// DECLARAR ACESSO A DADOS EXTERNOS
//
extern struct configuracao Config;
//==============================================

struct timespec t_inicial;
long intervalo_alarme;

void tempo_iniciar(long intervalo) {
    //==============================================
    // INICIAR ESTRUTURA t_inicial COM VALOR DE RELOGIO (CLOCK_REALTIME)
	//
	// funções de tempo:
	// - clock_gettime() dá um resultado em nanosegundos
	// - gettimeofday()  dá um resultado em milisegundos
	// como a função clock_gettime() dá um valor mais preciso do que gettimeofday()
	// deve ser usada clock_gettime()
	//
    // fazer:
	// - se intervalo!=0 então intervalo_alarme = intervalo
	// - se intervalo!=0 então chamar tempo_armar_alarme();
	// - iniciar estrutura t_inicial com clock_gettime usando CLOCK_REALTIME
    //so_tempo_iniciar(intervalo);
    //==============================================

    if (intervalo != 0) {
        intervalo_alarme = intervalo;
        tempo_armar_alarme();
    }

    clock_gettime(CLOCK_REALTIME, &t_inicial);

}

void tempo_terminar(long intervalo)
{
    //==============================================
    // DESATIVAR ALARME
    //
	// desassociar SIGALRM da função tempo_escrever_log_temporizado
    //so_tempo_terminar(intervalo);
    //==============================================

    signal(SIGALRM, SIG_IGN);

}

void tempo_armar_alarme() {
    //==============================================
    // ARMAR ALARME DE ACORDO COM intervalo_alarme (SIGNAL E SETTIMER)
    //
	// fazer:
	// - associar SIGALRM com a função tempo_escrever_log_temporizado
	// - usar setitimer preenchendo apenas os campos value da estrutura
    //so_tempo_armar_alarme();
    //==============================================

    struct itimerval t_alarme;

    signal(SIGALRM, tempo_escrever_log_temporizado);

    t_alarme.it_value.tv_sec = 0;
    t_alarme.it_value.tv_usec = intervalo_alarme;
    t_alarme.it_interval.tv_sec = 0;
    t_alarme.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &t_alarme, NULL);

}

void tempo_escrever_log_temporizado() {
    //==============================================
    // ESCREVER LOG NO ECRAN DE FORMA TEMPORIZADA 
    //
	// rearmar alarme chamando novamente tempo_armar_alarme
	// escrever para o ecrã a informação esperada
    //printf("OLA");
    //so_tempo_escrever_log_temporizado();
    //==============================================

    int i;

    tempo_armar_alarme();

    printf("\tProdutos:\t");
    for(i = 0; i < Config.PRODUTOS; i++) {
        printf("0%d\t", i);
    }
    printf("\n");

    printf("\tStock:\t\t");
    for(i = 0; i < Config.PRODUTOS; i++) {
        printf("0%d\t", Config.stock[i]);
    }
    printf("\n");

}

double tempo_diferenca(struct timespec t1, struct timespec t2) {
    //==============================================
    // CALCULAR A DIFERENCA, ENTRE t1 E t2 (com segundos e nanosegundos)
    // o resultado deve estar em segundos representado como um double
	// realizar as operações aritméticas necessárias para obter o resultado
    //return so_tempo_diferenca(t1,t2);
    //==============================================
    
    struct timespec res;

    if ((t2.tv_nsec - t1.tv_nsec) < 0) {
        res.tv_sec = t2.tv_sec - t1.tv_sec - 1;
        res.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        res.tv_sec = t2.tv_sec - t1.tv_sec;
        res.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }

    return res.tv_nsec / 1000000000.0;

}

double tempo_ate_agora() {
    //==============================================
    // CALCULAR O INTERVALO DE TEMPO ENTRE t_inicial E O INSTANTE ATUAL
    //
	// fazer:
	// - obter o tempo atual com clock_gettime
	// - chamar tempo_diferenca
    //return so_tempo_ate_agora();
    //==============================================

    struct timespec t_atual;

    clock_gettime(CLOCK_REALTIME, &t_atual);

    return tempo_diferenca(t_inicial, t_atual);

}

void tempo_registar(struct timespec *t) {
    //==============================================
    // REGISTAR O TEMPO ATUAL EM t (CLOCK_REALTIME)
    //
	// usar clock_gettime com CLOCK_REALTIME
    //so_tempo_registar(t);
    //==============================================

    clock_gettime(CLOCK_REALTIME, t);

}

void tempo_processamento_encomenda() {
	//==============================================
	// ADORMECER POR 1 MILISEGUNDO
	//
	// usar usleep
	//so_tempo_processamento_encomenda();
	//==============================================

    usleep(1000);

}
