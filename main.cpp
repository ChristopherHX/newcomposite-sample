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

    pthread_jit_write_protect_np(0);
    sys_icache_invalidate(addr, size);

    return addr;
}

int main() {
  printf("c_get_memory %d", (int)(intptr_t)c_get_memory(1024);
}
