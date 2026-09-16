#ifndef IDS_TEST_UTILS_H
#define IDS_TEST_UTILS_H


#include <stdio.h>
#include <stdlib.h>


#define CHECK(expr)                                       \
    do {                                                  \
        if(!(expr)) {                                     \
            fprintf(stderr, "%s:%d: check failed: %s\n",  \
                    __FILE__, __LINE__, #expr);           \
            abort();                                      \
        }                                                 \
    } while(0)


#endif
