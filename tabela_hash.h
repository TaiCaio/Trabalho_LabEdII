#ifndef TABELA_HASH_H
#define TABELA_HASH_H

#include "produto.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define CAPACIDADE_MAXIMA 20000

typedef enum{
  SLOT_VAZIO = 0,
  SLOT_OCUPADO = 1,
  SLOT_REMOVIDO = 2
} EstadoSlot;

typedef struct {
  Produto registro;
  EstadoSlot estado;
} Slot;

typedef struct {
  size_t m;
  size_t m2;
  size_t n;
  size_t total_removidos;
  size_t cont_colisoes;
  size_t cont_sondagens;
  size_t cont_comparacoes;
} TabelaHashEstoque;

// Funções para preparar a estrutura e tratar as entradas antes das operações
void inicializarTabela(TabelaHashEstoque *tab, size_t capacidade);
void normalizarChave(const char *origem, char *destino, size_t tam_max);
// calculos
uint64_t valorPolinomial(const char *chave_norm);
size_t h1(const TabelaHashEstoque *tab, const char *chave_norm);
size_t h2(const TabelaHashEstoque *tab, const char *chave_norm);
size_t sondar(const TabelaHashEstoque *tab, size_t idx1, size_t passo_h2, size_t i);

bool inserir(TabelaHashEstoque *tab, const char *chave, Produto valor);
const Produto* buscar(TabelaHashEstoque *tab, const char *chave);
bool atualizar(TabelaHashEstoque *tab, const char *chave, Produto novo_valor);
bool remover(TabelaHashEstoque *tab, const char *chave);

// telemetria e auditoria
void listarRegistros(const TabelaHashEstoque *tab);
void exibirEstrutura(const TabelaHashEstoque *tab);
double fatorCarga(const TabelaHashEstoque *tab);
void zerarContadoresOperacionais(TabelaHashEstoque *tab);

#endif
