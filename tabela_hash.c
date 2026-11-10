#include "tabela_hash.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static bool ehPrimo(size_t num) {
    if (num < 2) return false;
    if (num == 2 || num == 3) return true;
    if (num % 2 == 0 || num % 3 == 0) return false;
    for (size_t i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) return false;
    }
    return true;
}

static size_t primoAnterior(size_t limite) {
    if (limite <= 3) return 1;
    for (size_t p = limite - 1; p >= 2; --p) {
        if (ehPrimo(p)) return p;
    }
    return 1;
}

void inicializarTabela(TabelaHashEstoque *tab, size_t capacidade) {
    if (capacidade > CAPACIDADE_MAXIMA) {
        capacidade = CAPACIDADE_MAXIMA;
    }
    tab->m = capacidade;
    tab->m2 = primoAnterior(capacidade);
    tab->n = 0;
    tab->total_removidos = 0;
    tab->cont_colisoes = 0;
    tab->cont_sondagens = 0;
    tab->cont_comparacoes = 0;
    for (size_t i = 0; i < tab->m; ++i) {
        tab->slots[i].estado = SLOT_VAZIO;
    }
}

void normalizarChave(const char *origem, char *destino, size_t tam_max) {
    size_t j = 0;
    for (size_t i = 0; origem[i] != '\0' && j + 1 < tam_max; ++i) {
        unsigned char c = (unsigned char)origem[i];
        if (isalnum(c)) {
            destino[j++] = (char)toupper(c);
        }
    }
    destino[j] = '\0';
}

uint64_t valorPolinomial(const char *chave_norm) {
    uint64_t hash_val = 0;
    const uint64_t p = 31;
    for (size_t i = 0; chave_norm[i] != '\0'; ++i) {
        hash_val = hash_val * p + (uint64_t)((unsigned char)chave_norm[i]);
    }
    return hash_val;
}

size_t h1(const TabelaHashEstoque *tab, const char *chave_norm) {
    return (size_t)(valorPolinomial(chave_norm) % tab->m);
}

size_t h2(const TabelaHashEstoque *tab, const char *chave_norm) {
    if (tab->m2 <= 1) return 1;
    return (size_t)(1 + (valorPolinomial(chave_norm) % tab->m2));
}

size_t sondar(const TabelaHashEstoque *tab, size_t idx1, size_t passo_h2, size_t i) {
    return (idx1 + i * passo_h2) % tab->m;
}

bool inserir(TabelaHashEstoque *tab, const char *chave, Produto valor) {
    char k[TAM_CODIGO];
    normalizarChave(chave, k, sizeof(k));
    if (k[0] == '\0') return false;

    size_t idx1 = h1(tab, k);
    size_t passo = h2(tab, k);
    long primeiro_removido = -1;
    bool colidiu_na_inicial = false;

    for (size_t i = 0; i < tab->m; ++i) {
        size_t idx = sondar(tab, idx1, passo, i);
        tab->cont_sondagens++;

        if (tab->slots[idx].estado == SLOT_OCUPADO) {
            if (i == 0) colidiu_na_inicial = true;
            tab->cont_comparacoes++;
            if (strcmp(tab->slots[idx].registro.codigo, k) == 0) {
                return false; // Chave duplicada rejeitada
            }
        } else if (tab->slots[idx].estado == SLOT_REMOVIDO) {
            if (primeiro_removido == -1) {
                primeiro_removido = (long)idx;
            }
        } else { // SLOT_VAZIO
            if (colidiu_na_inicial) {
                tab->cont_colisoes++;
            }
            size_t destino = (primeiro_removido != -1) ? (size_t)primeiro_removido : idx;
            if (primeiro_removido != -1) {
                tab->total_removidos--;
            }
            tab->slots[destino].registro = valor;
            strncpy(tab->slots[destino].registro.codigo, k, TAM_CODIGO - 1);
            tab->slots[destino].registro.codigo[TAM_CODIGO - 1] = '\0';
            tab->slots[destino].estado = SLOT_OCUPADO;
            tab->n++;
            return true;
        }
    }

    if (primeiro_removido != -1) {
        if (colidiu_na_inicial) {
            tab->cont_colisoes++;
        }
        size_t destino = (size_t)primeiro_removido;
        tab->total_removidos--;
        tab->slots[destino].registro = valor;
        strncpy(tab->slots[destino].registro.codigo, k, TAM_CODIGO - 1);
        tab->slots[destino].registro.codigo[TAM_CODIGO - 1] = '\0';
        tab->slots[destino].estado = SLOT_OCUPADO;
        tab->n++;
        return true;
    }

    return false; // Tabela cheia (E1.1)
}

