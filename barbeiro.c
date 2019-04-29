/*
    Estudo Dirigido 7 - Barbeiro dorminhoco
    Thiago Veras Machado
    16/0146682
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h> 

#define NUMTHRDS 10
#define CADEIRASLEN 5
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

pthread_t threads[NUMTHRDS];
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;
sem_t cadeira_espera, cadeira_barbeiro, disponibilidade_barbeiro; 
int cadeira_b;

char* color[6] = {
    "\x1b[31m", "\x1b[32m", "\x1b[33m",
    "\x1b[34m", "\x1b[35m", "\x1b[36m"
};

char* name[9] = {
    "José", "Jefferson", "Geraldo",
    "Genivaldo", "Neide", "Lurdes",
    "Francisca", "Zulmira", "Matilde"
};

struct{
    int idx;
    int cadeiras[CADEIRASLEN];
}sala_de_espera;

void sentar(int id){
    pthread_mutex_lock(&mutex_lock);
    int n = sala_de_espera.idx;
    sala_de_espera.cadeiras[n] = id;
    sala_de_espera.idx++;
    printf(GREEN "%s sentando no banco %d e o fila ficou : " RESET, name[id - 1], n+1);
    for(int i = 0; i < sala_de_espera.idx; i++){
        if(i) printf(" - ");
        printf(GREEN "%s" RESET, name[sala_de_espera.cadeiras[i]-1]);
    }
    printf("\n");
    pthread_mutex_unlock(&mutex_lock);
}

void levantar(){
    pthread_mutex_lock(&mutex_lock);
    int id = sala_de_espera.cadeiras[0];
    for(int i = 1; i < sala_de_espera.idx; i++)
        sala_de_espera.cadeiras[i-1] = sala_de_espera.cadeiras[i];
    sala_de_espera.idx--;
    pthread_mutex_unlock(&mutex_lock);
}

void *espera(void *threadid){
    int id = (int)(long)threadid;
    while(1){
        sleep(rand()%4 + 2);
        int try_wait = sem_trywait(&cadeira_espera);
        if(try_wait == 0){
            sentar(id);
            sem_wait(&cadeira_barbeiro);
            levantar();
            sem_post(&cadeira_espera);
            printf(BLUE "%s sentou pra cortar\n" RESET, name[id - 1]);
            cadeira_b = id;
            sem_post(&disponibilidade_barbeiro);
        }
        else{
            printf(RED "%s indo embora do barbeiro pois esta cheio\n" RESET, name[id - 1]);
        }
    }
}

void *barbeiro(){
    while(1){
        sem_wait(&disponibilidade_barbeiro);
        printf(YELLOW "Barbeiro cortando o cabelo de %s\n" RESET, name[cadeira_b - 1]);
        sleep(2);
        printf(YELLOW "Barbeiro finalizou o corte de %s que já foi embora\n" RESET, name[cadeira_b - 1]);
        sem_post(&cadeira_barbeiro);
    }
}

int main(){ 

    sala_de_espera.idx = 0;

    sem_init(&cadeira_barbeiro, 0, 1);
    sem_init(&disponibilidade_barbeiro, 0, 0);
    sem_init(&cadeira_espera, 0, CADEIRASLEN);

    pthread_create(&threads[0], NULL, barbeiro, NULL);

    for(int i = 1; i < NUMTHRDS; i++)
        pthread_create(&threads[i], NULL, espera, (void*)(long)i);

    for (int i = 0; i < NUMTHRDS - 1; i++) 
        pthread_join(threads[i], NULL);

    return 0;
}