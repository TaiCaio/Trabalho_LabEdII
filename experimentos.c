#include "tabela_hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static TabelaHashEstoque tab_exp;
static char chaves_ins[10000][TAM_CODIGO];
static char chaves_aus[10000][TAM_CODIGO];

static uint32_t lcg_rand(uint32_t *state) {
    *state = (*state) * 1664525u + 1013904223u;
    return *state;
}

static void gerarConjuntoChaves(char destino[][TAM_CODIGO], size_t qtd, uint32_t seed, const char *prefixo) {
    uint32_t st = seed;
    for (size_t i = 0; i < qtd; ++i) {
        int cat = 10 + (int)(lcg_rand(&st) % 90);
        snprintf(destino[i], TAM_CODIGO, "%s-%02d-%06zu", prefixo, cat, i + 1);
    }
}

static double tempoMs(struct timespec inicio, struct timespec fim) {
    double s = (double)(fim.tv_sec - inicio.tv_sec) * 1000.0;
    double ns = (double)(fim.tv_nsec - inicio.tv_nsec) / 1000000.0;
    return s + ns;
}

static void executarExperimento(size_t n_registros, size_t capacidade, int repeticoes) {
    double soma_colisoes = 0.0;
    double soma_sond_ins = 0.0;
    double soma_sond_ok = 0.0;
    double soma_sond_aus = 0.0;
    double soma_t_ins = 0.0;
    double soma_t_ok = 0.0;
    double soma_t_aus = 0.0;

    for (int r = 0; r < repeticoes; ++r) {
        uint32_t seed = (uint32_t)(1000 + r * 37);
        gerarConjuntoChaves(chaves_ins, n_registros, seed, "SKU");
        gerarConjuntoChaves(chaves_aus, n_registros, seed + 5000, "AUS");

        inicializarTabela(&tab_exp, capacidade);

        struct timespec t0, t1, t2, t3, t4, t5;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (size_t i = 0; i < n_registros; ++i) {
            Produto p;
            strncpy(p.codigo, chaves_ins[i], TAM_CODIGO - 1);
            p.codigo[TAM_CODIGO - 1] = '\0';
            snprintf(p.nome, TAM_NOME, "Produto_%zu", i);
            p.quantidade = (int)(i % 100) + 1;
            p.preco = 29.90;
            inserir(&tab_exp, chaves_ins[i], p);
        }
        clock_gettime(CLOCK_MONOTONIC, &t1);
        size_t sond_ins = tab_exp.cont_sondagens;
        size_t col_ins = tab_exp.cont_colisoes;

        zerarContadoresOperacionais(&tab_exp);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        for (size_t i = 0; i < n_registros; ++i) {
            volatile const Produto *p = buscar(&tab_exp, chaves_ins[i]);
            (void)p;
        }
        clock_gettime(CLOCK_MONOTONIC, &t3);
        size_t sond_ok = tab_exp.cont_sondagens;

        zerarContadoresOperacionais(&tab_exp);
        clock_gettime(CLOCK_MONOTONIC, &t4);
        for (size_t i = 0; i < n_registros; ++i) {
            volatile const Produto *p = buscar(&tab_exp, chaves_aus[i]);
            (void)p;
        }
        clock_gettime(CLOCK_MONOTONIC, &t5);
        size_t sond_aus = tab_exp.cont_sondagens;

        soma_colisoes += (double)col_ins;
        soma_sond_ins += (double)sond_ins / (double)n_registros;
        soma_sond_ok += (double)sond_ok / (double)n_registros;
        soma_sond_aus += (double)sond_aus / (double)n_registros;

        soma_t_ins += tempoMs(t0, t1);
        soma_t_ok += tempoMs(t2, t3);
        soma_t_aus += tempoMs(t4, t5);
    }

    printf("%-10zu %-8zu %-8.4f %-11.1f %-10.3f %-10.3f %-10.3f %-10.4f %-12.4f %-12.4f\n",
           n_registros, capacidade, (double)n_registros / (double)capacidade,
           soma_colisoes / repeticoes, soma_sond_ins / repeticoes,
           soma_sond_ok / repeticoes, soma_sond_aus / repeticoes,
           soma_t_ins / repeticoes, soma_t_ok / repeticoes, soma_t_aus / repeticoes);
}

int main(void) {
    printf(" ANALISE EXPERIMENTAL EM C - CONFIGURACAO 70 (E1.1, H6, C4, A2) - MEDIA DE 5 REPETICOES\n");

    printf("Experimento A: Capacidade Estatica Unica (m = 15013) variando Carga (n)\n");
    printf("%-10s %-8s %-8s %-11s %-10s %-10s %-10s %-10s %-12s %-12s\n",
           "Registros", "Cap.", "Alpha", "Colisoes", "Sond/Ins", "Sond/Ok", "Sond/Aus", "Ins.(ms)", "BuscaOk(ms)", "BuscaAus(ms)");
    size_t cargas[3] = {100, 1000, 10000};
    for (int i = 0; i < 3; ++i) {
        executarExperimento(cargas[i], 15013, 5);
    }

    printf("\nExperimento B: Capacidade Estatica Dimensionada por Cenario (alpha ~ 0.66)\n");
    printf("%-10s %-8s %-8s %-11s %-10s %-10s %-10s %-10s %-12s %-12s\n",
           "Registros", "Cap.", "Alpha", "Colisoes", "Sond/Ins", "Sond/Ok", "Sond/Aus", "Ins.(ms)", "BuscaOk(ms)", "BuscaAus(ms)");
    size_t caps[3] = {151, 1511, 15013};
    for (int i = 0; i < 3; ++i) {
        executarExperimento(cargas[i], caps[i], 5);
    }

    return 0;
}