const Produto* buscar(TabelaHashEstoque *tab, const char *chave) {
    char k[TAM_CODIGO];
    normalizarChave(chave, k, sizeof(k));
    if (k[0] == '\0') return NULL;

    size_t idx1 = h1(tab, k);
    size_t passo = h2(tab, k);

    for (size_t i = 0; i < tab->m; ++i) {
        size_t idx = sondar(tab, idx1, passo, i);
        tab->cont_sondagens++;

        if (tab->slots[idx].estado == SLOT_VAZIO) {
            return NULL;
        }
        if (tab->slots[idx].estado == SLOT_OCUPADO) {
            tab->cont_comparacoes++;
            if (strcmp(tab->slots[idx].registro.codigo, k) == 0) {
                return &tab->slots[idx].registro;
            }
        }
    }
    return NULL;
}

bool atualizar(TabelaHashEstoque *tab, const char *chave, Produto novo_valor) {
    char k[TAM_CODIGO];
    normalizarChave(chave, k, sizeof(k));
    if (k[0] == '\0') return false;

    size_t idx1 = h1(tab, k);
    size_t passo = h2(tab, k);

    for (size_t i = 0; i < tab->m; ++i) {
        size_t idx = sondar(tab, idx1, passo, i);
        tab->cont_sondagens++;

        if (tab->slots[idx].estado == SLOT_VAZIO) {
            return false;
        }
        if (tab->slots[idx].estado == SLOT_OCUPADO) {
            tab->cont_comparacoes++;
            if (strcmp(tab->slots[idx].registro.codigo, k) == 0) {
                strncpy(tab->slots[idx].registro.nome, novo_valor.nome, TAM_NOME - 1);
                tab->slots[idx].registro.nome[TAM_NOME - 1] = '\0';
                tab->slots[idx].registro.quantidade = novo_valor.quantidade;
                tab->slots[idx].registro.preco = novo_valor.preco;
                return true;
            }
        }
    }
    return false;
}

bool remover(TabelaHashEstoque *tab, const char *chave) {
    char k[TAM_CODIGO];
    normalizarChave(chave, k, sizeof(k));
    if (k[0] == '\0') return false;

    size_t idx1 = h1(tab, k);
    size_t passo = h2(tab, k);

    for (size_t i = 0; i < tab->m; ++i) {
        size_t idx = sondar(tab, idx1, passo, i);
        tab->cont_sondagens++;

        if (tab->slots[idx].estado == SLOT_VAZIO) {
            return false;
        }
        if (tab->slots[idx].estado == SLOT_OCUPADO) {
            tab->cont_comparacoes++;
            if (strcmp(tab->slots[idx].registro.codigo, k) == 0) {
                tab->slots[idx].estado = SLOT_REMOVIDO;
                tab->n--;
                tab->total_removidos++;
                return true;
            }
        }
    }
    return false;
}

void listarRegistros(const TabelaHashEstoque *tab) {
    printf(" Registros Ativos no Estoque (n = %zu) \n", tab->n);
    for (size_t i = 0; i < tab->m; ++i) {
        if (tab->slots[i].estado == SLOT_OCUPADO) {
            const Produto *p = &tab->slots[i].registro;
            printf("[%2zu] Codigo: %s | Nome: %s | Qtd: %d | Preco: R$ %.2f\n",
                   i, p->codigo, p->nome, p->quantidade, p->preco);
        }
    }
}

void exibirEstrutura(const TabelaHashEstoque *tab) {
    printf(" Estado da Tabela Hash (m = %zu, m2 = %zu, n = %zu, alpha = %.3f, colisoes = %zu) \n",
           tab->m, tab->m2, tab->n, fatorCarga(tab), tab->cont_colisoes);
    for (size_t i = 0; i < tab->m; ++i) {
        printf("Indice [%2zu]: ", i);
        if (tab->slots[i].estado == SLOT_VAZIO) {
            printf("[ VAZIO ]\n");
        } else if (tab->slots[i].estado == SLOT_REMOVIDO) {
            printf("[ REMOVIDO (Tombstone) ]\n");
        } else {
            const Produto *p = &tab->slots[i].registro;
            printf("[ OCUPADO ] Chave=%s (h1=%zu, h2=%zu) | Nome=%s | Qtd=%d | R$ %.2f\n",
                   p->codigo, h1(tab, p->codigo), h2(tab, p->codigo),
                   p->nome, p->quantidade, p->preco);
        }
    }
}

double fatorCarga(const TabelaHashEstoque *tab) {
    return tab->m > 0 ? (double)tab->n / (double)tab->m : 0.0;
}

void zerarContadoresOperacionais(TabelaHashEstoque *tab) {
    tab->cont_sondagens = 0;
    tab->cont_comparacoes = 0;
}