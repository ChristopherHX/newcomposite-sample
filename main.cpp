#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <libkern/OSCacheControl.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>

void handler(int, siginfo_t *info, ucontext_t *uap) {
  printf("crash test\n");
  uap->__ss.__pc += 4;
  printf("try to continue\n");
}

int main() {
  printf("initialize crash handler\n");
  struct sigaction act;
  sigemptyset(&act.sa_mask);
  act.sa_sigaction = decltype(act.sa_sigaction)handler;
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGSEGV, &act, 0);
  printf("initialize test\n");
  void** tcb;
  __asm__ volatile ("mrs %0, tpidr_el0" : "=r" (tcb));
  // create a crash
  auto pointer = tcb[0x28];
  printf("finish test\n");
}
