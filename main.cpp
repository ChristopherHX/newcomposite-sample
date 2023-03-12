#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <libkern/OSCacheControl.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t* c_get_memory(uint32_t size) {
    int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
    int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_JIT;
    int fd = -1;
    int offset = 0;
    uint32_t* addr = 0;

    addr = (uint32_t*)mmap(0, size, prot, flags, fd, offset);
    if (addr == MAP_FAILED){
        printf("failure detected\n");
        exit(-1);
    }

    return addr;
}

int main() {
  pthread_jit_write_protect_np(0);
  auto mem = c_get_memory(16384 * 10);
  printf("c_get_memory %d\n", (int)(intptr_t)mem);
  mem[0] = 12;
  mem[1] = 15;
  printf("mem[0] %d, mem[1] %d\n", (int)mem[0], (int)mem[1]);
  pthread_jit_write_protect_np(1);
  sys_icache_invalidate(mem, 16384 * 10);
  printf("mem[0] %d, mem[1] %d\n", (int)mem[0], (int)mem[1]);
  pthread_jit_write_protect_np(0);
  auto fixedaddr = (uint32_t*)mmap((char*)mem + 4096 * 5, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
  printf("fixedaddr %d\n", (int)(intptr_t)fixedaddr);
  pthread_jit_write_protect_np(1);
  sys_icache_invalidate(mem, 16384 * 10);
  if(!fixedaddr) return 1;
  fixedaddr[0] = 12;
  fixedaddr[1] = 15;
  printf("mem[0] %d, mem[1] %d\n", (int)mem[0], (int)mem[1]);
  printf("fixedaddr[0] %d, fixedaddr[1] %d\n", (int)fixedaddr[0], (int)fixedaddr[1]);
}
