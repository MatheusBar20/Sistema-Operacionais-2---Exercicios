# Pontifícia Universidade Católica 
# Goiânia, 25 de setembro de 2025
# Alunos: Matheus Barbosa Silva, Yan Menezes
# Professor: Fernando Abadia

# Repositório para a atividade de laboratório da disciplina CMP1075

----------------------------------------------------------------------------------------------------------
Exercício 02: Buffer Circular com Produtores e Consumidores
----------------------------------------------------------------------------------------------------------
# Descrição do Exercício
Este projeto implementa um clássico sistema de Produtor-Consumidor utilizando um buffer circular compartilhado. O objetivo principal é demonstrar o uso de primitivas de sincronização para gerenciar a produção e o consumo de dados entre múltiplas threads.

O código foi projetado para:

- Gerar itens com tempos aleatórios.
- Coletar estatísticas de desempenho, como o throughput e o tempo médio de espera das threads.
- Analisar, por meio de experimentos, como o tamanho do buffer afeta o desempenho do sistema.

# Compilação e Execução

O código foi escrito em C e utiliza a biblioteca pthread e semáforos POSIX. Certifique-se de que seu ambiente de compilação suporte essas bibliotecas.

Para compilar o código, utilize o seguinte comando:

Bash

gcc exercicio2.c -o exercicio2 -pthread -lrt
gcc: Compilador padrão para C.

-pthread: Flag para vincular a biblioteca Pthreads.

-lrt: Flag para vincular a biblioteca de tempo real (necessária para os semáforos POSIX).

Para executar o programa, utilize:

Bash

./exercicio2

# Decisões de Sincronização
A sincronização é o ponto central deste exercício, e foi implementada para garantir exclusão mútua e espera ativa zero. As primitivas utilizadas são:

- Mutex (pthread_mutex_t): Usado para proteger a seção crítica do buffer. O mutex garante que apenas uma thread de cada vez (seja produtor ou consumidor) possa acessar e modificar as variáveis de estado do buffer (in, out, count, etc.). Isso previne condições de corrida e garante a integridade dos dados.

- Semáforos (sem_t): Foram utilizados dois semáforos para controle de fluxo e para evitar a espera ativa (busy-waiting):

- sem_empty: Inicializado com o tamanho total do buffer. Representa os espaços vazios disponíveis. Os produtores aguardam neste semáforo quando o buffer está cheio.

- sem_full: Inicializado com 0. Representa os itens cheios no buffer. Os consumidores aguardam neste semáforo quando o buffer está vazio.

Essa abordagem com semáforos é um exemplo de espera passiva e é muito mais eficiente do que a espera ativa, pois as threads bloqueiam e liberam a CPU enquanto aguardam.

# Análise dos Resultados
O experimento demonstra como o tamanho do buffer é um fator crítico para o desempenho do sistema.

- Buffers pequenos (e.g., 5, 10): O throughput tende a ser menor. Produtores e consumidores se bloqueiam com mais frequência, pois o buffer enche ou esvazia rapidamente. Isso aumenta o tempo de espera das threads, criando um gargalo.

- Buffers grandes (e.g., 50, 100): O sistema opera de forma mais suave. Há menos contenção e bloqueio, pois há mais espaço para acomodar as variações nas taxas de produção e consumo. Consequentemente, o throughput aumenta e o tempo de espera das threads diminui.

A análise do experimento mostra que um buffer de tamanho ideal pode equilibrar a pressão dos produtores e a capacidade dos consumidores, otimizando o desempenho geral do sistema.