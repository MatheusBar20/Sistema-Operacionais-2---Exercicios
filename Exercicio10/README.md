# Pontifícia Universidade Católica 
# Goiânia, 23 de setembro de 2025
# Alunos: Matheus Barbosa Silva, Yan Menezes
# Professor: Fernando Abadia

# Repositório para a atividade de laboratório da disciplina CMP1075

----------------------------------------------------------------------------------------------------------
  EXERCICIO 10 - Deadlock  Watchdog
----------------------------------------------------------------------------------------------------------

O nosso código, o projeto é uma demonstração prática de um deadlock (impasse) em um ambiente de múltiplas threads e como um mecanismo "watchdog" pode ser usado para detectar a ausência de progresso em um sistema.

O código apresenta a solução para o problema, adotando uma estratégia de ordem total de travamento, que previne o impasse.

## Funcionamento

O nosso código executa dois cenários distintos:

1.  Cenário de Deadlock Intencional: Duas threads tentam adquirir dois recursos (`recurso_A` e `recurso_B`) em ordens diferentes. Uma terceira thread, o **watchdog**, monitora o progresso. Ao detectar que as threads de trabalho pararam de reportar progresso por um tempo determinado, o watchdog emite um alerta e um relatório.

2.  Cenário Corrigido: As mesmas tarefas são executadas novamente, mas desta vez, todas as threads seguem a mesma ordem para adquirir os recursos (primeiro `recurso_A`, depois `recurso_B`). Isso remove a condição de espera circular, evitando o deadlock e permitindo que as tarefas sejam concluídas com sucesso.

![Funcionamento do Código](./Captura%20de%20Tela%20(70).png)

