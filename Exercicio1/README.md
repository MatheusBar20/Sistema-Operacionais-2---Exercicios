# Pontifícia Universidade Católica 
# Goiânia, 25 de setembro de 2025
# Alunos: Matheus Barbosa Silva, Yan Menezes
# Professor: Fernando Abadia

# Repositório para a atividade de laboratório da disciplina CMP1075

----------------------------------------------------------------------------------------------------------
 Exercício 01
----------------------------------------------------------------------------------------------------------

# Descrição do Exercício
Este projeto implementa uma simulação de uma corrida de cavalos usando Pthreads em C. Cada cavalo é representado por uma thread, que avança sua posição de forma independente, simulando a corrida. A thread principal gerencia a largada, exibe o placar em tempo real e determina o resultado final. A corrida é interativa, permitindo que o usuário faça uma aposta no cavalo vencedor.

A implementação foca em problemas de sincronização e concorrência, garantindo que a largada seja simultânea e que o placar da corrida seja atualizado de forma segura, sem condições de corrida.

## Compilação e Execução
O código utiliza a biblioteca pthread, que deve ser incluída durante a compilação.
Para compilar o código, utilize o seguinte comando:

## Bash

gcc exercicio1.c -o exercicio1.o -pthread  
Em seguida, para executar a simulação, utilize o comando:

## Bash

./exercicio1.o
# Decisões de Sincronização

### Para garantir a correção e a segurança da simulação, foram utilizadas as seguintes primitivas de sincronização:

- Mutex (pthread_mutex_t): Usado para garantir exclusão mútua ao acessar e modificar variáveis globais compartilhadas. O mutex é travado na função atualizar_placar, que é a seção crítica, para que apenas uma thread por vez possa alterar as posições dos cavalos e as informações de colocação. Isso previne condições de corrida e garante a integridade dos dados do placar.

- Variável de Condição (pthread_cond_t): Utilizada para coordenar a largada. As threads dos cavalos são criadas, mas elas não começam a correr imediatamente. Elas aguardam na linha de largada (pthread_cond_wait) até que a thread principal, após uma contagem regressiva, use a função pthread_cond_broadcast para notificar todas as threads simultaneamente, garantindo uma largada sincronizada.

# Análise dos Resultados
O uso de threads permite que cada cavalo corra de forma concorrente, com sua própria lógica de avanço, simulando um ambiente realista. O placar em tempo real demonstra a imprevisibilidade da corrida, pois a ordem dos cavalos pode mudar a cada exibição.

A sincronização com pthread_mutex e pthread_cond foi fundamental para:

- Largada Sincronizada: Todos os cavalos iniciam a corrida no mesmo instante, eliminando a vantagem do primeiro cavalo criado.

- Integridade do Placar: O travamento na atualização do placar garante que, mesmo se dois ou mais cavalos terminarem em tempos muito próximos, o sistema atribua as posições de forma correta e sem erros, evitando que a mesma colocação seja atribuída a mais de um cavalo.

O programa demonstra de forma visual e interativa a necessidade e o comportamento das primitivas de sincronização em um cenário multi-thread.