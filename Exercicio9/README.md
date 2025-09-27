# Pontifícia Universidade Católica 
# Goiânia, 23 de setembro de 2025
# Alunos: Matheus Barbosa Silva, Yan Menezes
# Professor: Fernando Abadia

# Repositório para a atividade de laboratório da disciplina CMP1075

--------------------------------------------------------------------------------------------------------
Exercicio 9
--------------------------------------------------------------------------------------------------------
# Relatório - Corrida de Revezamento com Barreiras

Este documento descreve a simulação de uma corrida de revezamento em Python, utilizando a sincronização de threads para modelar a dinâmica do evento. O objetivo é demonstrar o uso de barreiras (`threading.Barrier`) e analisar o impacto do tamanho da equipe na velocidade de conclusão das "pernas" da corrida.

## Descrição

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

Resultado de saída do código:

É esperado que, à medida que o tamanho da equipe aumenta, o número de rodadas por minuto diminua. Isso ocorre porque o tempo total para uma equipe concluir uma perna é determinado pelo corredor mais lento. Quanto mais corredores, maior a chance de haver um que leve mais tempo, fazendo com que a equipe inteira espere por ele na barreira. 

![Funcionamento do código](<Captura de Tela (72).png>)