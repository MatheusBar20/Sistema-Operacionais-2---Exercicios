# Pontifícia Universidade Católica 
# Goiânia, 25 de setembro de 2025
# Alunos: Matheus Barbosa Silva, Yan Menezes
# Professor: Fernando Abadia

# Repositório para a atividade de laboratório da disciplina CMP1075

----------------------------------------------------------------------------------------------------------
 Exercício 05
----------------------------------------------------------------------------------------------------------

# Descrição do Exercício
Este projeto implementa um pool de threads de tamanho fixo (N) para processar uma fila de tarefas de forma concorrente. O objetivo é demonstrar como um grupo de threads de trabalho pode ser reutilizado para executar tarefas de uma fila, otimizando o desempenho e evitando o alto custo de criação e destruição de threads.

A simulação funciona da seguinte forma:

- A thread principal lê inteiros da entrada padrão (stdin).
- Cada inteiro lido é enfileirado como uma tarefa para ser processada.
- Um pool de threads trabalhadoras consome as tarefas da fila, executando uma função de processamento CPU-bound (por exemplo, um teste de primalidade).
- O sistema continua a processar tarefas até que o sinal de Fim de Arquivo (EOF) seja recebido na entrada padrão.
- Ao detectar o EOF, a thread principal inicia um processo de encerramento seguro do pool de threads.

# Compilação e Execução
O código utiliza a biblioteca pthread para o gerenciamento das threads.
Para compilar o programa, use o seguinte comando:

Bash

gcc exercicio5.c -o exercicio5 -pthread
Para executar o programa e fornecer as tarefas a partir de um arquivo ou da entrada padrão:

Para ler de um arquivo:

Bash

./exercicio5 < dados.txt
Para digitar as tarefas manualmente:

Bash

./exercicio5

# Decisões de Sincronização
A integridade da fila de tarefas e o encerramento ordenado do pool são críticos para a correção do sistema. As seguintes primitivas de sincronização foram utilizadas para garantir um comportamento robusto:

- Mutex (pthread_mutex_t): Uma variável mutex protege o acesso à fila de tarefas compartilhada. Ela garante exclusão mútua, permitindo que apenas uma thread por vez (seja a principal enfileirando uma tarefa ou uma trabalhadora desenfileirando uma) acesse a estrutura da fila. Isso previne condições de corrida e corrupção de dados.

- Variáveis de Condição (pthread_cond_t): Utilizadas para otimizar o fluxo de trabalho e evitar o uso de CPU com espera ativa (busy-waiting):

- Uma condição para a fila not_empty: As threads trabalhadoras esperam passivamente nesta condição se a fila de tarefas estiver vazia. A thread principal as acorda quando uma nova tarefa é adicionada.

- Uma condição para a fila not_full: A thread principal esperaria nesta condição se a fila estivesse cheia. Uma thread trabalhadora a acorda quando um espaço é liberado.

- Encerramento Graceful (Poison Pill): Para garantir que todas as tarefas sejam concluídas e as threads finalizem de forma segura, a thread principal envia um "veneno" (Poison Pill) para a fila para cada thread no pool. Ao receber o veneno (um valor especial, como -1), uma thread trabalhadora sabe que deve sair de seu loop de processamento e terminar sua execução. A thread principal então usa pthread_join para aguardar a finalização de todas as threads do pool.

# Análise dos Resultados
A execução do programa prova a correção da implementação de duas maneiras:

- Thread Safety da Fila: A combinação de mutex e variáveis de condição assegura que a fila é thread-safe. Nenhuma operação de adição ou remoção de tarefas pode ocorrer simultaneamente, garantindo a integridade dos dados, independentemente do número de threads.

- Garantia de Não-Perda de Dados: O mecanismo de encerramento com "Poison Pill" e pthread_join garante que o sistema só finalize após todas as threads terem processado suas tarefas designadas e saído de seus loops. Uma contagem final de tarefas concluídas pode ser impressa para provar que o número de tarefas processadas corresponde ao número de tarefas submetidas, garantindo que nenhuma tarefa se perdeu.