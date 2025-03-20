#include "genalg.h"

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

typedef struct scored_index {
    double score;
    int index;
} scored_index;

typedef struct GenAlg {
    uint8_t *population;
    uint8_t *children;
    
    ScoreFunc score;
    DisplayFunc display;
    
    scored_index *score_arr;
    double total_score;

    double mutation_chance;

    int child_cnt;
    int elite_cnt;
    int pop_cnt;

    int gene_size;
    int allele_count;
    int allele_size;
    
    double sd;
}* GenAlg;

#define gene_index(alg, ind) ((ind) * (alg)->gene_size)
#define sorted_gene(alg, ind) ((alg)->population + gene_index((alg), (alg)->score_arr[(ind)].index))
#define get_child(alg, ind) ((alg)->children + gene_index(alg, ind))
#define allele_offset(alg, ind) ((ind) * (alg)->allele_size)
#define gene_f_score(alg, score) ((alg)->population + (score)->index * (alg)->gene_size)

void print_scores(GenAlg alg, int until) {
    int end = until < 0 ? alg->pop_cnt : until;
    for(int i = 0; i < end && i < alg->pop_cnt; i++) {
        printf("%3d %f\n", alg->score_arr[i].index, alg->score_arr[i].score);
    }
    printf("\n");
}

void print_gene(GenAlg alg, uint8_t *gene) {
    for(int j = 0; j < alg->gene_size; j++) {
        printf("%02x ", gene[j]);
    }
    printf("\n");
}

void print_genes(GenAlg alg, int until) {
    int end = until < 0 ? alg->pop_cnt : until;
    for(int i = 0; i < end && i < alg->pop_cnt; i++) {
        print_gene(alg, sorted_gene(alg, i));
    }
    if(end > 0) {
        printf("\n");
    }
}

double drand(double max) {
    return rand() / (RAND_MAX / max);
}

uint8_t *select_parent_rlet(GenAlg alg) {
    // Choose a value between 0 and the total score of the generation.
    double choice = drand(alg->total_score);

    // Sum up the score from the potential parents until the it reaches the chosen value.
    scored_index *parent_score = alg->score_arr;
    for(double pos = parent_score->score; pos < choice; pos += parent_score->score) {
        parent_score++;
    }

    // Return the parent.
    return gene_f_score(alg, parent_score);
}

uint8_t *select_parent_msus(GenAlg alg, int child) {
    // Calculate the size of each stretch to take.
    double segment_size = alg->total_score / alg->child_cnt;

    // create a random number between 0 and segment size and then add the offset for the child.
    double choice = drand(segment_size) + child * segment_size;

    // Sum up the score from the potential parents until the it reaches the chosen value.
    scored_index *parent_score = alg->score_arr;
    for(double pos = parent_score->score; pos < choice; pos += parent_score->score) {
        parent_score++;
    }

    // Return the parent.
    return gene_f_score(alg, parent_score);
}

uint8_t *select_parent_torn(GenAlg alg) {
    // Select two random potential parents.
    scored_index *a = alg->score_arr + (rand() % alg->pop_cnt);
    scored_index *b = alg->score_arr + (rand() % alg->pop_cnt);

    // Return the parent with the highest score.
    return gene_f_score(alg, a->score > b->score ? a : b);
}

void copy_allele(GenAlg alg, uint8_t *src, int index, uint8_t *out) {
    off_t offset = allele_offset(alg, index);
    memcpy(out + offset, src + offset, alg->allele_size);
}

void swap_child(GenAlg alg, int src_ind, int out_ind) {
    memcpy(
        sorted_gene(alg, out_ind),
        get_child(alg, src_ind),
        alg->gene_size
    );
}

void mutate_gene(GenAlg alg, uint8_t *gene) {
    // Loop over all alleles.
    for(int i = 0; i < alg->gene_size; i++) {
        // Decide if an allele should be mutated.
        if(drand(1) <= alg->mutation_chance) {
            // Add a small random value to the allele.
            gene[i] += (rand() % 11) - 5;
        }
    }
}

