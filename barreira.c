/*
    Estudo Dirigido 12 - Barreira
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
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

pthread_t threads[NUMTHRDS];
pthread_barrier_t barrier;

int matrix[10][200];
int soma_da_linha[10];
int qnt = 0;

void * soma_linha(void * _id){

    int id = (int)(long)_id;
    printf(YELLOW"Somando linha %d\n", id);
    int i,j;
    soma_da_linha[id] = 0;
    for(j = 0; j < 200; j ++){
        soma_da_linha[id] += matrix[id][j];
    }
    pthread_barrier_wait(&barrier);
    
    if(id == 0){
        int soma_total = 0;
        for(i = 0; i < NUMTHRDS; i++){
            soma_total += soma_da_linha[i];
        }
        printf(GREEN"Soma final = %d\n"RESET, soma_total);
    }

   
}


int main(){ 
    srand(time(NULL));
    int i,j;
    for(i = 0; i < NUMTHRDS; i++){
        for(j = 0; j < 200; j++){
            matrix[i][j] = rand()%1000;
        }
    }
    
    pthread_barrier_init(&barrier, NULL, NUMTHRDS);

    for(i = 0; i < NUMTHRDS; i++)
        pthread_create(&threads[(int)i], NULL, soma_linha, (void *)(long)i);


    for (i = 0; i < NUMTHRDS; i++) 
        pthread_join(threads[i], NULL);
    

    return 0;
}