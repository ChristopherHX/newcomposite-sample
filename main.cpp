#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <libkern/OSCacheControl.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>

bool skipcrash = false;

void handler(int, siginfo_t *info, ucontext_t *uap) {
  printf("crash test pc %llx\n", (long long)uap->uc_mcontext->__ss.__pc);
  printf("-9 instruction %x\n", *(uint32_t*)(intptr_t)(uap->uc_mcontext->__ss.__pc - 36));
  printf("-8 instruction %x\n", *(uint32_t*)(intptr_t)(uap->uc_mcontext->__ss.__pc - 32));
  printf("-7 instruction %x\n", *(uint32_t*)(intptr_t)(uap->uc_mcontext->__ss.__pc - 28));
  printf("-6 instruction %x\n", *(uint32_t*)(intptr_t)(uap->uc_mcontext->__ss.__pc - 24));
  printf("-5 instruction %x\n", *(uint32_t*)(intptr_t)(uap->uc_mcontext->__ss.__pc - 20));
  printf("-4 instruction %x\n", *(uint32_t*)(intptr_t)(uap->uc_mcontext->__ss.__pc - 16));
  printf("-3 instruction %x\n", *(uint32_t*)(intptr_t)(uap->uc_mcontext->__ss.__pc - 12));
  printf("-2 instruction %x\n", *(uint32_t*)(intptr_t)(uap->uc_mcontext->__ss.__pc - 8));
  printf("-1 instruction %x\n", *(uint32_t*)(intptr_t)(uap->uc_mcontext->__ss.__pc - 4));
  printf("current instruction %x\n", *(uint32_t*)(intptr_t)(uap->uc_mcontext->__ss.__pc));
  printf("next instruction %x\n", *(uint32_t*)(intptr_t)(uap->uc_mcontext->__ss.__pc + 4));
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
  void* pointer = nullptr;
  __asm__ volatile ("mrs %0, tpidrro_el0" : "=r" (tcb));
  auto pointer0 = tcb[0x28 / sizeof(void*)];
  __asm__ volatile ("mrs %0, tpidr_el0" : "=r" (tcb));
  // create a crash
  pointer = tcb[0x28 / sizeof(void*)];
  printf("finish test %p %p\n", pointer0, pointer);
}
