#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <libkern/OSCacheControl.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>

bool skipcrash = false;

void handler(int, siginfo_t *info, ucontext_t *uap) {
  printf("crash test\n");
  printf("prev instruction %x", *(uint32_t*)(intptr_t)(uap->uc_mcontext->__ss.__pc - 4));
  printf("current instruction %x", *(uint32_t*)(intptr_t)(uap->uc_mcontext->__ss.__pc));
  if(skipcrash) {
    uap->uc_mcontext->__ss.__pc += 4;
    printf("try to continue\n");
  } else {
    skipcrash = true;
    uap->uc_mcontext->__ss.__pc -= 4;
    printf("simulate failed patch\n");
  }
}

int main() {
  printf("initialize crash handler\n");
  struct sigaction act;
  sigemptyset(&act.sa_mask);
  act.sa_sigaction = (decltype(act.sa_sigaction))handler;
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGSEGV, &act, 0);
  printf("initialize test\n");
  void** tcb;
  __asm__ volatile ("mrs %0, tpidr_el0" : "=r" (tcb));
  // create a crash
  auto pointer = tcb[0x28];
  printf("finish test\n");
}
