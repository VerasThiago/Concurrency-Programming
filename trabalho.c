#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <semaphore.h>


#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"



pthread_mutex_t linf = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t enche = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_veterano_entrar = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_veterano_sair = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_calouro_entrar = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_calouro_sair = PTHREAD_COND_INITIALIZER;
sem_t AAAAAA, evacuar_veterano;


int calouro_entrar = 0, calouro_sair = 0, veterano_entrar = 0, veterano_sair = 0, enchente = 0;
int vagas = 30;
int estado_veterano[200];
int estado_calouro[200];
int qnt_veterano = 0, qnt_calouro = 0;

void evacuar_todos(){
    sem_post(&AAAAAA);
    sem_post(&evacuar_veterano);
}

void entrar_veterano(int id){
    pthread_mutex_lock(&linf);

    veterano_entrar ++;
    while(vagas == 0){
        pthread_cond_wait(&cond_veterano_entrar, &linf);
    }
    veterano_entrar --;
    estado_veterano[id] = 1;
    vagas--;
    qnt_veterano++;
    pthread_mutex_unlock(&linf);

    printf(RED "Veterano %d entrou no linf e ficou com %d veterano\n"RESET, id, qnt_veterano);
    sleep(5);

    pthread_mutex_lock(&linf);

    while(enchente){
        pthread_cond_wait(&cond_veterano_sair, &linf);
    } 

    if(estado_veterano[id] == 0){ // Caso o aluno estava no linf e ja foi evacuado
        printf("ALOOOOOOOOOOOOOOU veterano %d ja tinha saido aqui\n", id);
        pthread_mutex_unlock(&linf);
        return;
    }
    vagas++;
    qnt_veterano--;
    estado_veterano[id] = 0;
    printf(GREEN "Veterano %d saiu do linf e ficou com %d veterano\n"RESET, id, qnt_veterano);
    pthread_cond_broadcast(&cond_veterano_entrar); 
    pthread_cond_broadcast(&cond_calouro_entrar); 

    pthread_mutex_unlock(&linf);
}

void entrar_calouro(int id){
    pthread_mutex_lock(&linf);

    calouro_entrar ++;
    while(vagas == 0 || veterano_entrar){
        pthread_cond_wait(&cond_calouro_entrar, &linf);
    }
    calouro_entrar --;
    estado_calouro[id] = 1;
    vagas--;
    qnt_calouro++;
    
    pthread_mutex_unlock(&linf);

    printf(YELLOW "\t\t\t\t\t\t\tCalouro %d entrou no linf e ficou com %d calouro\n"RESET, id, qnt_calouro);
    sleep(3);

    pthread_mutex_lock(&linf);

    while(enchente){
        pthread_cond_wait(&cond_veterano_sair, &linf);
    } 

    if(estado_calouro[id] == 0){ // Caso o aluno estava no linf e ja foi evacuado
        printf("ALOOOOOOOOOOOOOOU2222 calouro %d ja tinha saido aqui\n", id);
        pthread_mutex_unlock(&linf);
        return;
    }
    vagas++;
    qnt_calouro--;
    estado_calouro[id] = 0;
    printf(BLUE "\t\t\t\t\t\t\tCalouro %d saiu do linf e ficou com %d calouro\n"RESET, id, qnt_calouro);
    pthread_cond_broadcast(&cond_veterano_entrar); 
    pthread_cond_broadcast(&cond_calouro_entrar); 

    pthread_mutex_unlock(&linf);
}

void * esvaziar_calouro(){
    while(true){
        sem_wait(&AAAAAA);
        for(int id = 30; id < 60; id++){
            pthread_mutex_lock(&linf);
            if(enchente == 0){
                pthread_cond_signal(&cond_veterano_sair);
                pthread_mutex_unlock(&linf);
                break;    
            }
            if(estado_calouro[id] == 1){
                vagas++;
                estado_calouro[id] = 0;
                qnt_calouro--;
                printf(MAGENTA "\t\t\t\t\t\t\tCalouro %d saiu CORRENDO %d calouros\n"RESET, id, qnt_calouro);
                sleep(1);
            }
            pthread_mutex_unlock(&linf);
        }
        pthread_cond_broadcast(&cond_veterano_sair);
    }
}

void * esvaziar_veterano(){
    while(true){
        sem_wait(&evacuar_veterano);
        pthread_mutex_lock(&linf);
        pthread_cond_wait(&cond_veterano_sair, &linf);
        pthread_mutex_unlock(&linf);


        for(int id = 0; id < 30; id++){
            pthread_mutex_lock(&linf);
            if(enchente == 0){
                pthread_mutex_unlock(&linf);
                break;    
            }
            if(estado_veterano[id] == 1){
                vagas++;
                qnt_veterano--;
                estado_veterano[id] = 0;
                printf(CYAN "Veterano %d saiu CORRENDO %d veteranos\n"RESET, id, qnt_veterano);
                sleep(1);
            }
            pthread_mutex_unlock(&linf);
        }
    }
}


void * veteranos(void * _id){
    int id = (int)(long)_id;
    int alagou;
    while (true){
        sleep(rand()%10 + 1);
        pthread_mutex_lock(&linf);
            while(enchente){
                pthread_cond_wait(&cond_veterano_entrar, &linf);
            } 
        pthread_mutex_unlock(&linf);
        sleep(1);
        entrar_veterano(id);
    }
}

void * calouros(void * _id){
    int id = (int)(long)_id;
    while (true){
        sleep(rand()%8 + 1);
        pthread_mutex_lock(&linf);
            while(enchente){
                pthread_cond_wait(&cond_calouro_entrar, &linf);
            } 
        pthread_mutex_unlock(&linf);
        sleep(1);
        entrar_calouro(id);
    }
}

void * alagar(){
    while(true){
        sleep(rand()%10 + 3);
        pthread_mutex_lock(&linf);
        enchente = 1;
        printf(CYAN"\t\t\t\t\t\t\t\t\t\t\t\t\t\tENCHENTEEEEEEEEEEEEEEEEEEEEEEEEE \n" RESET);
        evacuar_todos();
        sleep(2);
        pthread_mutex_unlock(&linf);

        sleep(rand()%20 + 20);

        pthread_mutex_lock(&linf);
        enchente = 0;
        printf(CYAN"\t\t\t\t\t\t\t\t\t\t\t\t\t\tSECOOOOOOOOOOOOOOOOOOOOOOU \n" RESET);
        pthread_cond_broadcast(&cond_veterano_entrar);
        pthread_cond_broadcast(&cond_calouro_entrar);
        pthread_mutex_unlock(&linf);

    }

}


int main(){ 
    pthread_t threads[63];
    memset(estado_veterano, 0, sizeof(estado_veterano));
    memset(estado_calouro, 0, sizeof(estado_calouro));
    sem_init(&AAAAAA,0,0);
    sem_init(&evacuar_veterano,0,0);

    for(int i = 0; i < 30; i++)
        pthread_create(&threads[i], NULL, veteranos, (void * )(long)i);
    for(int i = 30; i < 60; i++)
        pthread_create(&threads[i], NULL, calouros, (void * )(long)i);
    
    pthread_create(&threads[60], NULL, alagar, NULL);
    pthread_create(&threads[61], NULL, esvaziar_calouro, NULL);
    pthread_create(&threads[62], NULL, esvaziar_veterano, NULL);

    for(int i = 0; i < 63; i++)
        pthread_join(threads[i], NULL); 

    return 0;
}