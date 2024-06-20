//to solve this, we will use pthread library and pthread mutex
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t agent_sem;
sem_t tobacco_sem;        
sem_t paper_sem;      
sem_t matches_sem;       
sem_t table_sem;         

typedef enum { TOBACCO, PAPER, MATCHES } Ingredient;

void *agent(void *arg);
void *smoker(void *arg);
void place_items_on_table(Ingredient item1, Ingredient item2);

int main() {
    pthread_t agent_thread;
    pthread_t smoker_threads[3];
    int smoker_ids[3] = { TOBACCO, PAPER, MATCHES };

   
    sem_init(&agent_sem, 0, 1);
    sem_init(&tobacco_sem, 0, 0);
    sem_init(&paper_sem, 0, 0);
    sem_init(&matches_sem, 0, 0);
    sem_init(&table_sem, 0, 1);

    pthread_create(&agent_thread, NULL, agent, NULL);

    for (int i = 0; i < 3; i++) {
        pthread_create(&smoker_threads[i], NULL, smoker, &smoker_ids[i]);
    }

    pthread_join(agent_thread, NULL);
    for (int i = 0; i < 3; i++) {
        pthread_join(smoker_threads[i], NULL);
    }

    sem_destroy(&agent_sem);
    sem_destroy(&tobacco_sem);
    sem_destroy(&paper_sem);
    sem_destroy(&matches_sem);
    sem_destroy(&table_sem);

    return 0;
}

void *agent(void *arg) {
    Ingredient items[3] = { TOBACCO, PAPER, MATCHES };
    while (1) {
        sem_wait(&agent_sem); // Wait for the table to be clear
        int item1 = rand() % 3;
        int item2 = (item1 + 1 + rand() % 2) % 3;
        place_items_on_table(items[item1], items[item2]);
    }
}

void *smoker(void *arg) {
    int item = *((int *)arg);
    int smoker_num = item + 1; 
    while (1) {
        if (item == TOBACCO) {
            sem_wait(&tobacco_sem); 
            printf("Smoker %d with tobacco takes paper and matches and makes a cigarette.\n", smoker_num);
        } else if (item == PAPER) {
            sem_wait(&paper_sem); 
            printf("Smoker %d with paper takes tobacco and matches and makes a cigarette.\n", smoker_num);
        } else if (item == MATCHES) {
            sem_wait(&matches_sem);
            printf("Smoker %d with matches takes tobacco and paper and makes a cigarette.\n", smoker_num);
        }
        sleep(1); 
        printf("Smoker %d has finished smoking.\n", smoker_num);
        sem_post(&agent_sem);
    }
}

void place_items_on_table(Ingredient item1, Ingredient item2) {
    sem_wait(&table_sem); 
    printf("Agent places %s and %s on the table.\n", item1 == TOBACCO ? "tobacco" : item1 == PAPER ? "paper" : "matches",
                                                      item2 == TOBACCO ? "tobacco" : item2 == PAPER ? "paper" : "matches");
    if ((item1 == TOBACCO && item2 == PAPER) || (item1 == PAPER && item2 == TOBACCO)) {
        sem_post(&matches_sem); 
    } else if ((item1 == TOBACCO && item2 == MATCHES) || (item1 == MATCHES && item2 == TOBACCO)) {
        sem_post(&paper_sem);
    } else if ((item1 == PAPER && item2 == MATCHES) || (item1 == MATCHES && item2 == PAPER)) {
        sem_post(&tobacco_sem); 
    }
    sem_post(&table_sem); 
}
