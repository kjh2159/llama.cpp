#ifndef __AFFINITY_H
#define __AFFINITY_H

#include <sched.h>       // for sched_setaffinity
#include <sys/syscall.h> // for syscall(SYS_gettid)
#include <unistd.h>      // for syscall

#include <vector>        // for std::initializer_list

static pid_t gettid_();
static void pin_tid(pid_t tid, std::initializer_list<int> cpus);
void pin_current(std::initializer_list<int> cpus);


#endif // __AFFINITY_H