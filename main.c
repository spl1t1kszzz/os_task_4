#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc/malloc.h>
#include <string.h>
#include <sys/mman.h>

int global_init = 1;
int global_not_init;
const int global_const = 2;
const int MEMORY_SIZE = 4;


void f() {
    int pid = getpid();
    int local;  // in [stack]
    static int static_local;
    const int const_f;
    printf("%d\n", pid);
    printf("local: %p\n", &local);
    printf("static_local: %p\n", &static_local);
    printf("const_f: %p\n", &const_f);
    printf("global_init: %p\n", &global_init);
    printf("global_not_init: %p\n", &global_not_init);
    printf("global_const: %p\n", &global_const);
    sleep(60);
}

void foo() {
    int array[4] = {0xaa, 0xbb, 0xcc, 0xdd};

}

void sigbus_handler(int sig) {
    printf("Received SIGBUS signal\n");
    exit(1);
}

int main(int argc, char* argv[]) {
    // a)
    {
        printf("%d\n", getpid());
        char *args[] = {argv[0], NULL};
        if (execv(args[0], args) == -1) {
            perror("execv");
            exit(-1);
        }
    }
    // c) (i - vii)
    {
        void* p;
        printf("%d\n", getpid());
        for (int i = 0; i < 10; ++i) {
            p = malloc(MEMORY_SIZE);
            if (p == NULL) {
                perror("malloc");
                exit(-1);
            }
            memset(p, 0xaa, MEMORY_SIZE);
        }
        free(p);
    }
    // (c.viii - e)
    {
        long page_size = sysconf(_SC_PAGE_SIZE);
        void *p = mmap(NULL, page_size * 10, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (p == MAP_FAILED) {
            perror("mmap");
            return -1;
        }
        signal(SIGBUS, sigbus_handler);
        *(int*)p = 1;
    }
    return 0;
}