/******************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the function signatures and
 * the semaphore values (which are to be changed as needed).
 ******************************************************************/


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


int sem_create (key_t key, int num);


int sem_init (int id, int num, int);


/*
* wait for semaphore
* @param id: semaphore id
* @param num: semaphore number
* @param time: time to wait
* @return: false if waited more than time, true otherwise
*/
bool timed_wait(int id, short unsigned int num, int time);


void sem_signal (int, short unsigned int);


int sem_close (int);