#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>

void stackTrace(void) {
    char **strings;
    size_t i, size;
    enum Constexpr { MAX_SIZE = 1024 };
    void *array[MAX_SIZE];
    size = backtrace(array, MAX_SIZE);
    strings = backtrace_symbols(array, size);
    for (i = 0; i < size; i++) {
      printf("%s\n", strings[i]);
    }
    puts("");
    free(strings);
}
