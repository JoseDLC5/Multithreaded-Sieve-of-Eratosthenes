// I pledge my honor that I have abided by the Stevens Honor System.
// Jose de la Cruz
// Breanna Shinn

#include <pthread.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include <math.h>

int total_count = 0;
pthread_mutex_t lock;

typedef struct arg_struct
{
  int start;
  int end;
} thread_args;

int hasTwoThrees(int num)
{
  int three = 0;
  int digits = 0;
  int num_temp = num;
  while (num_temp != 0)
  {
    digits++;
    num_temp = num_temp / 10;
  }
  char num_str[digits + 50];
   sprintf(num_str, "%d", num);
  //itoa(num, num_str, 10);
  for (int i = 0; i < digits; i++)
  {
    if (num_str[i] == '3')
    {
      three++;
    }
  }
  if (three >= 2)
  {
    return 1;
  }
  return 0;
}

void *sieve(void *ptr)
{
  int retval;
  int primeCount = 0;
  thread_args *threadArgs = (thread_args *)ptr;

  int a = threadArgs->start;
  int b = threadArgs->end;
  int rootB = sqrt((double)b);

  //1.
  int *low_primes = malloc((rootB + 1) * sizeof(int));

  for (size_t i = 0; i <= rootB; i++)
  {
    low_primes[i] = 1;
  }

  for (int i = 2; i * i <= rootB; i++)
  {
    if (low_primes[i] == 1)
    {
      for (int j = i * i; j <= rootB; j += i)
        low_primes[j] = 0;
    }
  }

  //2.
  int *high_primes = malloc((b - a + 1) * sizeof(int));

  for (int i = 0; i <= (b - a); i++)
  {
    high_primes[i] = 1;
  }

  //3.
  for (int p = 2; p <= rootB; p++)
  {
    if ((low_primes[p] == 1))
    {
      int i = ceil((double)a / p) * p - a;
      if (a <= p)
      {
        i = i + p;
      }

      for (int x = i; x <= (b - a); x = x + p)
      {
        high_primes[x] = 0;
      }
    }
  }

  for (int i = 0; i <= (b - a); i++)
  {
    if ((high_primes[i] == 1) && hasTwoThrees(i + a)) //check for 3's
    {
      primeCount++;
    }
  }

  free(low_primes);
  free(high_primes);

  if ((retval = pthread_mutex_lock(&lock)) != 0)
  {
    fprintf(stderr, "Warning: Cannot lock mutex. %s.\n",
            strerror(retval));
  }

  total_count += primeCount;

  if ((retval = pthread_mutex_unlock(&lock)) != 0)
  {
    fprintf(stderr, "Warning: Cannot unlock mutex. %s.\n",
            strerror(retval));
  }
  pthread_exit(NULL);
}

void single_sieve(int start, int end)
{
  int primeCount = 0;

  int *prime = malloc((end + 1) * sizeof(int));
  for (int i = 0; i <= end; i++)
  {
    prime[i] = 1;
  }

  for (int i = 2; i * i <= end; i++)
  {
    if (prime[i] == 1)
    {
      for (int j = i * i; j <= end; j += i)
      {
        prime[j] = 0;
      }
    }
  }
  for (int i = start; i <= end; i++)
  {
    if ((prime[i] == 1) && (hasTwoThrees(i) == 1)) //check for 3's
    {
      primeCount++;
    }
  }
  free(prime);
  total_count += primeCount;
}

