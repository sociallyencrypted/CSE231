#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 5
#define LEFT i
#define RIGHT (i+1) % N
#define FIRST (i%2 == 0) ? LEFT : RIGHT
#define SECOND (i%2 == 0) ? RIGHT : LEFT


struct Philospher
{
    long long eaten;
    pthread_t thread;
};


struct Fork
{
    sem_t semaphore;
    pthread_mutex_t lock;
};


struct SauceBowl
{
    sem_t semaphore;
    pthread_mutex_t lock;
};


struct Philospher makePhilosopher()
{
    pthread_t tid;
    struct Philospher philosopher = { 0ll, tid };
    return philosopher;
}


struct Fork makeFork()
{
    pthread_mutex_t lock;
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("Mutex init failed \n");
        exit(EXIT_FAILURE);
    }

    sem_t semaphore;
    if (sem_init(&semaphore, 0, 1) != 0)
    {
        printf("Semaphore init failed \n");
        exit(EXIT_FAILURE);
    }

    struct Fork fork = { semaphore, lock };
    return fork;
}


struct SauceBowl makeSauceBowl()
{
    pthread_mutex_t lock;
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("Mutex init failed \n");
        exit(EXIT_FAILURE);
    }

    sem_t semaphore;
    if (sem_init(&semaphore, 0, 1) != 0)
    {
        printf("Semaphore init failed \n");
        exit(EXIT_FAILURE);
    }

    struct SauceBowl sauceBowl = { semaphore, lock };
    return sauceBowl;
}