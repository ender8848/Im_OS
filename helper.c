/******************************************************************
 * The helper file that contains the following helper functions:
 * check_arg - Checks if command line input is a number and returns it
 * sem_create - Create number of sempahores required in a semaphore array
 * sem_init - Initialise particular semaphore in semaphore array
 * sem_wait - Waits on a semaphore (akin to down ()) in the semaphore array
 * sem_signal - Signals a semaphore (akin to up ()) in the semaphore array
 * sem_close - Destroy the semaphore array
 ******************************************************************/

# include "helper.h"
# include <exception>

vector<int> parse_args(int argc, char** argv) {
  // wrong use
  if (argc != 5) {
    std::string error = "Usage: " + std::string(argv[0]) + 
    " <num of processes> <num of jobs per producer> <num of producers> <num of consumers>";
    // return an exception with the message 
    throw std::runtime_error(error);
  }
  // wrong number
  for (int i = 1; i < argc; i++) {
    int j = 0;
    while (argv[i][j] != '\0') {
      if (argv[i][j] > '9' || argv[i][j] < '0') {
        std::string error = "Invalid arg " + std::string(argv[i]) + "\n";
        throw std::runtime_error(error);
      }
      j++;
    }
  }
  // 0 as argument
  for (int i = 1; i < argc; i++) {
    if (std::stoi(argv[i]) == 0) {
      std::string error = "Invalid arg " + std::string(argv[i]) + "\n";
      throw std::runtime_error(error);
    }
  }
  // parse args
  return {atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4])};
}

int sem_create (key_t key, int num)
{
  int id;
  if ((id = semget (key, num,  0666 | IPC_CREAT | IPC_EXCL)) < 0)
    return -1;
  return id;
}

int sem_init (int id, int num, int value)
{
  union semun semctl_arg;
  semctl_arg.val = value;
  if (semctl (id, num, SETVAL, semctl_arg) < 0)
    return -1;
  return 0;
}

void sem_wait (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  semop (id, op, 1);
}

void sem_signal (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, 1, SEM_UNDO}
  };
  semop (id, op, 1);
}

int sem_close (int id)
{
  if (semctl (id, 0, IPC_RMID, 0) < 0)
    return -1;
  return 0;
}
