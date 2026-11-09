#ifndef PRODUTO_H
#define PRODUTO_H

#define TAM_CODIGO 32
#define TAM_NOME 64

typedef struct {
  char codigo[TAM_CODIGO];
  char nome[TAM_NOME];
  int quantidade;
  double preco;
} Produto;

#endif
