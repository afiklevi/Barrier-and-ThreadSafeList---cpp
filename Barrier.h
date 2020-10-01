#ifndef BARRIER_H_
#define BARRIER_H_

#include <semaphore.h>
#include <pthread.h>

class Barrier {
    unsigned int threads_num;
    unsigned int cnt;
    pthread_mutex_t lock;
    sem_t sem1;
    sem_t sem2;
public:
    Barrier(unsigned int num_of_threads);
    void wait();
    ~Barrier();

	// TODO: define the member variables
	// Remember: you can only use semaphores!
};

#endif // BARRIER_H_

