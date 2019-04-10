/*  
	Programação Concorrente 2019-1
	Thiago Veras Machado
	16/0146682s
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMTHRDS 20
#define RESET "\x1b[0m"

pthread_t threads[NUMTHRDS];
pthread_mutex_t mutex_direita, mutex_esquerda, mutex_turno, mutex_bridge;
int L,R;

struct thread_data{
   int thread_id;
   int side;
};

struct thread_data thread_data_array[NUMTHRDS];

char* color[6] = {
    "\x1b[31m", "\x1b[32m", "\x1b[33m",
    "\x1b[34m", "\x1b[35m", "\x1b[36m"
};



void *macaco(void *threadarg){
    sleep(2);
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;

    while(1){
        if(my_data->side){
            pthread_mutex_lock(&mutex_turno);
            pthread_mutex_lock(&mutex_direita);
            R++;
            if(R == 1){
                pthread_mutex_lock(&mutex_bridge);
            }
            printf("%s O macaco %d ta indo e temos %d macacos indo da direta pra esquerda\n" RESET, color[2], my_data->thread_id, R);
            pthread_mutex_unlock(&mutex_direita);
            pthread_mutex_unlock(&mutex_turno);
            my_data->side = !my_data->side;
            sleep(1);
            pthread_mutex_lock(&mutex_direita);
            R--;
            printf("%s O macaco %d chegou na esquerda, sobrando %d macacos terminarem de passar\n" RESET, color[2], my_data->thread_id, R);
            if(R == 0){
                pthread_mutex_unlock(&mutex_bridge);
            }
            pthread_mutex_unlock(&mutex_direita);

        } else{
            pthread_mutex_lock(&mutex_turno);
            pthread_mutex_lock(&mutex_esquerda);
            L++;
            if(L == 1){
                pthread_mutex_lock(&mutex_bridge);
            }
            printf("%s O macaco %d ta indo e temos %d macacos indo da esquerda pra direta\n" RESET, color[4], my_data->thread_id, L);
            pthread_mutex_unlock(&mutex_esquerda);
            pthread_mutex_unlock(&mutex_turno);
            my_data->side = !my_data->side;
            sleep(1);
            pthread_mutex_lock(&mutex_esquerda);
            L--;
            printf("%s O macaco %d chegou na direita, sobrando %d macacos terminarem de passar\n" RESET, color[4], my_data->thread_id, L);
            if(L == 0){
                pthread_mutex_unlock(&mutex_bridge);
            }
            pthread_mutex_unlock(&mutex_esquerda);
        }
    }
}


int main(){ 

    pthread_mutex_init(&mutex_direita, NULL);
    pthread_mutex_init(&mutex_esquerda, NULL);
    pthread_mutex_init(&mutex_turno, NULL);
    pthread_mutex_init(&mutex_bridge, NULL);

    

    for(long i = 0; i < 10; i++){
        thread_data_array[(int)i].thread_id = i;
        thread_data_array[(int)i].side = 1;
        pthread_create(&threads[(int)i], NULL, macaco, (void *) &thread_data_array[(int)i]);
    }
    for(long i = 10; i < 20; i++){
        thread_data_array[(int)i].thread_id = i;
        thread_data_array[(int)i].side = 0;
        pthread_create(&threads[(int)i], NULL, macaco, (void *) &thread_data_array[(int)i]);
    }

    for (int i = 0; i < NUMTHRDS; i++) 
        pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&mutex_direita);
    pthread_mutex_destroy(&mutex_esquerda);
    pthread_mutex_destroy(&mutex_turno);
    pthread_mutex_destroy(&mutex_bridge);

    pthread_exit (NULL);

    return 0;
}