#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <libkern/OSCacheControl.h>
#include <stdlib.h>
#include <stdint.h>

int main() {
  printf("initialize test\n");
  void** tcb;
  __asm__ volatile ("mrs %0, tpidr_el0" : "=r" (tcb));
  // create a crash
  auto pointer = tcb[0x28];
  printf("finish test\n");
}
