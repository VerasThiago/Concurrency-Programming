/*
    Estudo Dirigido 6 - Semáforos
    Thiago Veras Machado
    16/0146682
*/



#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h> 

#define NUMTHRDS 2
#define VECLEN 5
#define N 3
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

int database[VECLEN];
pthread_t threads[NUMTHRDS];
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;
sem_t vazio, consumo; 
int readers;
int prod_sleep = 1;
int consu_sleep = 5;
int turn;


struct buffers{

    int idx;
    int v[100];

}buffer;


char* color[6] = {
    "\x1b[31m", "\x1b[32m", "\x1b[33m",
    "\x1b[34m", "\x1b[35m", "\x1b[36m"
};


void swap_time(){
    int aux = prod_sleep;
    prod_sleep = consu_sleep;
    consu_sleep = aux;
    turn = !turn;
}

void inserir(){
    pthread_mutex_lock(&mutex_lock);
    int item = rand()%500;
    buffer.idx++;
    buffer.v[buffer.idx] = item;
    printf(MAGENTA "Inserindo o valor %d, agora temos %d numeros\n" RESET, item, buffer.idx +1);
    if(buffer.idx == N && !turn) swap_time();
    pthread_mutex_unlock(&mutex_lock);
}

void consumir(){
    pthread_mutex_lock(&mutex_lock);
    int item;
    item = buffer.v[buffer.idx--];
    printf(CYAN "Consumi o valor %d, agora temos %d numeros\n" RESET, item, buffer.idx + 1);
    if(buffer.idx == -1 && turn) swap_time();
    pthread_mutex_unlock(&mutex_lock);
}

void *produtor(){
    while(1){
        sleep(rand()%4 + prod_sleep);
        sem_wait(&vazio); 
        inserir();
        sem_post(&consumo); 
    }
}



void *consumidor(){
    while(1){
        sleep(rand()%4 + consu_sleep);
        sem_wait(&consumo); 
        consumir();
        sem_post(&vazio); 
    }
}



int main(){ 

    buffer.idx = -1;
    srand(time(0));
    sem_init(&vazio, 0, VECLEN);
    sem_init(&consumo, 0, 0); 
    pthread_create(&threads[0], NULL, consumidor, NULL);
    pthread_create(&threads[1], NULL, produtor, NULL);

    for (int i = 0; i < NUMTHRDS; i++) 
        pthread_join(threads[i], NULL);
    

    return 0;
}