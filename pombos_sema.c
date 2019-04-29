/*
    Estudo Dirigido 9 - Pombos semáforo
    Thiago Veras Machado
    16/0146682
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h> 

#define NUMTHRDS 4
#define BAGSIZE 20
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

pthread_t threads[NUMTHRDS];
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;
sem_t mochila_pombo, viagem_pombo; 
int mochila;



char* color[6] = {
    "\x1b[31m", "\x1b[32m", "\x1b[33m",
    "\x1b[34m", "\x1b[35m", "\x1b[36m"
};

void write_bag(int id){
    pthread_mutex_lock(&mutex_lock);
    mochila++;
    printf(BLUE "Usuário %d escreveu uma mensagem e agora temos %d mensagens\n", id, mochila);
    if(mochila == BAGSIZE){
        printf(CYAN "Usuário %d mandou o pombo ir entregar tudo\n" RESET, id);
        sem_post(&viagem_pombo);
    }
    pthread_mutex_unlock(&mutex_lock);
}

void free_bag(){
    pthread_mutex_lock(&mutex_lock);
    mochila = 0;
    for(int i = 0; i < BAGSIZE; i++)
        sem_post(&mochila_pombo);
    pthread_mutex_unlock(&mutex_lock);
}
void failed(int id){
    printf(RED "Usuario %d não conseguiu escrever pois o pombo ta viajando\n", id);
}

void * pombo(){
    while(1){
        sem_wait(&viagem_pombo);
        printf(GREEN "Pombo indo de A para B enviar as mensagens\n" RESET);
        sleep(3);
        printf(MAGENTA "Pombo voltando de B para A com a mochila vazia\n" RESET);

        free_bag();
        
    }
}

void * usuario(void * thread_id){
    int id = (int)(long)thread_id;
    while(1){
        sleep(1);
        int try_wait = sem_trywait(&mochila_pombo);
        if(try_wait == 0){
            write_bag(id);
        }
        else{
            failed(id);
        }
    }
}

int main(){ 

    sem_init(&mochila_pombo, 0, BAGSIZE);
    sem_init(&viagem_pombo, 0, 0);
    pthread_create(&threads[0], NULL, pombo, NULL);
    for(int i = 1; i < NUMTHRDS; i++)
        pthread_create(&threads[i], NULL, usuario, (void * )(long)i);

    for(int i = 0; i < NUMTHRDS  - 1; i++)
        pthread_join(threads[i], NULL); 

    return 0;
}