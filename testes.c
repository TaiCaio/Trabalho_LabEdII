#include "tabela_hash.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

static TabelaHashEstoque estoque;

int main(void) {
    printf("BATERIA DE TESTES OBRIGATORIOS - CONFIGURACAO 70 (C11)\n");
    printf("(E1.1: Vetor Estatico | H6: Polinomial | C4: Hash Duplo | A2: Estoque)\n\n");
    inicializarTabela(&estoque, 11); // m = 11 (primo), m2 = 7

    // Teste 7.1: Estrutura vazia
    printf("[TESTE 7.1] Busca e remocao em estrutura vazia\n");
    const Produto *res_vazio = buscar(&estoque, "PRD-001");
    bool rem_vazio = remover(&estoque, "PRD-001");
    printf("  Entrada: buscar('PRD-001'), remover('PRD-001')\n");
    printf("  Obtido: %s e %s\n\n", res_vazio == NULL ? "NULL" : "nao-nulo", rem_vazio ? "true" : "false");
    assert(res_vazio == NULL && !rem_vazio);

    // Teste 1: Insercao e busca de chave existente + normalizacao
    printf("[TESTE 1] Insercao e busca de chave existente (com normalizacao)\n");
    Produto p1 = {"prd-100", "Sensor Optico E38", 15, 89.90};
    bool ins1 = inserir(&estoque, "prd-100", p1);
    const Produto *busca1 = buscar(&estoque, "PRD 100");
    printf("  Entrada: inserir('prd-100'), buscar('PRD 100')\n");
    printf("  Obtido: %s e Produto='%s' (chave='%s')\n\n",
           ins1 ? "true" : "false", busca1 ? busca1->nome : "N/A", busca1 ? busca1->codigo : "N/A");
    assert(ins1 && busca1 != NULL && strcmp(busca1->codigo, "PRD100") == 0);

    // Teste 2: Busca de chave inexistente
    printf("[TESTE 2] Busca de chave inexistente\n");
    const Produto *busca_inex = buscar(&estoque, "PRD-999");
    printf("  Entrada: buscar('PRD-999')\n");
    printf("  Obtido: %s\n\n", busca_inex == NULL ? "NULL" : "encontrado");
    assert(busca_inex == NULL);

    // Teste 3: Tentativa de insercao duplicada
    printf("[TESTE 3] Tentativa de insercao duplicada\n");
    size_t col_antes = estoque.cont_colisoes;
    Produto p_dup = {"PRD-100", "Clone Invalido", 99, 1.99};
    bool ins_dup = inserir(&estoque, "PRD-100", p_dup);
    size_t col_depois = estoque.cont_colisoes;
    printf("  Entrada: inserir('PRD-100') novamente\n");
    printf("  Obtido: %s | Colisoes (antes/depois): %zu / %zu\n\n",
           ins_dup ? "true" : "false", col_antes, col_depois);
    assert(!ins_dup && col_antes == col_depois);

    // Teste 6: Atualizacao sem alterar a chave
    printf("[TESTE 6] Atualizacao de registro existente sem alterar a chave\n");
    Produto p_atu = {"prd-100", "Sensor Optico E38S6G5", 25, 95.50};
    bool atu = atualizar(&estoque, "prd-100", p_atu);
    const Produto *busca_atu = buscar(&estoque, "PRD100");
    printf("  Entrada: atualizar('prd-100', Qtd=25, Preco=95.50)\n");
    printf("  Obtido: %s | Qtd=%d, Preco=%.2f\n\n",
           atu ? "true" : "false", busca_atu->quantidade, busca_atu->preco);
    assert(atu && busca_atu->quantidade == 25 && busca_atu->preco == 95.50);

    // Teste 4: Duas ou mais chaves com colisao demonstrada em h1
    printf("[TESTE 4] Insercao de chaves distintas com colisao intencional em h1\n");
    Produto pc1 = {"PRD112", "Driver Motor A4988", 40, 14.50};
    Produto pc2 = {"PRD124", "Motor Passo NEMA17", 12, 68.00};
    bool c_ins1 = inserir(&estoque, "PRD112", pc1);
    bool c_ins2 = inserir(&estoque, "PRD124", pc2);
    printf("  Chave Base : PRD100 -> h1=%zu, h2=%zu\n", h1(&estoque, "PRD100"), h2(&estoque, "PRD100"));
    printf("  Colidente 1: PRD112 -> h1=%zu, h2=%zu\n", h1(&estoque, "PRD112"), h2(&estoque, "PRD112"));
    printf("  Colidente 2: PRD124 -> h1=%zu, h2=%zu\n\n", h1(&estoque, "PRD124"), h2(&estoque, "PRD124"));
    assert(c_ins1 && c_ins2);
    exibirEstrutura(&estoque);
    printf("\n");

    // Teste 5: Remocao em colisao e busca das chaves restantes
    printf("[TESTE 5] Remocao da 1a chave da cadeia de colisao e busca das seguintes\n");
    bool rem_col = remover(&estoque, "PRD100");
    const Produto *b_rem = buscar(&estoque, "PRD100");
    const Produto *b_rest1 = buscar(&estoque, "PRD112");
    const Produto *b_rest2 = buscar(&estoque, "PRD124");
    printf("  Entrada: remover('PRD100'), buscar('PRD100'), buscar('PRD112'), buscar('PRD124')\n");
    printf("  Obtido: removido=%s, PRD100=%s, PRD112=%s, PRD124=%s\n\n",
           rem_col ? "true" : "false", b_rem == NULL ? "NULL" : "erro",
           b_rest1 ? b_rest1->nome : "NULL", b_rest2 ? b_rest2->nome : "NULL");
    assert(rem_col && b_rem == NULL && b_rest1 != NULL && b_rest2 != NULL);
    exibirEstrutura(&estoque);
    printf("\n");

    // Teste 7.2: Preenchimento total da capacidade fixa (E1.1 m=11) e rejeicao de excesso
    printf("[TESTE 7.2] Preenchimento total da capacidade fixa (m=11) e rejeicao de excesso\n");
    int id_extra = 200;
    while (estoque.n < 11) {
        char k[TAM_CODIGO];
        snprintf(k, sizeof(k), "ITEM%d", id_extra++);
        Produto p_extra = {"", "Componente Eletronico", 5, 10.0};
        snprintf(p_extra.codigo, TAM_CODIGO, "%s", k);
        inserir(&estoque, k, p_extra);
    }
    Produto p_exc = {"EXCESSO01", "Item Excedente", 1, 1.0};
    bool ins_cheia = inserir(&estoque, "EXCESSO01", p_exc);
    printf("  Ocupacao atual: n=%zu/%zu\n", estoque.n, estoque.m);
    printf("  Tentativa de inserir 12o item ('EXCESSO01'): Obtido=%s\n\n", ins_cheia ? "true" : "false");
    assert(!ins_cheia && estoque.n == 11);

    listarRegistros(&estoque);
    return 0;
}
