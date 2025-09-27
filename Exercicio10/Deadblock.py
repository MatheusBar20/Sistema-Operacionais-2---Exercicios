#Pontifícia Universidade Católica 
#Goiânia, 23 de setembro de 2025
#Alunos: Matheus Barbosa Silva, Yan Menezes
#Professor: Fernando Abadia

import threading
import time

# =========================================================
# Cenário 1: Propositadamente com Deadlock
# =========================================================

print("="*50)
print("Cenário 1: Propositadamente com Deadlock")
print("="*50 + "\n")

# Recursos compartilhados
recurso_A = threading.Lock()
recurso_B = threading.Lock()

# Dicionário para rastrear o último heartbeat das threads
heartbeats = {}
# Bloqueio para garantir acesso seguro ao dicionário heartbeats
heartbeats_lock = threading.Lock()

def tarefa_thread_1_com_heartbeat():
    nome_thread = "Thread-1"
    print(f"{nome_thread}: Iniciando tarefa...")
    with heartbeats_lock:
        heartbeats[nome_thread] = time.time()
    
    # Adquire o Recurso A
    print(f"{nome_thread}: Tentando adquirir Recurso A...")
    recurso_A.acquire()
    print(f"{nome_thread}: Recurso A adquirido.")
    with heartbeats_lock:
        heartbeats[nome_thread] = time.time()
    
    time.sleep(1) # Simula o processamento
    
    # Adquire o Recurso B
    print(f"{nome_thread}: Tentando adquirir Recurso B...")
    recurso_B.acquire()
    print(f"{nome_thread}: Recurso B adquirido.")
    with heartbeats_lock:
        heartbeats[nome_thread] = time.time()
    
    print(f"{nome_thread}: Tarefa completa.")
    recurso_B.release()
    recurso_A.release()

def tarefa_thread_2_com_heartbeat():
    nome_thread = "Thread-2"
    print(f"{nome_thread}: Iniciando tarefa...")
    with heartbeats_lock:
        heartbeats[nome_thread] = time.time()
    
    # Adquire o Recurso B
    print(f"{nome_thread}: Tentando adquirir Recurso B...")
    recurso_B.acquire()
    print(f"{nome_thread}: Recurso B adquirido.")
    with heartbeats_lock:
        heartbeats[nome_thread] = time.time()
        
    time.sleep(1) # Simula o processamento
    
    # Adquire o Recurso A
    print(f"{nome_thread}: Tentando adquirir Recurso A...")
    recurso_A.acquire()
    print(f"{nome_thread}: Recurso A adquirido.")
    with heartbeats_lock:
        heartbeats[nome_thread] = time.time()
    
    print(f"{nome_thread}: Tarefa completa.")
    recurso_A.release()
    recurso_B.release()

def watchdog_monitor(timeout_segundos):
    print("\nWatchdog: Iniciando monitoramento...")
    while True:
        time.sleep(1)
        agora = time.time()
        suspeitos = []
        with heartbeats_lock:
            for nome_thread, ultimo_beat in heartbeats.items():
                if agora - ultimo_beat > timeout_segundos:
                    suspeitos.append(nome_thread)
        
        if suspeitos:
            print("\n--- Relatório do Watchdog ---")
            print(f"Alerta: Ausência de progresso detectada por mais de {timeout_segundos} segundos!")
            for s in suspeitos:
                print(f" - Thread suspeita: '{s}'")
            print("--- Fim do Relatório ---\n")
            break # Termina o loop após encontrar o deadlock
        else:
            print("Watchdog: Tudo em ordem. Continuar monitorando...")


t1 = threading.Thread(target=tarefa_thread_1_com_heartbeat, name="Thread-1")
t2 = threading.Thread(target=tarefa_thread_2_com_heartbeat, name="Thread-2")
t_watchdog = threading.Thread(target=watchdog_monitor, args=(3,))

t1.start()
t2.start()
t_watchdog.start()

t1.join()
t2.join()
t_watchdog.join()

# =========================================================
# Cenário 2: Corrigido com Ordem Total de Travamento
# =========================================================
# Reiniciando os locks e o dicionário de heartbeats para a correção
recurso_A = threading.Lock()
recurso_B = threading.Lock()
heartbeats.clear()

print("\n" + "="*50)
print("Cenário 2: Corrigido com Ordem Total de Travamento")
print("="*50 + "\n")

def tarefa_thread_corrigida(nome_thread):
    print(f"{nome_thread}: Iniciando tarefa com ordem total...")
    
    # Aquisição de locks na ordem correta: A, depois B
    print(f"{nome_thread}: Tentando adquirir Recurso A...")
    recurso_A.acquire()
    print(f"{nome_thread}: Recurso A adquirido.")
    
    time.sleep(1) # Simula o processamento
    
    print(f"{nome_thread}: Tentando adquirir Recurso B...")
    recurso_B.acquire()
    print(f"{nome_thread}: Recurso B adquirido.")
    
    print(f"{nome_thread}: Tarefa completa. Liberando recursos.")
    recurso_B.release()
    recurso_A.release()

t3 = threading.Thread(target=tarefa_thread_corrigida, args=("Thread-3",))
t4 = threading.Thread(target=tarefa_thread_corrigida, args=("Thread-4",))

t3.start()
t4.start()

t3.join()
t4.join()

print("\nTodas as threads corrigidas completaram suas tarefas sem deadlock.")