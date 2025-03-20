#include "loader.h"
#include "testfuncs.h"
#include "genalg.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>

#define DIR "/"
#define IMPS "/imps/"
#define EXT ".so"

#define BASE_ADDR_LEN(test) (strlen(test) + sizeof(SHARED))
#define TEST_ADDR_LEN(test) (2 * strlen(test) + sizeof(SHARED DIR EXT))
#define IMP_ADDR_LEN(test, imp) (strlen(test) + strlen(imp) + sizeof(SHARED IMPS EXT))

#define IMP_A 0
#define IMP_B 1

typedef struct Tester {
    TestFuncs funcs;
    Funcs imp_funcs[2];
    void *imps[2];

    const TestFunc *tester_funcs;
    const char **tester_func_names;

    size_t func_count;
    size_t test_len;

    GenAlg alg;
}* Tester;

void create_base_addr(const char *test, char *out) {
    strcpy(out, SHARED);
    strcat(out, test);
}

void create_imp_addr(char *base, const char *imp, char *out) {
    strcpy(out, base);
    strcat(out, IMPS);
    strcat(out, imp);
    strcat(out, EXT);
}

void create_test_addr(char *base, const char *test, char *out) {
    strcpy(out, base);
    strcat(out, DIR);
    strcat(out, test);
    strcat(out, EXT);
}

int load_test_funcs(const char *test, const char *imp_a, const char *imp_b, Tester tester) {
    char base_addr[BASE_ADDR_LEN(test)];
    create_base_addr(test, base_addr);
    
    char test_addr[TEST_ADDR_LEN(test)];
    create_test_addr(base_addr, test, test_addr);

    char imp_a_addr[IMP_ADDR_LEN(test, imp_a)];
    char imp_b_addr[IMP_ADDR_LEN(test, imp_b)];
    create_imp_addr(base_addr, imp_a, imp_a_addr);
    create_imp_addr(base_addr, imp_b, imp_b_addr);

    tester->funcs = load_imp(test_addr);
    tester->imp_funcs[IMP_A] = load_imp(imp_a_addr);
    tester->imp_funcs[IMP_B] = load_imp(imp_b_addr);

    return tester->funcs && tester->imp_funcs[IMP_A] && tester->imp_funcs[IMP_B] ? 0 : -1;
}

void load_test_info(Tester tester) {
    tester->imps[IMP_A] = tester->funcs->create_imp(tester->imp_funcs[0]);
    tester->imps[IMP_B] = tester->funcs->create_imp(tester->imp_funcs[1]);

    tester->tester_funcs = tester->funcs->get_funcs();
    tester->tester_func_names = tester->funcs->get_func_names();
    tester->func_count = tester->funcs->func_count();
}

void run_test_func(Tester tester, int imp, int func) {
    tester->tester_funcs[func](tester->imps[imp], tester->imp_funcs[imp]);
}

uint64_t time_since(struct timeval start) {
    struct timeval end;
    gettimeofday(&end, NULL);

    uint64_t time = (end.tv_sec > start.tv_sec ? end.tv_sec - start.tv_sec : start.tv_sec - end.tv_sec) * 1000000ULL;
    time += end.tv_usec;
    time -= start.tv_usec;
}

typedef uint8_t allele;

#define function_index(gene, tester, test) (((allele *)(gene))[test] % (tester)->func_count)

uint64_t time_test(Tester tester, allele *gene, int imp) {

    struct timeval start;
    gettimeofday(&start, NULL);

    tester->funcs->reset_imp(tester->imp_funcs[imp], tester->imps[imp]);
    
    for(size_t test = 0; test < tester->test_len; test++) {
        run_test_func(tester, imp, function_index(gene, tester, test));
    }

    return time_since(start);
}

double test_score(uint8_t *gene, void *env) {
    uint64_t a = time_test(env, gene, IMP_A);
    uint64_t b = time_test(env, gene, IMP_B);
    return (double) (a > b ? a - b : b - a);
}

void test_display(uint8_t *gene, void *env) {
    Tester tester = (Tester) env;
    for(size_t test = 0; test < tester->test_len; test++) {
        printf("%10s%c", tester->tester_func_names[function_index(gene, tester, test)], test % 21 == 20 || test == tester->test_len - 1 ? '\n' : ' ');
    }
    printf("\n");
}

Tester create_tester(const char *test, const char *imp_a, const char *imp_b, size_t test_len) {
    Tester tester = (Tester) malloc(sizeof(struct Tester));
    tester->test_len = test_len;

    if(load_test_funcs(test, imp_a, imp_b, tester) < 0) {
        return NULL;
    }
    load_test_info(tester);

    tester->alg = create_gen_alg(0.5, 0.1, 50, tester->test_len, sizeof(allele), 0.001, test_score, NULL);

    return tester;
}

void run_tester(Tester tester) {
    run_alg(tester->alg, tester);
}

void close_tester(Tester tester) {
    tester->funcs->close_imp(tester->imp_funcs[IMP_A], tester->imps[IMP_A]);
    tester->funcs->close_imp(tester->imp_funcs[IMP_A], tester->imps[IMP_B]);
    close_gen_alg(tester->alg);
    free(tester);
}