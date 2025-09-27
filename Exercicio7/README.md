# Pontifícia Universidade Católica 
# Goiânia, 23 de setembro de 2025
# Alunos: Matheus Barbosa Silva, Yan Menezes
# Professor: Fernando Abadia

# Repositório para a atividade de laboratório da disciplina CMP1075

---------------------------------------------------------------------------------------------------------
Exercicio 7
---------------------------------------------------------------------------------------------------------

# Relatório - Problema dos Filósofos Glutões

Este documento explica a implementação em Python do Problema dos Filósofos Glutões, um exemplo clássico de problemas de concorrência e sincronização em sistemas operacionais.

## Descrição do Problema

Cinco filósofos se sentam ao redor de uma mesa redonda. Entre cada par de filósofos, há um garfo. Para comer, um filósofo precisa de dois garfos: um à sua esquerda e um à sua direita. O problema surge porque a aquisição dos garfos pode levar a dois cenários indesejáveis:

* Deadlock (Impasse): Todos os filósofos pegam o garfo à sua esquerda simultaneamente e ficam esperando eternamente pelo garfo à sua direita, que nunca é liberado.
* **Starvation (Inanição): Um ou mais filósofos nunca conseguem pegar os dois garfos necessários para comer, pois os outros filósofos sempre os adquirem primeiro.

Implementamos em Python usando a biblioteca `threading` para criar threads que representam cada filósofo. Os garfos são representados por objetos `threading.Lock()`, que funcionam como mutex (trava de exclusão mútua), garantindo que apenas uma thread por vez possa "pegar" um garfo.

### Solução A: Ordem Global de Aquisição

Esta solução previne o deadlock ao impor uma ordem na qual os garfos devem ser adquiridos. A regra é simples: cada filósofo sempre tenta pegar o garfo com o menor índice primeiro, e só depois o garfo com o maior índice.

Como funciona:
* Um filósofo com ID 0 precisa dos garfos 0 e 1. Ele tenta pegar o garfo 0, depois o garfo 1.
* Um filósofo com ID 4 precisa dos garfos 4 e 0. Ele tenta pegar o garfo 0, depois o garfo 4 (já que 0 < 4).

Dessa forma, a situação de "abraço mortal" (deadlock) é evitada, pois não há uma cadeia circular de espera. A lógica de `acquire(timeout=...)` com um tempo máximo de espera e a liberação do primeiro garfo em caso de falha na aquisição do segundo também ajuda a mitigar o starvation, dando a outros filósofos a chance de pegar os garfos.

### Solução B: Semáforo Limitando Filósofos

Esta solução evita o deadlock ao garantir que no máximo `N-1` filósofos possam tentar pegar os garfos ao mesmo tempo, onde `N` é o número total de filósofos. No nosso caso, com 5 filósofos, o semáforo é inicializado com um valor de `4`.

Funcionamento:
* Um semáforo atua como uma "sala de espera". Antes de tentar pegar qualquer garfo, o filósofo deve adquirir o semáforo.

* Quando o semáforo atinge o valor zero, significa que a capacidade máxima de filósofos na "sala" foi atingida. Qualquer novo filósofo que tente entrar terá que esperar até que um dos que está dentro saia ,ou seja, termine de comer e libere o semáforo.

* Isso garante que sempre haverá pelo menos um filósofo que pode pegar os dois garfos, já que sempre sobrará um garfo livre. Por exemplo, se 4 filósofos pegarem seu garfo esquerdo, o quinto filósofo será impedido de tentar pegar qualquer garfo até que um dos outros termine, o que quebra a cadeia de espera.

## Coleta de Métricas e Mitigação de Starvation

O código armazena métricas para cada filósofo em um dicionário global chamado `metricas_filosofo`. As métricas coletadas são:

* `refeicoes`: O número total de vezes que o filósofo conseguiu comer.
* `maior_espera`: O tempo de espera mais longo (em segundos) que o filósofo precisou para adquirir os dois garfos.

A mitigação do starvation foi implementada nas duas soluções por meio de duas estratégias:

1.  **`acquire` com `timeout`: As chamadas para pegar os garfos (`garfos[...].acquire(...)`) usam um tempo limite. Se um garfo não puder ser adquirido dentro desse tempo, a tentativa falha.
2.  Liberação do Garfo: Se um filósofo consegue pegar o primeiro garfo, mas falha em pegar o segundo, ele libera o primeiro garfo imediatamente. Isso é crucial para evitar que ele fique travado esperando e para dar a oportunidade a outros filósofos de pegá-lo. Após a liberação, a thread "dorme" por um curto período de tempo para evitar uma tentativa imediata, o que ajuda a distribuir a chance de comer de forma mais equitativa.

##imagem do resultado:
![funcionamento do código](<Captura de Tela (71).png>)
