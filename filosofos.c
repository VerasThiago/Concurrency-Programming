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

pthread_t threads[NUMTHRDS];
pthread_mutex_t mutex_fork[5];

char* color[6] = {
    "\x1b[31m", "\x1b[32m", "\x1b[33m",
    "\x1b[34m", "\x1b[35m", "\x1b[36m"
};

void swap(int * a, int *b){
    int c = *a;
    *a = *b;
    *b = c;
}

void * philosophers(void * _id){
    int id = (int)(long)_id;
    int left = id;
    int right = (id+1)%5;
    if(id == 4) swap(&left,&right);
    while(true){
        sleep(rand()%7 + 2);
        int left_fork = pthread_mutex_trylock(&mutex_fork[left]); 
        if(left_fork != 0){
            if(id == 4) printf("%s Philosopher %d can't get right fork\n" RESET,color[id], id);
            else printf("%s Philosopher %d can't get left fork\n" RESET,color[id], id);
            //sleep(1);
        } else{
            int right_fork = pthread_mutex_trylock(&mutex_fork[right]); 
            if(right_fork != 0){
                // Already been used
                if(id == 4) printf("%s Philosopher %d can't get left fork\n" RESET,color[id], id);
                else printf("%s Philosopher %d can't get right fork\n" RESET,color[id], id);
                //sleep(1);
                pthread_mutex_unlock(&mutex_fork[left]);
            } else{
                printf("%s Philosopher %d is eating\n" RESET,color[id], id);
                sleep(rand()%10 + 2);
                printf("%s Philosopher %d FINISHED eating\n" RESET,color[id], id);
                pthread_mutex_unlock(&mutex_fork[left]);
                pthread_mutex_unlock(&mutex_fork[right]);
            }
        }
    }

}


int main(){ 

    for(int i = 0; i < 5; i++) pthread_mutex_init(&mutex_fork[i], NULL);

    
    for(int i = 0; i < NUMTHRDS; i++)
        pthread_create(&threads[i], NULL, philosophers, (void * )(long)i);

    for(int i = 0; i < NUMTHRDS  - 1; i++)
        pthread_join(threads[i], NULL); 

    return 0;
}