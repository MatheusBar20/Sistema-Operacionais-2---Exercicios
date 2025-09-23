#Pontifícia Universidade Católica 
#Goiânia, 23 de setembro de 2025
#Alunos: Matheus Barbosa Silva, Yan Menezes
#Professor: Fernando Abadia

import threading
import time
import random

# Variáveis globais
NUMERO_FILOSOFOS = 5
TEMPO_MAXIMO_ESPERA = 0.5  # Tempo máximo para esperar por um garfo

# Mutex para os garfos
garfos = [threading.Lock() for _ in range(NUMERO_FILOSOFOS)]

# Metricas
metricas_filosofo = [{'refeicoes': 0, 'maior_espera': 0} for _ in range(NUMERO_FILOSOFOS)]

def filosofo(id_filosofo, solucao):
    """Função que representa o comportamento de um filósofo."""
    
    # Índices dos garfos
    garfo_esquerda = id_filosofo
    garfo_direita = (id_filosofo + 1) % NUMERO_FILOSOFOS
    
    # Variavel para controlar o tempo de espera
    tempo_inicio_espera = 0
    
    while True:
        # Filosofo pensa
        print(f"Filósofo {id_filosofo} está pensando.")
        time.sleep(random.uniform(0.1, 0.5))
        
        # Inicia a tentativa de comer
        print(f"Filósofo {id_filosofo} está com fome.")
        
        # Lógica de acordo com a solução escolhida
        if solucao == "ordem_global":
            # Solução A: Ordem global de aquisição
            # Sempre pega o garfo com o menor índice primeiro para evitar deadlock
            if garfo_esquerda < garfo_direita:
                primeiro_garfo = garfo_esquerda
                segundo_garfo = garfo_direita
            else:
                primeiro_garfo = garfo_direita
                segundo_garfo = garfo_esquerda
                
            tempo_inicio_espera = time.time()
            if garfos[primeiro_garfo].acquire(timeout=TEMPO_MAXIMO_ESPERA):
                if garfos[segundo_garfo].acquire(timeout=TEMPO_MAXIMO_ESPERA):
                    # Garfos adquiridos
                    tempo_espera = time.time() - tempo_inicio_espera
                    if tempo_espera > metricas_filosofo[id_filosofo]['maior_espera']:
                        metricas_filosofo[id_filosofo]['maior_espera'] = tempo_espera
                    
                    comer(id_filosofo, garfo_esquerda, garfo_direita)
                    
                    garfos[segundo_garfo].release()
                    garfos[primeiro_garfo].release()
                else:
                    # Falhou em pegar o segundo garfo, libera o primeiro para evitar starvation
                    print(f"Filósofo {id_filosofo} não conseguiu pegar o segundo garfo. Liberando o primeiro.")
                    garfos[primeiro_garfo].release()
                    time.sleep(random.uniform(0.1, 0.3)) # Pequena espera antes de tentar novamente
            else:
                # Falhou em pegar o primeiro garfo
                time.sleep(random.uniform(0.1, 0.3))

        elif solucao == "semaforo":
            # Solução B: Semáforo limitando filosofos
            with semaforo:
                tempo_inicio_espera = time.time()
                if garfos[garfo_esquerda].acquire(timeout=TEMPO_MAXIMO_ESPERA):
                    if garfos[garfo_direita].acquire(timeout=TEMPO_MAXIMO_ESPERA):
                        # Garfos adquiridos
                        tempo_espera = time.time() - tempo_inicio_espera
                        if tempo_espera > metricas_filosofo[id_filosofo]['maior_espera']:
                            metricas_filosofo[id_filosofo]['maior_espera'] = tempo_espera
                            
                        comer(id_filosofo, garfo_esquerda, garfo_direita)
                        
                        garfos[garfo_direita].release()
                        garfos[garfo_esquerda].release()
                    else:
                        # Falhou em pegar o segundo garfo, libera o primeiro para evitar starvation
                        print(f"Filósofo {id_filosofo} não conseguiu pegar o segundo garfo. Liberando o primeiro.")
                        garfos[garfo_esquerda].release()
                        time.sleep(random.uniform(0.1, 0.3))
                else:
                    # Falhou em pegar o primeiro garfo
                    time.sleep(random.uniform(0.1, 0.3))
                        
def comer(id_filosofo, garfo_esquerda, garfo_direita):
    """Função que simula a ação de comer."""
    print(f"Filósofo {id_filosofo} pegou os garfos {garfo_esquerda} e {garfo_direita} e está comendo.")
    time.sleep(random.uniform(0.1, 0.5))
    print(f"Filósofo {id_filosofo} terminou de comer e vai soltar os garfos.")
    
    # Atualiza as métricas
    metricas_filosofo[id_filosofo]['refeicoes'] += 1

def executar_solucao(solucao):
    """Função principal para executar a simulação."""
    global semaforo
    
    print(f"Iniciando a simulação com a solução: {solucao.upper()}")
    
    for i in range(NUMERO_FILOSOFOS):
        metricas_filosofo[i] = {'refeicoes': 0, 'maior_espera': 0}
        
    threads_filosofos = []
    
    if solucao == "semaforo":
        semaforo = threading.Semaphore(NUMERO_FILOSOFOS - 1)
        
    for i in range(NUMERO_FILOSOFOS):
        t = threading.Thread(target=filosofo, args=(i, solucao))
        threads_filosofos.append(t)
        t.daemon = True # Permite que o programa termine mesmo com as threads rodando
        t.start()
        
    # Executa a simulação por um tempo limitado
    time.sleep(10)
    
    print("\nSimulação encerrada. Coletando métricas...")
    
    for i in range(NUMERO_FILOSOFOS):
        print(f"\n--- Métricas do Filósofo {i} ---")
        print(f"Refeições: {metricas_filosofo[i]['refeicoes']}")
        print(f"Maior tempo de espera (s): {metricas_filosofo[i]['maior_espera']:.4f}")

if __name__ == "__main__":
    executar_solucao("ordem_global")
    print("\n" + "="*50 + "\n")
    executar_solucao("semaforo")