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
pthread_cond_t cond_veterano_entrar = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_veterano_sair = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_calouro_sair = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_calouro_entrar = PTHREAD_COND_INITIALIZER;
pthread_cond_t evacuar = PTHREAD_COND_INITIALIZER;
pthread_cond_t fim_evacuar = PTHREAD_COND_INITIALIZER;


int veterano_entrar = 0, enchente = 0, espera = 9;
int vagas = 20;
int estado_veterano[60];
int estado_calouro[60];

void entrar_veterano(int id){

    pthread_mutex_lock(&linf);

    veterano_entrar ++;
    while(vagas == 0 || enchente == 1){
        pthread_cond_wait(&cond_veterano_entrar, &linf);
    }
    veterano_entrar --;
    estado_veterano[id] = 1;
    vagas--;

    if(vagas == 0){
        espera = 15;
    }

    pthread_mutex_unlock(&linf);

    printf(RED "Veterano %d entrou no linf e ficou com %d vagas\n"RESET, id, vagas);
    sleep(rand()%4 + 5);

    pthread_mutex_lock(&linf);

    while(enchente == 1){
        pthread_cond_wait(&fim_evacuar, &linf);
    } 

    if(estado_veterano[id] == 0 ){ 
        pthread_mutex_unlock(&linf);
        return;
    }
    vagas++;

    if(vagas == 20){
        espera = 9;
    }

    estado_veterano[id] = 0;
    printf(GREEN "Veterano %d saiu do linf e ficou com %d vagas\n"RESET, id, vagas);
    
    pthread_cond_broadcast(&cond_veterano_entrar); 
    pthread_cond_broadcast(&cond_calouro_entrar); 

    pthread_mutex_unlock(&linf);
}

void entrar_calouro(int id){
    pthread_mutex_lock(&linf);

    while(vagas == 0 || veterano_entrar || enchente == 1){
        pthread_cond_wait(&cond_calouro_entrar, &linf);
    }
    estado_calouro[id] = 1;
    vagas--;

    if(vagas == 0){
        espera = 15;
    }
    
    pthread_mutex_unlock(&linf);

    printf(YELLOW "\t\t\t\t\t\t\tCalouro %d entrou no linf e ficou com %d vagas\n"RESET, id, vagas);
    sleep(rand()%4 + 3);

    pthread_mutex_lock(&linf);

    while(enchente == 1){
        pthread_cond_wait(&fim_evacuar, &linf);
    } 

    if(estado_calouro[id] == 0){ 
        pthread_mutex_unlock(&linf);
        return;
    }
    vagas++;
    estado_calouro[id] = 0;

    if(vagas == 20){
        espera = 9;
    }

    printf(BLUE "\t\t\t\t\t\t\tCalouro %d saiu do linf e ficou com %d vagas\n"RESET, id, vagas);
    pthread_cond_broadcast(&cond_veterano_entrar); 
    pthread_cond_broadcast(&cond_calouro_entrar); 

    pthread_mutex_unlock(&linf);
}

void * esvaziar_calouro(void * _id){
    int id = (int)(long)_id;
    while(true){
        sleep(rand()%3 + 2);
        pthread_mutex_lock(&linf);
        pthread_cond_wait(&evacuar, &linf);
        if(enchente == 0 || estado_calouro[id] == 0){
            pthread_mutex_unlock(&linf);
            continue;    
        }
        vagas++;
        estado_calouro[id] = 0;
        printf(MAGENTA "\t\t\t\t\t\t\tCalouro %d saiu CORRENDO %d vagas\n"RESET, id, vagas);
        sleep(1);
        if(vagas == 20){
            pthread_cond_signal(&fim_evacuar);
        }
        pthread_mutex_unlock(&linf);
    }
}

void * esvaziar_veterano(void * _id){
    int id = (int)(long)_id;
    while(true){
        sleep(rand()%3 + 2);
        pthread_mutex_lock(&linf);
        pthread_cond_wait(&evacuar, &linf);
        if(enchente == 0 || estado_veterano[id] == 0){
            pthread_mutex_unlock(&linf);
            continue;    
        }
        vagas++;
        estado_veterano[id] = 0;
        printf(CYAN "Veterano %d saiu CORRENDO %d vagas\n"RESET, id, vagas);
        sleep(1);
        if(vagas == 20){
            pthread_cond_signal(&fim_evacuar);
        }
        pthread_mutex_unlock(&linf);
       
    }
}


