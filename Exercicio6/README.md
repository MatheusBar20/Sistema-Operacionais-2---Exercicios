
# Pontifícia Universidade Católica 
# Goiânia, 25 de setembro de 2025
# Alunos: Matheus Barbosa Silva, Yan Menezes
# Professor: Fernando Abadia

# Repositório para a atividade de laboratório da disciplina CMP1075

----------------------------------------------------------------------------------------------------------
 Exercício 06
----------------------------------------------------------------------------------------------------------

# Exercicio

 Leia um arquivo grande de inteiros e calcule (a) soma total (b) histograma de frequências usando P threads em paralelo; particione o arquivo em blocos, faça "map" local em cada thread e "reduce" na principal com exclusão mútua mínima; meça o speedup para P = 1, 2, 4, 8

### Descrição do exercicio
Criamos 2 códigos-fontes: arquivogerador.c e exercicio6.c, no arquivogerador.c, é criado um arquivo que contém os números inteiros para serem processados pelo exercicio6.c.
No exercicio6.c, é feito o processamento de histograma de frequências e a soma total pelas threads. 

### Compilação e execução
É necessário criar um arquivo primeiro, logo em arquivogerador.c, faça:

``````
gcc arquivogerador.c -o arquivogerador.o   #para compilar
./arquivogerador.o <nome_do_arquivo>.bin <quantidade_inteiros>   #para executar
``````
ai vai no exercicio6.c e roda também assim:
````
gcc exercicio6.c -o exercicio6.o    //para compilar
./exercicio6.o <nome_do_arquivo>.bin    //para executar
````
### Decisões de sincronização
A decisão de sincronização utilizada foi o Mutex (exclusão mútua). O mutex foi usado para proteger o acesso a duas variáveis globais que são compartilhadas por todas as threads:
*somaTotal* e *histogramaTotal*.

A solução adota o conceito de **exclusão mútua mínima**, o que significa que o mutex é aplicado apenas na menor porção de código possível. Existem duas fases:
+ **Fase Map**: As threads leem o arquivo e calculam a soma e o histograma localmente sem usar o mutex.
+ **Fase Reduce**: O mutex é travado apenas no momento em que a thread soma seus resultados locais às variáveis globais, e em seguida, é liberado imediatamente.  

### Análise dos resultados

A partir das evidências de execução, é perceptível analisar que quanto maior o número de threads para o processamento, mais rápido o progrema finaliza.

Speedups calculados: 
```
Speedup2 = 0.310768 / 0.197073  1.576918
Speedup4 =  0.310768 / 0.150924 = 2.059102
Speedup8 = 0.310768 / 0.084611 = 3.672903

```