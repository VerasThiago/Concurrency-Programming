/*
    Estudo Dirigido 11 - Filósofos
    Thiago Veras Machado
    16/0146682
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h> 

#define NUMTHRDS 5
#define BAGSIZE 20
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"
#define THINK 0
#define HUNGRY 1
#define EAT 2

pthread_t threads[NUMTHRDS];
sem_t philosopher[5];
pthread_mutex_t table;
int state[3];

char* color[6] = {
    "\x1b[31m", "\x1b[32m", "\x1b[33m",
    "\x1b[34m", "\x1b[35m", "\x1b[36m"
};

void try_eat(int id){
    int left = (id-1+5)%5 ;
    int right = (id+1)%5;
    if(state[id] == HUNGRY && state[left] != EAT && state[right] != EAT){
        state[id] = EAT;
        printf("%s Philosopher %d is eating\n" RESET,color[id], id);
        sleep(rand()%7 + 2);
        sem_post(&philosopher[id]);
    }
}

void * philosophers(void * _id){
    int id = (int)(long)_id;
    int left = (id-1+5)%5 ;
    int right = (id+1)%5;
    while(true){
        sleep(rand()%7 + 2);

        pthread_mutex_lock(&table);

        state[id] = 1; // Ta pensando
        printf("%s Philosopher %d is hungry!\n" RESET,color[id], id);
        try_eat(id);
        
        pthread_mutex_unlock(&table);
        
        sem_wait(&philosopher[id]);
        sleep(1);

        pthread_mutex_lock(&table);

        state[id] = 0;
        printf("%s Philosopher %d FINISHED eating\n" RESET,color[id], id);
        try_eat(left);
        try_eat(right);

        pthread_mutex_unlock(&table);

   }


}


int main(){ 

    for(int i = 0; i < 5; i++) sem_init(&philosopher[i],0,0);
    
    for(int i = 0; i < NUMTHRDS; i++)
        pthread_create(&threads[i], NULL, philosophers, (void * )(long)i);

    for(int i = 0; i < NUMTHRDS  - 1; i++)
        pthread_join(threads[i], NULL); 

    return 0;
}