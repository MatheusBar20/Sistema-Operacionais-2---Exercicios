# Pontifícia Universidade Católica 
# Goiânia, 26 de setembro de 2025
# Alunos: Matheus Barbosa Silva, Yan Menezes
# Professor: Fernando Abadia

# Repositório para a atividade de laboratório da disciplina CMP1075

----------------------------------------------------------------------------------------------------------
EXERCICIO 8
----------------------------------------------------------------------------------------------------------

# Buffer Circular com Rajadas e Backpressure

Este documento falaremos sobre como simula um sistema de produtor-consumidor usando um **buffer circular** para demonstrar o gerenciamento de carga de trabalho dinâmica e a aplicação de **backpressure**.

A simulação ilustra como o sistema reage a períodos de alta produção (rajadas) e ociosidade, mantendo a estabilidade através da sincronização de threads.

![Saida do programa](<Captura de Tela (73).png>)

## Funcionalidades

- Buffer Circular: Implementação de uma fila de tamanho fixo para armazenar itens.
- Múltiplos Produtores e Consumidores: Threads que interagem com o buffer simultaneamente.
- Sincronização Avançada: Uso de `threading.Condition` e `threading.Lock` para garantir a exclusão mútua e evitar o uso de CPU (espera ativa ou *busy-waiting*).
- Simulação de Carga Dinâmica: Os produtores operam em **rajadas** (produção rápida) e períodos de ociosidade (produção lenta).
- Backpressure: Mecanismo que força os produtores a pausarem sua produção quando o buffer atinge a capacidade máxima, evitando sobrecarga no sistema.
- Coleta de Estatísticas: O programa registra o throughput (taxa de produção/consumo) e a ocupação do buffer para análise de desempenho e estabilidade.

Ao final da execução, o relatório exibirá métricas importantes para avaliar o sistema:

- Throughput: A taxa de itens processados por segundo. A estabilidade do sistema é demonstrada quando o throughput de produção se aproxima do throughput de consumo.

- Ocupação do Buffer: A ocupação média e a ocupação máxima do buffer indicam a pressão exercida pelos produtores. Se a ocupação máxima for igual ao tamanho do buffer, o backpressure foi ativado, mostrando que o sistema se defendeu contra a sobrecarga.
