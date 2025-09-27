# Pontifícia Universidade Católica 
# Goiânia, 25 de setembro de 2025
# Alunos: Matheus Barbosa Silva, Yan Menezes
# Professor: Fernando Abadia

# Repositório para a atividade de laboratório da disciplina CMP1075

----------------------------------------------------------------------------------------------------------
 Exercício 03
----------------------------------------------------------------------------------------------------------
# Descrição do Exercício

Este projeto simula um sistema bancário simples, demonstrando o problema de condições de corrida e como a sincronização de threads é essencial para garantir a consistência dos dados.

## O código implementa dois cenários principais:

- Transferências sem locks: Onde múltiplas threads realizam transações simultaneamente, resultando em um saldo global inconsistente.

- Transferências com locks: Onde locks são utilizados para proteger as operações de leitura e escrita, garantindo que o saldo global do sistema permaneça constante, mesmo sob alta concorrência.

A simulação também inclui um teste de estresse com verificações contínuas para provar a robustez da solução com locks.

# Compilação e Execução

O código foi escrito em C e utiliza a biblioteca pthread para o gerenciamento de threads. Para compilar, certifique-se de vincular a biblioteca pthread.

## Para compilar, use o seguinte comando:

Bash

gcc exercicio3.c -o exercicio3 -pthread
Para executar a simulação, utilize:

Bash

./exercicio3

# Decisões de Sincronização

A integridade do saldo global é a métrica principal para avaliar a correção do programa. As seguintes decisões de sincronização foram cruciais:

- Locks por Conta: Cada conta bancária (Account) possui seu próprio mutex (pthread_mutex_t). Isso é uma estratégia eficiente, pois permite que transações entre contas diferentes (e que não compartilham os mesmos locks) ocorram simultaneamente, aumentando o paralelismo.

- Ordem de Aquisição de Locks: Para prevenir deadlocks (impasse), a função transfer_with_locks impõe uma ordem estrita na aquisição dos locks. Os locks são sempre adquiridos em ordem crescente de ID da conta (from->id < to->id). Isso garante que não ocorra espera circular, uma das quatro condições para um deadlock.

- Teste de Inconsistência: O cenário sem locks propositalmente remove a exclusão mútua. A saída do programa demonstrará que, apesar de as transferências individuais parecerem corretas, a soma total dos saldos (global_balance) será diferente do valor inicial, evidenciando uma condição de corrida.

# Análise dos Resultados

A saída do programa mostra claramente a diferença entre os cenários com e sem sincronização.

- Cenário sem Locks: O saldo final diverge do saldo inicial. Isso ocorre porque múltiplas threads podem ler e modificar o saldo de uma mesma conta ao mesmo tempo, resultando em perdas ou ganhos de valores devido a operações de leitura/escrita não atômicas.

- Cenário com Locks: O saldo final é sempre idêntico ao inicial (dentro de uma tolerância mínima de ponto flutuante), independentemente do número de threads e transferências. Isso prova que o uso de mutexes na seção crítica (transfer_with_locks) garante a consistência e a atomicidade das operações, eliminando as condições de corrida.

- Teste de Estresse: Este teste reforça a prova, garantindo que o saldo global permanece constante mesmo durante a execução. As asserts no código falhariam caso uma inconsistência fosse detectada, provando a robustez da implementação com locks.