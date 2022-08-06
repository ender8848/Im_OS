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
void *producer (void *id);
void *consumer (void *id);

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

  // create producer, consumer threads and queue
  pthread_t producerids[args.num_of_producers];
  pthread_t consumerids[args.num_of_consumers];
  queue = new Job[args.size_of_q];
  // set job id using their indexed in queue
  for (int i = 0; i < args.size_of_q; i++) {queue[i].id = i;} 

  std::cerr << "threads and shared data created" << std::endl;
  std::cerr << "-------------------------------" << std::endl;

  // init producer and consumer threads
  // has to create something that can be referenced (person_id)
  int len = std::max(args.num_of_producers, args.num_of_consumers);
  int person_id[len];
  for (int i = 0; i < len; i++) {person_id[i] = i+1;}
  for (int i = 0; i < args.num_of_producers; i++) {
    pthread_create(&producerids[i], NULL, producer, &person_id[i]);
  }
  for (int i = 0; i < args.num_of_consumers; i++) {
		pthread_create(&consumerids[i], NULL, consumer, &person_id[i]);
  }

  // wait until finished, close threads and free heap space
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

  // TODO

  int *param = (int *) parameter;

  cout << "Parameter = " << *param << endl;

  sleep (5);

  cout << "\nThat was a good sleep - thank you \n" << endl;

  pthread_exit(0);
}

void *consumer (void *id) 
{
    // TODO 

  pthread_exit (0);

}
