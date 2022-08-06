/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"


static Args args;
static int front = 0;
static int rear  = 0;
static Job* queue = nullptr;
static int const NUM_OF_SEM = 3;
void *producer (void *id); // producer and consumer are used as pthread function
void *consumer (void *id); // which in routine takes a void pointer and return a void pointer
static int const PRODUCER_SEM = 0;
static int const CONSUMER_SEM = 1;
static int const MUTEX = 2;


int main (int argc, char **argv)
{
  std::cerr << "-------------------------------" << std::endl; 
  // setup command-line args
  vector<int> args_;
  try {
    args_ = parse_args(argc, argv);
  } catch (exception& e) {
    cout << e.what() << endl;
    exit(1);
  }
  args.size_of_q = args_[0];
  args.num_of_jobs_to_arrange_per_producer = args_[1];
  args.num_of_producers = args_[2];
  args.num_of_consumers = args_[3];
  args.num_of_jobs_to_arrange = args_[1] * args_[2];

  // init semaphores
  args.sem_id = sem_create(SEM_KEY, NUM_OF_SEM);
  if (args.sem_id == -1) {
    std::cerr << "semaphore creation failed " << std::endl;
    exit(2);
  } else {
    std::cerr << "semaphors created" << std::endl;
  }

  // init semaphores
  sem_init(args.sem_id, PRODUCER_SEM, args.size_of_q); // producer job semaphore，namely vacant space left for jobs
  sem_init(args.sem_id, CONSUMER_SEM, 0);               // consumer job semaphore
  sem_init(args.sem_id, MUTEX, 1);               // Mutual Exclusion semaphore
  std::cerr << "semaphors initialized" << std::endl;


  // create producer, consumer threads and queue
  // pthread_t is the type that uniquely identifies a thread
  pthread_t producerids[args.num_of_producers];
  pthread_t consumerids[args.num_of_consumers];
  queue = new Job[args.size_of_q];
  // set job id using their indexed in queue
  for (int i = 0; i < args.size_of_q; i++) {queue[i].id = i;} 

  std::cerr << "threads and shared data created" << std::endl;
  std::cerr << "-------------------------------" << std::endl;

  // init producer and consumer threads
  // has to create something that can be referenced (person_id) as lvalue ref paremeter in thread function
  int len = std::max(args.num_of_producers, args.num_of_consumers);
  int person_id[len];
  for (int i = 0; i < len; i++) {person_id[i] = i+1;}
  // pthread_create requires a pthread identifier, a function to be run, 
  // and a parameter to be passed to the function (as a void pointer)
  for (int i = 0; i < args.num_of_producers; i++) {
    pthread_create(&producerids[i], NULL, producer, &person_id[i]);
  }
  for (int i = 0; i < args.num_of_consumers; i++) {
		pthread_create(&consumerids[i], NULL, consumer, &person_id[i]);
  }

  // wait until finished, close threads and free heap space
  // pthread_join waits until the thread specified by the identifier is finished
  for (int i = 0; i < args.num_of_producers; i++) {
    pthread_join(producerids[i], NULL);
  }
	for (int i = 0; i < args.num_of_consumers; i++) {
    pthread_join(consumerids[i], NULL);
  }
  sem_close(args.sem_id);

  delete [] queue; 
  std::cerr << "-------------------------------" << std::endl; 
  std::cerr << "Resrouces freed, program exited" << std::endl; 
  return 0;
}

void *producer (void *parameter) 
{
  int *param = (int *) parameter;
  int num_of_jobs_to_arrange = args.num_of_jobs_to_arrange_per_producer;
  bool still_patient = true;

  while (num_of_jobs_to_arrange && still_patient) {
    int job_id; // a temporary id used later

    /* -- critical section starts -- */
    // wait for vacant space left for jobs
    still_patient = timed_wait(args.sem_id, PRODUCER_SEM, 20); 
    // once got a job to arrange, wait for mutex
    timed_wait(args.sem_id, MUTEX, 20);
    if (still_patient && queue[rear].finished == true) {
      job_id = rear;
      queue[rear].duration = rand() % 10 + 1;
      queue[rear].finished = false;
      rear = (rear + 1) % args.size_of_q;
      --num_of_jobs_to_arrange;
      --args.num_of_jobs_to_arrange;
      ++args.num_of_jobs_available;
    }
    sem_signal(args.sem_id, MUTEX); 
    sem_signal(args.sem_id, CONSUMER_SEM); // signal that a job is available
    /* -- critical section ends -- */

    if (num_of_jobs_to_arrange == 0) {
      std::cerr << "Producer (" << *param << "): ";
      std::cerr << "No more jobs to generate." << std::endl;
      break;
    }
    // if there are sill jobs to arrange
    if (still_patient) {
      std::cerr << "Producer (" << *param << "): ";
	    std::cerr << "Job id " << queue[job_id].id << " duration ";
      std::cerr << queue[job_id].duration << std::endl;
      sleep (rand()%5 + 1);
    } else { 
      std::cerr << "Producer (" << *param << "): ";
      std::cerr << "Has waited too long!" << std::endl;
      break;
    }
  }

  pthread_exit(0);
}

void *consumer (void *parameter) 
{
  int *param = (int *) parameter;
  bool still_patient = true;
  
  // consumers should try to do at least once!
  do {
    int job_id; // a temporary id used later
    int workload = 0; // used for record working time

    /* -- critical section starts -- */
    // wait for a job to be available
    still_patient = timed_wait(args.sem_id, CONSUMER_SEM, 20);
    // wait for mutex
    timed_wait(args.sem_id, MUTEX, 20);
    if (still_patient && args.num_of_jobs_available > 0) {
      job_id = front;
      workload = queue[front].duration;
      queue[front].finished = true;
      front = (front + 1) % args.size_of_q;
      --args.num_of_jobs_available;
    }
    sem_signal(args.sem_id, MUTEX); // signal that mutex is free
    sem_signal(args.sem_id, PRODUCER_SEM); // signal that a job is available
    /* -- critical section ends -- */
    
    if (args.num_of_jobs_to_arrange == 0) {
      std::cerr << "Consumer (" << *param << "): ";
      std::cerr << "No more jobs left." << std::endl;
      break;
    }

    if (still_patient) {
      sleep(workload);
      std::cerr << "Consumer (" << *param << "): ";
      std::cerr << queue[job_id].id << " completed." << std::endl;
    } else { 
      std::cerr << "Consumer (" << *param << "): ";
      std::cerr << "Has waited too long!" << std::endl;
      break;
    }
  } while(args.num_of_jobs_to_arrange);

  pthread_exit (0);
}
