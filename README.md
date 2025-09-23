# Sistema-Operacionais-2---Exercicios
Repositório para a atividade de laboratório da disciplina CMP1075

---------------------------------------------------------------------------------------------------------
Exercicio 7
---------------------------------------------------------------------------------------------------------

# Relatório - Problema dos Filósofos Glutões

Este documento explica a implementação em Python do Problema dos Filósofos Glutões, um exemplo clássico de problemas de concorrência e sincronização em sistemas operacionais.

## Descrição do Problema

Cinco filósofos se sentam ao redor de uma mesa redonda. Entre cada par de filósofos, há um garfo. Para comer, um filósofo precisa de dois garfos: um à sua esquerda e um à sua direita. O problema surge porque a aquisição dos garfos pode levar a dois cenários indesejáveis:

* Deadlock (Impasse): Todos os filósofos pegam o garfo à sua esquerda simultaneamente e ficam esperando eternamente pelo garfo à sua direita, que nunca é liberado.
* **Starvation (Inanição): Um ou mais filósofos nunca conseguem pegar os dois garfos necessários para comer, pois os outros filósofos sempre os adquirem primeiro.

A solução é implementada em Python usando a biblioteca `threading` para criar threads que representam cada filósofo. Os garfos são representados por objetos `threading.Lock()`, que funcionam como mutex (trava de exclusão mútua), garantindo que apenas uma thread por vez possa "pegar" um garfo.

## Soluções Implementadas

Foram propostas e implementadas duas soluções para mitigar os problemas de deadlock e starvation.

### Solução A: Ordem Global de Aquisição

Esta solução previne o deadlock ao impor uma ordem na qual os garfos devem ser adquiridos. A regra é simples: cada filósofo sempre tenta pegar o garfo com o menor índice primeiro, e só depois o garfo com o maior índice.

Como funciona:
* Um filósofo com ID 0 precisa dos garfos 0 e 1. Ele tenta pegar o garfo 0, depois o garfo 1.
* Um filósofo com ID 4 precisa dos garfos 4 e 0. Ele tenta pegar o garfo 0, depois o garfo 4 (já que 0 < 4).

Dessa forma, a situação de "abraço mortal" (deadlock) é evitada, pois não há uma cadeia circular de espera. A lógica de `acquire(timeout=...)` com um tempo máximo de espera e a liberação do primeiro garfo em caso de falha na aquisição do segundo também ajuda a mitigar o starvation, dando a outros filósofos a chance de pegar os garfos.

### Solução B: Semáforo Limitando Filósofos

Esta solução evita o deadlock ao garantir que no máximo `N-1` filósofos possam tentar pegar os garfos ao mesmo tempo, onde `N` é o número total de filósofos. No nosso caso, com 5 filósofos, o semáforo é inicializado com um valor de `4`.

Como funciona:
* Um semáforo atua como uma "sala de espera". Antes de tentar pegar qualquer garfo, o filósofo deve adquirir o semáforo.
* Quando o semáforo atinge o valor zero, significa que a capacidade máxima de filósofos na "sala" foi atingida. Qualquer novo filósofo que tente entrar terá que esperar até que um dos que está dentro saia (ou seja, termine de comer e libere o semáforo).
* Isso garante que sempre haverá pelo menos um filósofo que pode pegar os dois garfos, já que sempre sobrará um garfo livre. Por exemplo, se 4 filósofos pegarem seu garfo esquerdo, o quinto filósofo será impedido de tentar pegar qualquer garfo até que um dos outros termine, o que quebra a cadeia de espera.

## Coleta de Métricas e Mitigação de Starvation

O código armazena métricas para cada filósofo em um dicionário global chamado `metricas_filosofo`. As métricas coletadas são:

* `refeicoes`: O número total de vezes que o filósofo conseguiu comer.
* `maior_espera`: O tempo de espera mais longo (em segundos) que o filósofo precisou para adquirir os dois garfos.

A mitigação do starvation foi implementada nas duas soluções por meio de duas estratégias:

