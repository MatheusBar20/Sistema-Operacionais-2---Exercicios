#Pontifícia Universidade Católica 
#Goiânia, 23 de setembro de 2025
#Alunos: Matheus Barbosa Silva, Yan Menezes
#Professor: Fernando Abadia

import threading
import time
import random

DURACAO_SIMULACAO_SEGUNDOS = 60
numero_de_rodadas = 0
tempo_inicio_simulacao = 0

def corredor(id_equipe, id_corredor, barreira):
    global numero_de_rodadas    
    while True:
        if time.time() - tempo_inicio_simulacao > DURACAO_SIMULACAO_SEGUNDOS:
            break            
        print(f"Equipe {id_equipe}, Corredor {id_corredor} está correndo...")
        time.sleep(random.uniform(0.01, 0.05))        
        print(f"Equipe {id_equipe}, Corredor {id_corredor} chegou na barreira.")
        try:
            barreira.wait()            
            if barreira._state == 0:
                numero_de_rodadas += 1                
        except threading.BrokenBarrierError:
            break
def executar_simulacao(tamanho_equipe):
    global numero_de_rodadas
    global tempo_inicio_simulacao    
    numero_equipes = 1 
    print(f"--- Iniciando simulação com {tamanho_equipe} corredores por equipe ---")
    numero_de_rodadas = 0
    tempo_inicio_simulacao = time.time()
    barreira = threading.Barrier(tamanho_equipe)
    threads_corredores = []
    for i in range(numero_equipes):
        for j in range(tamanho_equipe):
            t = threading.Thread(target=corredor, args=(i, j, barreira))
            threads_corredores.append(t)
            t.daemon = True
            t.start()            
    while time.time() - tempo_inicio_simulacao < DURACAO_SIMULACAO_SEGUNDOS:
        time.sleep(1)        
    try:
        barreira.abort()
    except threading.BrokenBarrierError:
        pass    
    for t in threads_corredores:
        t.join(timeout=1)
    tempo_total = time.time() - tempo_inicio_simulacao
    rodadas_por_minuto = (numero_de_rodadas / tempo_total) * 60
    print(f"\nResultados para {tamanho_equipe} corredores:")
    print(f"Rodadas concluídas: {numero_de_rodadas}")
    print(f"Rodadas por minuto: {rodadas_por_minuto:.2f}\n")
if __name__ == "__main__":
    tamanhos_equipe_a_testar = [2, 4, 8, 16]
    for tamanho in tamanhos_equipe_a_testar:
        executar_simulacao(tamanho)
        print("="*50)