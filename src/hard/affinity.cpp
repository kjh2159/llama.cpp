#include "affinity.h"


static pid_t gettid_() { 
    return (pid_t)syscall(SYS_gettid);
}

static void pin_tid(pid_t tid, std::initializer_list<int> cpus) {
  cpu_set_t cs; CPU_ZERO(&cs);
  for (int c : cpus) CPU_SET(c, &cs);
  sched_setaffinity(tid, sizeof(cs), &cs);
}

void pin_current(std::initializer_list<int> cpus) {
  pin_tid(gettid_(), cpus);
}