#ifndef TESTER_H
#define TESTER_H

#include <stddef.h>

typedef struct Tester *Tester;

Tester create_tester(const char *test, const char *imp_a, const char *imp_b, size_t test_len);
void close_tester(Tester tester);

void run_tester(Tester tester);

#endif