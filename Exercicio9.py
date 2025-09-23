#Pontifícia Universidade Católica 
#Goiânia, 23 de setembro de 2025
#Alunos: Matheus Barbosa Silva, Yan Menezes
#Professor: Fernando Abadia

import threading
import time
import random

# Variáveis globais
DURACAO_SIMULACAO_SEGUNDOS = 60
numero_de_rodadas = 0
tempo_inicio_simulacao = 0

def corredor(id_equipe, id_corredor, barreira):
    """Função que representa um corredor de uma equipe."""
    global numero_de_rodadas
    
    while True:
        # A simulação termina após o tempo definido
        if time.time() - tempo_inicio_simulacao > DURACAO_SIMULACAO_SEGUNDOS:
            break
            
        print(f"Equipe {id_equipe}, Corredor {id_corredor} está correndo...")
        # Simula o tempo que o corredor leva para completar sua "perna"
        time.sleep(random.uniform(0.01, 0.05))
        
        print(f"Equipe {id_equipe}, Corredor {id_corredor} chegou na barreira.")
        
        try:
            # Espera todos os corredores da equipe chegarem na barreira
            # Se for o último, a barreira é liberada e a nova "perna" começa
            barreira.wait()
            
            # Se a barreira foi liberada, incrementa o contador de rodadas para a primeira thread
            if barreira._state == 0:
                numero_de_rodadas += 1
                
        except threading.BrokenBarrierError:
            # A barreira foi quebrada, a simulação terminou
            break

def executar_simulacao(tamanho_equipe):
    """Função principal para executar a simulação para um tamanho de equipe."""
    global numero_de_rodadas
    global tempo_inicio_simulacao
    
    numero_equipes = 1 # Para simplificar, vamos simular apenas uma equipe por vez
    
    print(f"--- Iniciando simulação com {tamanho_equipe} corredores por equipe ---")
    
    # Reinicia as variáveis para a nova simulação
    numero_de_rodadas = 0
    tempo_inicio_simulacao = time.time()
    
    # Cria a barreira para o tamanho da equipe
    barreira = threading.Barrier(tamanho_equipe)
    
    threads_corredores = []
    
    for i in range(numero_equipes):
        for j in range(tamanho_equipe):
            t = threading.Thread(target=corredor, args=(i, j, barreira))
            threads_corredores.append(t)
            t.daemon = True
            t.start()
            
    # Mantém a thread principal viva até o tempo de simulação terminar
    while time.time() - tempo_inicio_simulacao < DURACAO_SIMULACAO_SEGUNDOS:
        time.sleep(1)
        
    # Quando o tempo acabar, quebra a barreira para encerrar as threads
    try:
        barreira.abort()
    except threading.BrokenBarrierError:
        pass
    
    # Aguarda as threads encerrarem
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