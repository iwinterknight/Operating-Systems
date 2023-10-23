#include "buffer.h"
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>

unsigned int seed = 42;

buffer_item buffer[BUFFER_SIZE];
int sleep_time;
int num_producer_threads;
int num_consumer_threads;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t empty;
sem_t full;
int empty_index = 0;
int full_index = 0;


int insert_item(buffer_item item) {
    /* insert an item into buffer */
    buffer[full_index] = item;
    int insert_index = full_index;
    full_index = (full_index + 1) % BUFFER_SIZE;
    printf("producer produced  %d\n", item);
    /* return 0 if successful, otherwise
       return -1 indicating an error condition */
    if (buffer[insert_index] == item) {
        return 0;
    }
    return -1;
}

int remove_item(buffer_item *item) {
    /* remove an object from buffer and placing it in item*/
    buffer_item removed_item = buffer[empty_index];
    empty_index = (empty_index + 1) % BUFFER_SIZE;
    item = &removed_item;
    printf("consumer consumed %d\n", *item);
    /* return 0 if successful, otherwise
       return -1 indicating an error condition */
    if (item != NULL) {
        return 0;
    }
    return -1;
}

void *producer(void *param) {
    buffer_item rand;
    while (1) {
        sleep(sleep_time);
        rand = rand_r(&seed);
        /* insert item */
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        if (insert_item(rand) < 0) {
            printf("Error inserting %d in buffer.\n", rand);
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

void *consumer(void *param) {
    buffer_item  rand;
    while (1) {
        /* sleep for a random period of time */
        sleep(sleep_time);
        rand = rand_r(&seed);
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        if (remove_item(&rand) < 0) {
            printf("Error removing item from buffer.\n");
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

int main(int argc, char* argv[]) {
    /* 1. Get command line arguments argv[1], argv[2], argv[3] */
    assert (argc == 4); // 1 for program name and 3 for args
    sleep_time = atoi(argv[1]);
    assert(1 <= sleep_time);
    assert(sleep_time <= 10);

    num_producer_threads = atoi(argv[2]);
    assert(1 <= num_producer_threads);
    assert(num_producer_threads <= 5);

    num_consumer_threads = atoi(argv[3]);
    assert(1 <= num_consumer_threads);
    assert(num_consumer_threads <= 5);

    /* 2. Initialize buffer, mutex, semaphores, other global vars */
//    /* Declaring buffer */
//    buffer_item buffer[BUFFER_SIZE];
    /* Declaring & initializing mutex */
    int rc;
    rc = pthread_mutex_init(&mutex, NULL);
    assert (rc == 0);
    /* Declaring & initializing semaphores */
    sem_init(&empty, 0, BUFFER_SIZE);   // BUFFER_SIZE are empty
    sem_init(&full, 0, 0);  // 0 are full

    /* 3. Create producer thread(s) */
    pthread_t p_tid;
    pthread_attr_t p_attr;
    /* get the default attribute */
    pthread_attr_init(&p_attr);
    /* create producer threads */
    for (int index = 0; index < num_producer_threads; index++)
    {
        pthread_create(&p_tid, &p_attr, producer, NULL);
    }

    /* 4. Create consumer thread(s) */
    pthread_t c_tid;
    pthread_attr_t c_attr;
    /* get the default attribute */
    pthread_attr_init(&c_attr);
    /* create consumer threads */
    for (int index = 0; index < num_consumer_threads; index++)
    {
        pthread_create(&c_tid, &c_attr, consumer, NULL);
    }

    /* 5. Sleep */
    sleep(sleep_time);

    /* 6. Release resources, e.g. destroy mutex and semaphores */
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    /* 7. Exit */
    pthread_exit(NULL);
}