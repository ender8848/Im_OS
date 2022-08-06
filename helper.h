# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
# include <errno.h>
# include <string.h>
# include <pthread.h>
# include <ctype.h>
# include <iostream>
# include <vector>
using namespace std;

# define SEM_KEY 0x51 // Change this number as needed

union semun {
    int val;               /* used for SETVAL only */
    struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    ushort *array;         /* used for GETALL and SETALL */
};

struct Job
{
    int id;
    int duration; // sleep time
    bool finished = true; // if false, reject job arrangement
};

struct Args
{
    int size_of_q; // q -> queue
    int num_of_jobs_to_arrange; // = num_of_p * num_of_jobs_per_p
    int num_of_jobs_to_arrange_per_producer; 
    int num_of_jobs_available = 0;
    int num_of_producers;
    int num_of_consumers;
    int sem_id; 
};


/*
* parse args
* @param argc: number of arguments
* @param argv: array of arguments
* @return: vector of parsed arguments
*/
vector<int> parse_args(int argc, char** argv) ;

/* 
* create a semaphore array with num semaphores
* @oaram key: key for the semaphore array
* @param num: number of semaphores in the array
* @return: semaphore array id
*/
int sem_create (key_t key, int num);

/*
* initialize the num-th semaphore in the array as value
* @param id: semaphore array id
* @param num: the num-th semaphore to initialize
* @param value: value to initialize the semaphore -- size for p, 1 for mutex and 0 for c
*/
int sem_init (int id, int num, int value);


/*
* wait for semaphore
* @param id: semaphore array id
* @param num: the num-th semaphore in the array
* @param time: max time to wait
* @return: false if waited more than time, true otherwise, can be intepreted as patient or not
*/
bool timed_wait(int id, short unsigned int num, int time = 20);


/*
* signal the semaphore
* @param id: semaphore array id
* @param num: the num-th semaphore in the array
*/
void sem_signal (int id, short unsigned int num);

/*
* close the semaphore array and free up resources
* @param id: semaphore array id
* @return: 0 if successful, -1 otherwise
*/
int sem_close (int);