# Trabalho LAEDII
Trabalho direcionado a disciplina de Laboratório de Algoritmos e Estruturas de Dados II, ministrada pelo professor Dr. Kennedy Reurison Lopes no período 2026.2, na Universidade Federal Rural do Semi-Árido (UFERSA).

## Autores
- Caio Barreto Meyer - ( 2023011893 ) | [Caio B. Meyer](https://github.com/TaiCaio)
- Carlos Henrique De Queiroz - ( 2025022528 ) | [Carlos Queiroz](https://github.com/CarlossQueiroz)
- Pablo Rubens De Moura - ( 2025022561 ) | [Pablo](https://github.com/pablozin)
___
## Configuração Atribuída
O projeto implementa a configuração estrutural correspondente ao índice **70:**
- **E1.1 (Estrutura):** Vetor estático com capacidade fixa.
- **H6 (Função Hash):** Polinomial (combinação de caracteres com pesos polinomiais).
- **C4 (Tratamento de colisões):** Hash duplo.
- **A2 (Aplicação):** Controle de estoque (Chave: Código do produto | Valores: Nome, quantidade, preço).
---
## Compilação e Execução
O projeto é dividido em três módulos independentes. Utilize os comandos abaixo no terminal Linux/Unix para compilar e executar cada um. As flags ```-std=c11 -Wall -Wextra``` garantem a conformidade e a exibição de todos os alertas de segurança.

**1. Aplicação Principal:**

```bash
gcc -std=c11 -Wall -Wextra main.c tabela_hash.c -o main
./main
```

**2. Testes Obrigatórios:**

```bash
gcc -std=c11 -Wall -Wextra testes.c tabela_hash.c -o testes
./testes
```

**3. Benchmarks:**
*Nota: A flag `-D_POSIX_C_SOURCE=199309L` é necessária para habilitar a medição de tempo em nanossegundos no Linux.*

```bash
gcc -std=c11 -Wall -Wextra -D_POSIX_C_SOURCE=199309L experimentos.c tabela_hash.c -o experimentos
./experimentos
```
---

