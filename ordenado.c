/*
    Estudo Dirigido 13
    Thiago Veras Machado
    16/0146682
*/



#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define NUMTHRDS 2
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

pthread_t threads[NUMTHRDS];
pthread_barrier_t barrier;


void * a_c(){
    printf(YELLOW "AAAAAAA\n" RESET);
    pthread_barrier_wait(&barrier);
    pthread_barrier_wait(&barrier);
    printf(RED "CCCCCCCC\n" RESET);
    pthread_barrier_wait(&barrier);
}

void * b_d(){
    pthread_barrier_wait(&barrier);
    printf(BLUE "BBBBBBBB\n" RESET);
    pthread_barrier_wait(&barrier);
    pthread_barrier_wait(&barrier);
    printf(GREEN "DDDDDDDD\n" RESET);
}


int main(){ 
    
    pthread_barrier_init(&barrier, NULL, NUMTHRDS);

    pthread_create(&threads[0], NULL, a_c, NULL);
    pthread_create(&threads[1], NULL, b_d, NULL);


    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    

    return 0;
}