/*
    Estudo Dirigido 4
    Thiago Veras Machado
    16/0146682
*/



#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define NUMTHRDS 2
#define VECLEN 4
#define N 3
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

int database[4] = {10, 20, 30, 40};
pthread_t threads[NUMTHRDS];
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_buffer = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ok_to_prod = PTHREAD_COND_INITIALIZER;
pthread_cond_t ok_to_consu = PTHREAD_COND_INITIALIZER;
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
    pthread_mutex_unlock(&mutex_lock);
}

int consumir(){
    pthread_mutex_lock(&mutex_lock);
    int item;
    item = buffer.v[buffer.idx--];
    printf(CYAN "Consumi o valor %d, agora temos %d numeros\n" RESET, item, buffer.idx + 1);
    pthread_mutex_unlock(&mutex_lock);
    return item;
}


bool ok_consu(){    
    return buffer.idx >= 0;
}

bool ok_prod(){
    return buffer.idx < N;
}

void *produtor(){
    while(1){
        sleep(rand()%4 + prod_sleep);
        pthread_mutex_lock(&mutex_lock);
        while(buffer.idx == N){
            printf(YELLOW "To cheio e n consigo produzir, vou ir dormir\n");
            if(!turn) swap_time();
            pthread_cond_wait(&ok_to_prod, &mutex_lock);
            printf(YELLOW "Acordei e vou voltar a produzir\n");
        }
        pthread_mutex_unlock(&mutex_lock);
        
        
        inserir();

        pthread_mutex_lock(&mutex_lock);
        if(ok_consu()){
            pthread_cond_signal(&ok_to_consu);
        }
        pthread_mutex_unlock(&mutex_lock);

    }

    pthread_exit(NULL);
}



void *consumidor(){
    while(1){
        sleep(rand()%4 + consu_sleep);
        pthread_mutex_lock(&mutex_lock);
        while(buffer.idx == -1){
            printf(RED "To vazio e n consigo consumir, vou ir dormir\n");
            if(turn) swap_time();
            pthread_cond_wait(&ok_to_consu, &mutex_lock);
            printf(RED "Acordei e vou voltar a consumir\n");

        }
        pthread_mutex_unlock(&mutex_lock);

        int num = consumir();
    
        pthread_mutex_lock(&mutex_lock);
        if(ok_prod()){
            pthread_cond_signal(&ok_to_prod);
        }
        pthread_mutex_unlock(&mutex_lock);

    }

    

    pthread_exit(NULL);

}



int main(){ 

    buffer.idx = -1;

    pthread_create(&threads[0], NULL, consumidor, NULL);
    pthread_create(&threads[1], NULL, produtor, NULL);

    for (int i = 0; i < NUMTHRDS; i++) 
        pthread_join(threads[i], NULL);
    

    return 0;
}