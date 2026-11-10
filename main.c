#include "tabela_hash.h"
#include <stdio.h>

static TabelaHashEstoque estoque;

int main(void) {
    inicializarTabela(&estoque, 17); // E1.1: Capacidade estatica m = 17 (primo), m2 = 13

    Produto p1 = {"SKU-1010", "Microcontrolador STM32U585", 10, 145.00};
    Produto p2 = {"SKU-2020", "Driver Dual H-Bridge DRV8255", 35, 32.50};
    Produto p3 = {"SKU-3030", "Encoder Incremental 600PPR", 8, 94.00};
    Produto p4 = {"SKU-4040", "Motor Coreless 716 3.7V", 50, 12.00};

    inserir(&estoque, p1.codigo, p1);
    inserir(&estoque, p2.codigo, p2);
    inserir(&estoque, p3.codigo, p3);
    inserir(&estoque, p4.codigo, p4);

    printf("=== Demonstracao da Aplicacao A2: Controle de Estoque (C11) ===\n\n");
    listarRegistros(&estoque);
    printf("\n");
    exibirEstrutura(&estoque);

    return 0;
}