void * veteranos(void * _id){
    int id = (int)(long)_id;
    while (true){
        sleep(rand()%espera + 1);
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
        sleep(rand()%espera + 1);
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
        sleep(rand()%10 + 10);
        pthread_mutex_lock(&linf);

        enchente = 1;

        printf(RED"\t\t\t\t _______ .__   __.   ______  __    __   _______ .__   __. .___________._______  __   __   __                         __\n");
        printf("\t\t\t\t|   ____||  \\ |  |  /      ||  |  |  | |   ____||  \\ |  | |           |   ____||  | |  | |  |                       /' )\n"); 
        printf("\t\t\t\t|  |__   |   \\|  | |  ,----'|  |__|  | |  |__   |   \\|  | `---|  |----|  |__   |  | |  | |  |                      /'   (                          ,\n"); 
        printf("\t\t\t\t|   __|  |  . `  | |  |     |   __   | |   __|  |  . `  |     |  |    |   __|  |  | |  | |  |                   __/'     )                        .' `;\n"); 
        printf("\t\t\t\t|  |____ |  |\\   | |  `----.|  |  |  | |  |____ |  |\\   |     |  |    |  |____ |__| |__| |__|           _.-~~~~'          ``---..__             .'   ;\n"); 
        printf("\t\t\t\t|_______||__| \\__|  \\______||__|  |__| |_______||__| \\__|     |__|    |_______|(__) (__) (__)      _.--'   b)                      ``--...____.'   .'\n"); 
        printf("\t\t\t\t                                                                                                  (     _.      )).      `-._                     <\n");
        printf("\t\t\t\t                                                                                                   `vvvvvvv-)-.....___.-     `-.         __...--'-.'.\n");
        printf("\t\t\t\t                                                                                                     `^^^^^'-------.....`-.___.'----... .'         `.;\n");
        printf("\t\t\t\t                                                                                                                                       `-`           `   \n"RESET);                                                                                          
        
        pthread_cond_broadcast(&evacuar);
        pthread_cond_wait(&fim_evacuar, &linf);
        
        enchente = 0;

        printf(GREEN"\t\t\t\t __       __  .__   __.  _______     ___________    ____  ___       ______  __    __       ___       _______   ______\n"); 
        printf("\t\t\t\t|  |     |  | |  \\ |  | |   ____|   |   ____\\   \\  /   / /   \\     /      ||  |  |  |     /   \\     |       \\ /  __  \\  \n");
        printf("\t\t\t\t|  |     |  | |   \\|  | |  |__      |  |__   \\   \\/   / /  ^  \\   |  ,----'|  |  |  |    /  ^  \\    |  .--.  |  |  |  | \n");
        printf("\t\t\t\t|  |     |  | |  . `  | |   __|     |   __|   \\      / /  /_\\  \\  |  |     |  |  |  |   /  /_\\  \\   |  |  |  |  |  |  | \n");
        printf("\t\t\t\t|  `----.|  | |  |\\   | |  |        |  |____   \\    / /  _____  \\ |  `----.|  `--'  |  /  _____  \\  |  '--'  |  `--'  | \n");
        printf("\t\t\t\t|_______||__| |__| \\__| |__|        |_______|   \\__/ /__/     \\__\\ \\______| \\______/  /__/     \\__\\ |_______/ \\______/  \n"RESET);
        sleep(rand()%3 + 3);

        pthread_cond_broadcast(&cond_veterano_entrar);
        pthread_cond_broadcast(&cond_calouro_entrar);

        pthread_mutex_unlock(&linf);
    }

}


int main(){ 

    pthread_t threads[63];

    memset(estado_veterano, 0, sizeof(estado_veterano));
    memset(estado_calouro, 0, sizeof(estado_calouro));

    for(int i = 0; i < 15; i++)
        pthread_create(&threads[i], NULL, veteranos, (void * )(long)i);
    for(int i = 15; i < 30; i++)
        pthread_create(&threads[i], NULL, esvaziar_veterano, (void * )(long)i-15);
    for(int i = 30; i < 45; i++)
        pthread_create(&threads[i], NULL, calouros, (void * )(long)i);
    for(int i = 45; i < 60; i++)
        pthread_create(&threads[i], NULL, esvaziar_calouro, (void * )(long)i-15);
    
    pthread_create(&threads[60], NULL, alagar, NULL);
    pthread_create(&threads[61], NULL, esvaziar_calouro, NULL);
    pthread_create(&threads[62], NULL, esvaziar_veterano, NULL);

    for(int i = 0; i < 63; i++)
        pthread_join(threads[i], NULL); 

    return 0;
}