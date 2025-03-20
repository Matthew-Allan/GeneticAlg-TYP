#ifndef GEN_ALG_H
#define GEN_ALG_H

#include <stdint.h>

typedef struct GenAlg* GenAlg;

typedef double (* ScoreFunc)(uint8_t *gene, void *env);
typedef void (* DisplayFunc)(uint8_t *gene, void *env);

void run_alg(GenAlg alg, void *env);
GenAlg create_gen_alg(double child_pcnt, double elite_pcnt, int pop_cnt, int allele_count, int allele_size, double mutation_chance, ScoreFunc score, DisplayFunc display);
void close_gen_alg(GenAlg alg);

double get_sd(GenAlg alg);
double get_mean(GenAlg alg);

#endif