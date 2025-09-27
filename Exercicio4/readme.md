# Pontifícia Universidade Católica
# Goiânia, 25 de setembro de 2025
# Alunos: Matheus Barbosa Silva, Yan Menezes
# Professor: Fernando Abadia

# Repositório para a atividade de laboratório da disciplina Sistemas Operacionais

Exercício 4: Pipeline de Threads com Múltiplos Estágios

# Descrição do Exercício
Este projeto implementa uma arquitetura de pipeline concorrente em C, onde três threads trabalham em estágios sequenciais para processar uma série de dados. O objetivo é demonstrar como um fluxo de trabalho pode ser dividido entre threads para melhorar o desempenho e a modularidade.

A arquitetura do pipeline é a seguinte:

- Thread de Captura: Produz itens e os coloca em uma fila de saída.

- Thread de Processamento: Consome itens da fila de captura, processa-os (dobrando seu valor) e os coloca em uma nova fila de saída.

- Thread de Gravação: Consome os itens processados e simula a gravação final.

Para um encerramento seguro e ordenado do pipeline, é utilizada a técnica de "Poison Pill", onde um valor especial (-1) é enviado pela primeira thread para sinalizar o fim do fluxo de trabalho.

# Compilação e Execução

O código utiliza a biblioteca pthread para o gerenciamento das threads. É necessário vincular essa biblioteca durante a compilação.

Para compilar, utilize o seguinte comando:

Bash

gcc exercicio4.c -o exercicio4 -pthread
Para executar o programa:

Bash

./exercicio4
A saída do console mostrará o progresso de cada thread, o fluxo de dados entre os estágios e a finalização de cada componente do pipeline.

# Decisões de Sincronização

- A sincronização é crucial para o funcionamento correto do pipeline, evitando condições de corrida e garantindo que as threads não esperem ativamente (busy-waiting). As seguintes primitivas de sincronização foram utilizadas:

- Mutex (pthread_mutex_t): Cada fila (Queue) possui seu próprio mutex, que garante exclusão mútua ao adicionar ou remover itens. Isso impede que duas threads acessem a fila simultaneamente, protegendo a integridade dos dados e do estado da fila (count, front, rear).

- Variáveis de Condição (pthread_cond_t): Utilizadas para otimizar o fluxo de trabalho e evitar a espera ativa.

    - not_empty: Uma thread consumidora (Processamento ou Gravação) que encontra a fila vazia espera passivamente por essa condição. A thread produtora sinaliza (pthread_cond_signal) a condição quando um novo item é adicionado.

    - not_full: Uma thread produtora (Captura ou Processamento) que encontra a fila cheia espera passivamente por essa condição. A thread consumidora sinaliza a condição quando um item é removido e espaço é liberado.

# Análise dos Resultados

- A simulação demonstra um modelo de produção-consumo encadeado. O pipeline funciona de forma fluida, com cada thread atuando de maneira independente e concorrente. As filas de comunicação desacoplam os estágios, permitindo que cada um trabalhe em seu próprio ritmo sem esperar diretamente pelo próximo.

- O uso de Poison Pill é uma solução elegante para encerrar o pipeline. Em vez de forçar o término das threads, o "veneno" viaja pelas filas, garantindo que todos os itens sejam processados até o fim antes que as threads se encerrem de forma segura e limpa. Isso é essencial para prevenir perda de dados em cenários reais.