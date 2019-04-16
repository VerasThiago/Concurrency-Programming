/*
    Estudo Dirigido 5
    Thiago Veras Machado
    16/0146682
*/



#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define NUMTHRDS 5
#define VECLEN 4
#define N 100
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

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
    pthread_mutex_lock(&mutex_buffer);
    int item = rand()%500;
    buffer.idx++;
    buffer.v[buffer.idx] = item;
    pthread_mutex_unlock(&mutex_buffer);
}

int consumir(int idx){
    pthread_mutex_lock(&mutex_buffer);
    int item;
    item = buffer.v[buffer.idx--];
    printf("%s Canibal %d consumiu o valor %d, agora temos %d numeros\n" RESET, color[idx], idx, item, buffer.idx + 1);
    pthread_mutex_unlock(&mutex_buffer);
    return item;
}


bool ok_consu(){    
    return buffer.idx >= 0;
}

bool ok_prod(){
    return buffer.idx == -1;
}

void *produtor(){

    while(1){   
        sleep(rand()%4 + prod_sleep);
        pthread_mutex_lock(&mutex_lock);
        
        int x = rand()%40 + 20;
        for(int i = 0; i < x; i++)
            inserir();
        printf(MAGENTA "Produzi %d porções\n" RESET, buffer.idx +1);
        while(buffer.idx >= 1){
            printf(YELLOW "Cozinheiro indo dormir\n" RESET);
            if(!turn) swap_time();
            pthread_cond_wait(&ok_to_prod, &mutex_lock);
            printf(YELLOW "Acordei e vou voltar a produzir\n" RESET);
        }
        pthread_mutex_unlock(&mutex_lock);

        pthread_mutex_lock(&mutex_lock);
        if(ok_consu()){
            pthread_cond_signal(&ok_to_consu);
        }
        pthread_mutex_unlock(&mutex_lock);

    }

    pthread_exit(NULL);
}



void *consumidor(void * data){
    int id = (int)(long) data;
    while(1){
        sleep(rand()%4 + consu_sleep);
        pthread_mutex_lock(&mutex_lock);
        while(buffer.idx == -1){
            printf("%s Canibal %d nao consegue consumir, vou ir dormir\n" RESET, color[id], id);
            if(turn) swap_time();
            pthread_cond_wait(&ok_to_consu, &mutex_lock);
            printf("%s Canibal %d acordou e vou voltar a consumir\n" RESET, color[id], id);

        }
        pthread_mutex_unlock(&mutex_lock);
        int num = consumir(id);
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
    for(long i = 0; i < NUMTHRDS -1; i++)
        pthread_create(&threads[(int)i], NULL, consumidor, (void *)i);

    pthread_create(&threads[NUMTHRDS - 1], NULL, produtor, NULL);

    for (int i = 0; i < NUMTHRDS; i++) 
        pthread_join(threads[i], NULL);
    

    return 0;
}