1.  **`acquire` com `timeout`: As chamadas para pegar os garfos (`garfos[...].acquire(...)`) usam um tempo limite. Se um garfo não puder ser adquirido dentro desse tempo, a tentativa falha.
2.  Liberação do Garfo: Se um filósofo consegue pegar o primeiro garfo, mas falha em pegar o segundo, ele libera o primeiro garfo imediatamente. Isso é crucial para evitar que ele fique travado esperando e para dar a oportunidade a outros filósofos de pegá-lo. Após a liberação, a thread "dorme" por um curto período de tempo para evitar uma tentativa imediata, o que ajuda a distribuir a chance de comer de forma mais equitativa.
--------------------------------------------------------------------------------------------------------
Exercicio 9
--------------------------------------------------------------------------------------------------------
# Relatório - Corrida de Revezamento com Barreiras

Este documento descreve a simulação de uma corrida de revezamento em Python, utilizando a sincronização de threads para modelar a dinâmica do evento. O objetivo é demonstrar o uso de barreiras (`threading.Barrier`) e analisar o impacto do tamanho da equipe na velocidade de conclusão das "pernas" da corrida.

## Descrição do Problema

Uma corrida de revezamento é composta por várias "pernas". Uma equipe só pode começar a próxima perna quando todos os seus corredores da perna anterior chegarem a um ponto de encontro. Esse ponto de encontro funciona como uma barreira de sincronização: nenhuma thread pode prosseguir até que todas as outras threads daquele grupo a tenham alcançado.

O desafio aqui é modelar esse comportamento de forma eficiente, garantindo que o progresso da equipe dependa da conclusão do trabalho de todos os seus membros.

## Implementação da Barreira

A biblioteca `threading` oferece uma solução elegante para esse problema: `threading.Barrier`.

* A barreira É um ponto de sincronização onde um grupo de threads deve esperar até que todas as threads do grupo atinjam o ponto. O construtor `threading.Barrier(parties)` recebe o número de threads que devem esperar na barreira.

* Eu usei o no código:
    1.  Uma barreira é criada com o tamanho da equipe (`threading.Barrier(tamanho_equipe)`).
    2.  Cada thread (corredor) executa sua tarefa (simulada por `time.sleep()`).
    3.  Ao "chegar ao ponto de encontro", a thread chama o método `barreira.wait()`.
    4.  Essa chamada bloqueia a thread até que o número necessário de threads (definido em `parties`) também chame `wait()`.
    5.  Quando todas as threads chegam, a barreira é liberada e todas as threads são desbloqueadas simultaneamente para começar a próxima perna da corrida.

A simulação é executada por um tempo fixo de 60 segundos. Após esse tempo, a barreira é "quebrada" usando `barreira.abort()`, o que libera todas as threads bloqueadas e permite que elas encerrem suas execuções de forma segura.

## Coleta de Métricas e Análise

O código mede a velocidade da equipe em "rodadas por minuto". Uma "rodada" corresponde à conclusão de uma perna da corrida. Para cada tamanho de equipe, o programa:

1.  Inicia a simulação e o cronômetro.
2.  Mantém um contador global (`numero_de_rodadas`).
3.  A cada vez que a barreira é liberada (e, portanto, uma nova perna começa), o contador é incrementado.
4.  Ao final do tempo de simulação, o número total de rodadas é dividido pelo tempo total e multiplicado por 60 para obter o valor por minuto.

Resultados Esperados:

É esperado que, à medida que o tamanho da equipe aumenta, o número de rodadas por minuto diminua. Isso ocorre porque o tempo total para uma equipe concluir uma perna é determinado pelo corredor mais lento. Quanto mais corredores, maior a chance de haver um que leve mais tempo, fazendo com que a equipe inteira espere por ele na barreira. Este é um exemplo clássico da "lei dos retornos decrescentes" em sistemas paralelos, onde a adição de mais trabalhadores (threads) nem sempre resulta em um aumento linear da produtividade.