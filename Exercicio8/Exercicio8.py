#Pontifícia Universidade Católica 
#Goiânia, 23 de setembro de 2025
#Alunos: Matheus Barbosa Silva, Yan Menezes
#Professor: Fernando Abadia

import threading
import time
import random
import collections

BUFFER_TAMANHO = 10
NUM_PRODUTORES = 2
NUM_CONSUMIDORES = 2
DURACAO_SIMULACAO_SEGUNDOS = 60

BURST_DURACAO_MIN = 2  #aqui serão os segundos
BURST_DURACAO_MAX = 5  
IDLE_DURACAO_MIN = 3   
IDLE_DURACAO_MAX = 8   

# Buffer circular
buffer = collections.deque(maxlen=BUFFER_TAMANHO)
buffer_lock = threading.Lock()
not_full = threading.Condition(buffer_lock)
not_empty = threading.Condition(buffer_lock)
produzidos_total = 0
consumidos_total = 0
ocupacao_historico = []
class Producer(threading.Thread):
    def run(self):
        global produzidos_total, not_full, not_empty, buffer_lock, ocupacao_historico
        while True:
            if random.random() < 0.5: 
                periodo_ocioso = False
                periodo_duracao = random.uniform(BURST_DURACAO_MIN, BURST_DURACAO_MAX)
                intervalo_producao = random.uniform(0.01, 0.1) 
                print(f"Produtor {self.name} - Começando RAJADA por {periodo_duracao:.2f}s.")
            else:
                periodo_ocioso = True
                periodo_duracao = random.uniform(IDLE_DURACAO_MIN, IDLE_DURACAO_MAX)
                intervalo_producao = random.uniform(2, 5) 
                print(f"Produtor {self.name} - Começando OCIOSIDADE por {periodo_duracao:.2f}s.")
            
            tempo_inicio_periodo = time.time()
            
            while time.time() - tempo_inicio_periodo < periodo_duracao:
                with buffer_lock:
                    # Implementação de Backpressure: espera se o buffer estiver cheio.
                    while len(buffer) == BUFFER_TAMANHO:
                        print(f"Produtor {self.name} - BUFFER CHEIO! Aguardando...")
                        not_full.wait()
                
                    # Adiciona item ao buffer
                    item = time.time()
                    buffer.append(item)
                    produzidos_total += 1
                    ocupacao_historico.append(len(buffer)) # Registra a ocupação
                    print(f"Produtor {self.name}: Item {produzidos_total} produzido. Buffer: {len(buffer)}/{BUFFER_TAMANHO}")
                    
                    not_empty.notify() # Notifica ao consumidor que um item está disponível
                
                time.sleep(intervalo_producao)
            if time.time() > tempo_inicio_simulacao + DURACAO_SIMULACAO_SEGUNDOS:
                break
class Consumer(threading.Thread):
    def run(self):
        global consumidos_total
        while True:
            with buffer_lock:
                while len(buffer) == 0:
                    print(f"Consumidor {self.name} - BUFFER VAZIO! Aguardando...")
                    not_empty.wait()
                item = buffer.popleft()
                consumidos_total += 1
                tempo_espera = time.time() - item 
                print(f"Consumidor {self.name}: Item consumido (tempo de espera: {tempo_espera:.4f}s). Buffer: {len(buffer)}/{BUFFER_TAMANHO}")
                not_full.notify()            
            time.sleep(random.uniform(0.1, 0.5))
            if time.time() > tempo_inicio_simulacao + DURACAO_SIMULACAO_SEGUNDOS:
                break
def main():
    global tempo_inicio_simulacao
    tempo_inicio_simulacao = time.time()
    produtores = [Producer(name=f"P{i}") for i in range(NUM_PRODUTORES)]
    consumidores = [Consumer(name=f"C{i}") for i in range(NUM_CONSUMIDORES)]
    for p in produtores:
        p.daemon = True
        p.start()
    for c in consumidores:
        c.daemon = True
        c.start()
    print(f"Iniciando simulação por {DURACAO_SIMULACAO_SEGUNDOS} segundos...")
    time.sleep(DURACAO_SIMULACAO_SEGUNDOS)
    print("\nSimulação concluída. Coletando estatísticas...")    
    throughput_prod = produzidos_total / DURACAO_SIMULACAO_SEGUNDOS
    throughput_cons = consumidos_total / DURACAO_SIMULACAO_SEGUNDOS
    print("\n--- Relatório Final ---")
    print(f"Total de itens produzidos: {produzidos_total}")
    print(f"Total de itens consumidos: {consumidos_total}")
    print(f"Throughput de Produção: {throughput_prod:.2f} itens/segundo")
    print(f"Throughput de Consumo: {throughput_cons:.2f} itens/segundo")
    if ocupacao_historico:
        media_ocupacao = sum(ocupacao_historico) / len(ocupacao_historico)
        ocupacao_max = max(ocupacao_historico)
        print(f"Ocupação média do buffer: {media_ocupacao:.2f} itens")
        print(f"Ocupação máxima do buffer: {ocupacao_max} itens")
if __name__ == "__main__":
    main()