void uniform_crossover(GenAlg alg, uint8_t *a, uint8_t *b, uint8_t *out) {
    // Loop over all alleles in the new child.
    for(int i = 0; i < alg->allele_count; i++) {
        // Copy either parent a or parent b's allels with a 1:1 ratio.
        copy_allele(alg, rand() % 2 ? a : b, i, out);
    }
}

void k_point_crossover(GenAlg alg, uint8_t *a, uint8_t *b, uint8_t *out) {
    // Points a and b are chosen randomly.
    int point_a = (rand() % (alg->allele_count)) - 1;
    int point_b = (alg->allele_count - point_a - 2 == 0 ? -1 : (rand() % (alg->allele_count - point_a - 2))) + point_a + 3;

    int i = 0;

    // Copy the first part from parent a.
    for(; i <= point_a; i++) {
        copy_allele(alg, a, i, out);
    }

    // Copy the second part from parent b.
    for(; i < point_b; i++) {
        copy_allele(alg, b, i, out);
    }

    // Copy the third part from parent a.
    for(; i < alg->allele_count; i++) {
        copy_allele(alg, a, i, out);
    }
}

void create_children(GenAlg alg) {
    for(int i = 0; i < alg->child_cnt; i++) {
        uniform_crossover(alg, select_parent_torn(alg), select_parent_torn(alg), alg->children + (i * alg->gene_size));
        mutate_gene(alg, alg->children + (i * alg->gene_size));
    }
    for(int i = 0; i < alg->child_cnt; i++) {
        swap_child(alg, i, i + alg->elite_cnt);
    }
}

void randomise_genes(GenAlg alg, int from) {
    for(int i = from; i < alg->pop_cnt; i++) {
        uint8_t *gene = sorted_gene(alg, i);
        for(int j = 0; j < alg->gene_size; j++) {
            gene[j] = (uint8_t) rand();
        }
    }
}

void score_genes(GenAlg alg, void *env) {
    alg->total_score = 0;
    for (int i = 0; i < alg->pop_cnt; i++) {
        alg->score_arr[i].score = alg->score(alg->population + (i * alg->gene_size), env);
        alg->score_arr[i].index = i;
        alg->total_score += alg->score_arr[i].score;
    }
}

int compare(const void *a, const void *b) {
    return ((scored_index *) a)->score > ((scored_index *) b)->score ? -1 : 1;
}

void setup_indexes(GenAlg alg) {
    for(int i = 0; i < alg->pop_cnt; i++) {
        alg->score_arr[i].index = i;
    }
}

void run_alg(GenAlg alg, void *env) {
    setup_indexes(alg);
    print_scores(alg, 10);
    randomise_genes(alg, 0);
    while(1) {
        score_genes(alg, env);
        qsort(alg->score_arr, alg->pop_cnt, sizeof(scored_index), compare);

        print_scores(alg, 1);
        // print_genes(alg, 1);
        if(alg->display) {
            alg->display(sorted_gene(alg, 0), env);
        }

        create_children(alg);
        randomise_genes(alg, alg->child_cnt + alg->elite_cnt);
    }
}

GenAlg create_gen_alg(double child_pcnt, double elite_pcnt, int pop_cnt, int allele_count, int allele_size, double mutation_chance, ScoreFunc score, DisplayFunc display) {
    GenAlg alg = (GenAlg) malloc(sizeof(struct GenAlg));
    
    alg->score = score;
    alg->display = display;

    alg->child_cnt = child_pcnt * pop_cnt;
    alg->elite_cnt = elite_pcnt * pop_cnt;

    alg->gene_size = allele_count * allele_size;
    alg->allele_size = allele_size;
    alg->pop_cnt = pop_cnt;
    alg->allele_count = allele_count;

    alg->mutation_chance = mutation_chance;

    alg->population = malloc(alg->gene_size * pop_cnt);
    alg->children = malloc(alg->gene_size * alg->child_cnt);
    alg->score_arr = malloc(sizeof(scored_index) * pop_cnt);

    alg->sd = (double) INTMAX_MAX;
    return alg;
}

void close_gen_alg(GenAlg alg) {
    free(alg->population);
    free(alg->children);
    free(alg->score_arr);
    free(alg);
}