//
// Created by student on 6/4/20.
//

#include "Barrier.h"

Barrier::Barrier(unsigned int num_of_threads) :
                            threads_num(num_of_threads), cnt(0) {
    //if num of threads for the berrier is 0 we don't really need the berrier:
    if(!num_of_threads) delete this; ///CHECK
    //initialize the semaphones:

    //this semaphones we make a list of the threads:
    int check = sem_init(&sem1, 0, 0);
    if (check == -1) delete this;

    check = sem_init(&sem2, 0, 0);
    if (check == -1) delete this;
}

void Barrier::wait() {
    pthread_mutex_lock(&lock);
    cnt++;
    if (cnt == threads_num){
        for (int i = 0; i < (int)threads_num ; ++i) {
            sem_post(&sem1);
        }
    }
    pthread_mutex_unlock(&lock);
    sem_wait(&sem1);

    //decrease the count for reusability:
    pthread_mutex_lock(&lock);
    cnt--;
    if (!cnt){
        for (int i = 0; i < (int)threads_num ; ++i) {
            sem_post(&sem2);
        }
    }
    pthread_mutex_unlock(&lock);
    sem_wait(&sem2);
}

Barrier::~Barrier() {
    pthread_mutex_destroy(&lock);
    sem_destroy(&sem1);
    sem_destroy(&sem2);
}