int main(int argc, char **argv)
{
  int sFlag = 0;
  int sValue;
  int eFlag = 0;
  int eValue;
  int tFlag = 0;
  int tValue = 0;
  int c;
  char *t;
  char *s;
  char *e;
  {
    if (argc == 1)
    {
      fprintf(stderr, "Usage: ./mtsieve -s <starting value> -e <ending value> -t <num threads>");
      return EXIT_FAILURE;
    }
    opterr = 0;

    while ((c = getopt(argc, argv, "s:e:t:")) != -1)
      switch (c)
      {
      case 's':
        sFlag = 1;
        for (s = &optarg[0]; *s != '\0'; s++)
        {
          if (isdigit(*s) == 0)
          {
            fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'.\n", optarg, 's');
            return EXIT_FAILURE;
          }
        }
        errno = 0;
        sValue = strtol(optarg, NULL, 10);
        if (errno != 0)
        {
          fprintf(stderr, "Error: Integer overflow for parameter '-%c'.\n", 's');
          return EXIT_FAILURE;
        }

        break;
      case 'e':
        eFlag = 1;
        for (e = &optarg[0]; *e != '\0'; e++)
        {
          if (isdigit(*e) == 0)
          {
            fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'.\n", optarg, 'e');
            return EXIT_FAILURE;
          }
        }
        errno = 0;
        eValue = strtol(optarg, NULL, 10);
        if (errno != 0)
        {
          fprintf(stderr, "Error: Integer overflow for parameter '-%c'.\n", 'e');
          return EXIT_FAILURE;
        }
        break;
      case 't':
        tFlag = 1;
        for (t = &optarg[0]; *t != '\0'; t++)
        {
          if (isdigit(*t) == 0)
          {
            fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'.\n", optarg, 't');
            return EXIT_FAILURE;
          }
        }
        errno = 0;
        tValue = strtol(optarg, NULL, 10);
        if (errno != 0)
        {
          fprintf(stderr, "Error: Integer overflow for parameter '-%c'.\n", 't');
          return EXIT_FAILURE;
        }
        break;
      case '?':
        if (optopt == 'e' || optopt == 's' || optopt == 't')
        {
          fprintf(stderr, "Error: Option -%c requires an argument.\n", optopt);
        }
        else if (isprint(optopt))
        {
          fprintf(stderr, "Error: Unknown option '-%c'.\n", optopt);
        }
        else
        {
          fprintf(stderr, "Error: Unknown option character '\\x%x'.\n", optopt);
        }
        return EXIT_FAILURE;
      }
    if (optind != argc)
    {
      fprintf(stderr, "Error: Non-option argument '%s' supplied.\n", argv[optind]);
      return EXIT_FAILURE;
    }
    if (sFlag == 0)
    {
      fprintf(stderr, "Error: Required argument <starting value> is missing.\n");
      return EXIT_FAILURE;
    }

    if (sValue < 2)
    {
      fprintf(stderr, "Error: Starting value must be >= 2.\n");
      return EXIT_FAILURE;
    }

    if (eFlag == 0)
    {
      fprintf(stderr, "Error: Required argument <ending value> is missing.\n");
      return EXIT_FAILURE;
    }

    if (eValue < 2)
    {
      fprintf(stderr, "Error: Ending value must be >= 2.\n");
      return EXIT_FAILURE;
    }

    if (eValue < sValue)
    {
      fprintf(stderr, "Error: Ending value must be >= starting value.\n");
      return EXIT_FAILURE;
    }

    if (tFlag == 0)
    {
      fprintf(stderr, "Error: Required argument <num threads> is missing.\n");
      return EXIT_FAILURE;
    }

    if (tValue < 1)
    {
      fprintf(stderr, "Error: Number of threads cannot be less than 1.\n");
      return EXIT_FAILURE;
    }

    if (tValue > (2 * get_nprocs()))
    {
      fprintf(stderr, "Error: Number of threads cannot exceed twice the number of processors(%d).\n", get_nprocs());
      return EXIT_FAILURE;
    }
  }

  int numsToTest = eValue - sValue + 1;
  if (numsToTest < tValue)
  {
    tValue = numsToTest;
  }
  int numEach = (numsToTest / tValue) - 1;
  int numLeft = numsToTest % tValue;

  thread_args tArgs[tValue];
  int start = sValue;
  for (int i = 0; i < tValue; i++)
  {
    tArgs[i].start = start;
    tArgs[i].end = start + numEach;
    if (numLeft > 0)
    {
      tArgs[i].end++;
      start++;
    }
    numLeft--;
    start += numEach + 1;
  }
  printf("Finding all prime numbers between %d and %d.\n", sValue, eValue);
  if (tValue == 1)
  {
    printf("%d segment:\n", tValue);
  }
  else
  {
    printf("%d segments:\n", tValue);
  }

  for (int i = 0; i < tValue; i++)
  {
    printf("   [%d, %d]\n", tArgs[i].start, tArgs[i].end);
  }

  //single_sieve(sValue, eValue);
  pthread_t threads[tValue];
  int retval;

  for (int i = 0; i < tValue; i++)
  {
    if ((retval = pthread_create(
             &threads[i], NULL, *sieve, &tArgs[i])) != 0)
    {
      fprintf(stderr, "Error: Cannot create thread %d. %s.\n",
              i, strerror(retval));
      return EXIT_FAILURE;
    }
  }

  for (int i = 0; i < tValue; i++)
  {
    if (pthread_join(threads[i], NULL) != 0)
    {
      fprintf(stderr, "Warning: Thread %d did not join properly.\n",
              i + 1);
    }
  }

  if ((retval = pthread_mutex_destroy(&lock)) != 0)
  {
    fprintf(stderr, "Warning: Cannot destroy mutex. %s.\n", strerror(retval));
  }

  printf("Total primes between %d and %d with two or more '3' digits: %d\n", sValue, eValue, total_count);

  return EXIT_SUCCESS;
}