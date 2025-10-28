/**
 * @file syscalls.c
 * @brief Minimal system call stubs for GBA
 */

#include <sys/stat.h>
#include <errno.h>

// Heap management
extern char __heap_start;
extern char __heap_end;
static char *heap_ptr = &__heap_start;

void *_sbrk(int incr) {
    char *prev_heap = heap_ptr;

    if (heap_ptr + incr > &__heap_end) {
        errno = ENOMEM;
        return (void *)-1;
    }

    heap_ptr += incr;
    return prev_heap;
}

// Minimal file I/O stubs
int _close(int file) {
    return -1;
}

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    return 0;
}

int _read(int file, char *ptr, int len) {
    return 0;
}

int _write(int file, char *ptr, int len) {
    return len;
}

void _exit(int status) {
    while (1) {}
}

int _kill(int pid, int sig) {
    return -1;
}

int _getpid(void) {
    return 1;
}
