# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <sys/mman.h>
# include <sched.h>
// int gettimeofday(struct timeval *restrict tp, void *restrict tzp);
// gcc -W -Wall -O2 -o gettime gettime.c
typedef struct timeval timesignature;
inline void tt_gettime (timesignature* tvr) {
  gettimeofday(tvr, 0);
}

inline long tt_time_diff (timesignature* a, timesignature* b) {
  return a->tv_sec * 1000000 + a->tv_usec - (b->tv_sec * 1000000 + b->tv_usec);
}

#define NLOOPS 5000000000
#define SPIKE_THRESHOLD 40
struct {
  long r_min, r_count;
} hist[] = { {0,0}, {1,0}, {2,0}, {5,0}, {10,0}, {20,0}, {50,0}, {100,0}, {200,0}, {500,0}, {1000,0}, {2000,0}, {5000,0} };

int main ()
{
  // long i, j, x, rv, diff;
  long i;
  int rv, diff;
  struct timeval t_stamps[2];
  struct timeval last_spike_time = {0,0};
  struct sched_param sp = { 99 };
  printf ("sched_setscheduler(): %d\n", sched_setscheduler (0, SCHED_FIFO, &sp));
  rv = mlockall (MCL_CURRENT | MCL_FUTURE);
  fprintf (stderr, "mlockall(): %d\n", rv);
  gettimeofday(&t_stamps[0], 0);
  for (i = 1; i <= NLOOPS; i++) {
    gettimeofday (&t_stamps[i%2], 0);
    diff = tt_time_diff(&t_stamps[i%2], &t_stamps[(i-1)%2]);
    if (diff > SPIKE_THRESHOLD) {
      printf("Latency spike of %d usec\n", diff);
      long gap = t_stamps[i%2].tv_sec * 1000000 + t_stamps[i%2].tv_usec -
       (last_spike_time.tv_sec * 1000000 + last_spike_time.tv_usec);
    if (last_spike_time.tv_sec != 0)
      printf("%ld usec since last spike\n", gap);
      last_spike_time.tv_sec = t_stamps[i%2].tv_sec;
      last_spike_time.tv_usec = t_stamps[i%2].tv_usec;
    }
  }
  return 0;
}

