// Pontifícia Universidade Católica 
// Goiânia, 25 de setembro de 2025
// Alunos: Matheus Barbosa Silva, Yan Menezes
// Professor: Fernando Abadia

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define QUEUE_SIZE 10
#define NUM_ITEMS 20
#define POISON_PILL -1


typedef struct {
    int id;
    int data;
    char processed_data[64];
} DataItem;


typedef struct {
    DataItem* items[QUEUE_SIZE];
    int front;
    int rear;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} ThreadSafeQueue;


typedef struct {
    ThreadSafeQueue* capture_to_process;
    ThreadSafeQueue* process_to_write;
    int items_processed;
    int items_written;
    pthread_mutex_t stats_lock;
    bool shutdown;
} Pipeline;


ThreadSafeQueue* queue_create() {
    ThreadSafeQueue* queue = malloc(sizeof(ThreadSafeQueue));
    if (!queue) return NULL;
    
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
    
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
    
    return queue;
}


void queue_destroy(ThreadSafeQueue* queue) {
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
    free(queue);
}


bool queue_enqueue(ThreadSafeQueue* queue, DataItem* item) {
    pthread_mutex_lock(&queue->lock);
    
    while (queue->count == QUEUE_SIZE) {
        pthread_cond_wait(&queue->not_full, &queue->lock);
    }
    
    queue->items[queue->rear] = item;
    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
    queue->count++;
    
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->lock);
    
    return true;
}


DataItem* queue_dequeue(ThreadSafeQueue* queue) {
    pthread_mutex_lock(&queue->lock);
    
    while (queue->count == 0) {
        pthread_cond_wait(&queue->not_empty, &queue->lock);
    }
    
    DataItem* item = queue->items[queue->front];
    queue->front = (queue->front + 1) % QUEUE_SIZE;
    queue->count--;
    
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->lock);
    
    return item;
}


DataItem* create_data_item(int id, int data) {
    DataItem* item = malloc(sizeof(DataItem));
    if (!item) return NULL;
    
    item->id = id;
    item->data = data;
    item->processed_data[0] = '\0';
    
    return item;
}


void* capture_thread(void* arg) {
    Pipeline* pipeline = (Pipeline*)arg;
    
    printf("Thread CAPTURA iniciada\n");
    
    for (int i = 1; i <= NUM_ITEMS; i++) {
        
        usleep(100000 + (rand() % 100000)); 
        
        DataItem* item = create_data_item(i, rand() % 1000);
        if (!item) continue;
        
        printf("CAPTURA: Item %d capturado (dado: %d)\n", item->id, item->data);
        
        
        queue_enqueue(pipeline->capture_to_process, item);
        
        pthread_mutex_lock(&pipeline->stats_lock);
        printf("CAPTURA -> PROCESSAMENTO: Fila tem %d itens\n", 
               pipeline->capture_to_process->count);
        pthread_mutex_unlock(&pipeline->stats_lock);
    }
    
    
    DataItem* poison_pill = create_data_item(POISON_PILL, POISON_PILL);
    queue_enqueue(pipeline->capture_to_process, poison_pill);
    
    printf("CAPTURA: Poison pill enviada para processamento\n");
    printf("Thread CAPTURA finalizada\n");
    
    return NULL;
}


void* process_thread(void* arg) {
    Pipeline* pipeline = (Pipeline*)arg;
    
    printf("Thread PROCESSAMENTO iniciada\n");
    
    while (true) {
        
        DataItem* item = queue_dequeue(pipeline->capture_to_process);
        
        
        if (item->id == POISON_PILL) {
            printf("PROCESSAMENTO: Recebida poison pill, encaminhando...\n");
            queue_enqueue(pipeline->process_to_write, item);
            break;
        }
        
        
        usleep(150000 + (rand() % 150000)); 
        
        
        snprintf(item->processed_data, sizeof(item->processed_data),
                "PROCESSADO(%d->%d)", item->data, item->data * 2);
        
        printf("PROCESSAMENTO: Item %d processado: %s\n", 
               item->id, item->processed_data);
        
        pthread_mutex_lock(&pipeline->stats_lock);
        pipeline->items_processed++;
        printf("PROCESSAMENTO -> GRAVAÇÃO: Fila tem %d itens\n", 
               pipeline->process_to_write->count);
        pthread_mutex_unlock(&pipeline->stats_lock);
        
        
        queue_enqueue(pipeline->process_to_write, item);
    }
    
    printf("Thread PROCESSAMENTO finalizada\n");
    return NULL;
}


void* write_thread(void* arg) {
    Pipeline* pipeline = (Pipeline*)arg;
    
    printf("Thread GRAVAÇÃO iniciada\n");
    
    while (true) {
        
        DataItem* item = queue_dequeue(pipeline->process_to_write);
        
        
        if (item->id == POISON_PILL) {
            printf("GRAVAÇÃO: Recebida poison pill, finalizando...\n");
            free(item);
            break;
        }
        
        
        usleep(80000 + (rand() % 80000)); 
        
        printf("GRAVAÇÃO: Item %d gravado: %s\n", 
               item->id, item->processed_data);
        
        pthread_mutex_lock(&pipeline->stats_lock);
        pipeline->items_written++;
        pthread_mutex_unlock(&pipeline->stats_lock);
        
        
        free(item);
    }
    
    printf("Thread GRAVAÇÃO finalizada\n");
    return NULL;
}


Pipeline* pipeline_create() {
    Pipeline* pipeline = malloc(sizeof(Pipeline));
    if (!pipeline) return NULL;
    
    pipeline->capture_to_process = queue_create();
    pipeline->process_to_write = queue_create();
    
    if (!pipeline->capture_to_process || !pipeline->process_to_write) {
        if (pipeline->capture_to_process) queue_destroy(pipeline->capture_to_process);
        if (pipeline->process_to_write) queue_destroy(pipeline->process_to_write);
        free(pipeline);
        return NULL;
    }
    
    pipeline->items_processed = 0;
    pipeline->items_written = 0;
    pipeline->shutdown = false;
    pthread_mutex_init(&pipeline->stats_lock, NULL);
    
    return pipeline;
}


void pipeline_destroy(Pipeline* pipeline) {
    
    printf("Verificando itens restantes nas filas...\n");
    
    pthread_mutex_lock(&pipeline->capture_to_process->lock);
    int remaining_capture = pipeline->capture_to_process->count;
    pthread_mutex_unlock(&pipeline->capture_to_process->lock);
    
    pthread_mutex_lock(&pipeline->process_to_write->lock);
    int remaining_process = pipeline->process_to_write->count;
    pthread_mutex_unlock(&pipeline->process_to_write->lock);
    
    printf("Itens restantes na fila captura->processamento: %d\n", remaining_capture);
    printf("Itens restantes na fila processamento->gravação: %d\n", remaining_process);
    
    if (remaining_capture > 0 || remaining_process > 0) {
        printf("ERRO: Itens perdidos no pipeline!\n");
    } else {
        printf("✓ Todos os itens foram processados corretamente\n");
    }
    
    queue_destroy(pipeline->capture_to_process);
    queue_destroy(pipeline->process_to_write);
    pthread_mutex_destroy(&pipeline->stats_lock);
    free(pipeline);
}


void* deadlock_detector(void* arg) {
    Pipeline* pipeline = (Pipeline*)arg;
    
    for (int i = 0; i < 30; i++) { 
        sleep(1);
        
        pthread_mutex_lock(&pipeline->stats_lock);
        int processed = pipeline->items_processed;
        int written = pipeline->items_written;
        pthread_mutex_unlock(&pipeline->stats_lock);
        
        printf("DETECTOR: Processados: %d, Gravados: %d\n", processed, written);
        
        
        static int last_processed = 0;
        static int last_written = 0;
        static int no_progress_count = 0;
        
        if (processed == last_processed && written == last_written) {
            no_progress_count++;
            if (no_progress_count >= 5) {
                printf("ALERTA: Possível deadlock detectado após %d segundos sem progresso!\n", 
                       no_progress_count);
                
            }
        } else {
            no_progress_count = 0;
            last_processed = processed;
            last_written = written;
        }
    }
    
    return NULL;
}

int main() {
    srand(time(NULL));
    
    printf("=== LINHA DE PROCESSAMENTO COM 3 THREADS ===\n");
    printf("Itens a processar: %d\n", NUM_ITEMS);
    printf("Tamanho das filas: %d\n", QUEUE_SIZE);
    printf("===========================================\n");
    
    
    Pipeline* pipeline = pipeline_create();
    if (!pipeline) {
        fprintf(stderr, "Erro ao criar pipeline\n");
        return 1;
    }
    
    pthread_t capture_tid, process_tid, write_tid, detector_tid;
    
    
    pthread_create(&detector_tid, NULL, deadlock_detector, pipeline);
    
    
    if (pthread_create(&capture_tid, NULL, capture_thread, pipeline) != 0 ||
        pthread_create(&process_tid, NULL, process_thread, pipeline) != 0 ||
        pthread_create(&write_tid, NULL, write_thread, pipeline) != 0) {
        
        fprintf(stderr, "Erro ao criar threads\n");
        pipeline_destroy(pipeline);
        return 1;
    }
    
    
    pthread_join(capture_tid, NULL);
    pthread_join(process_tid, NULL);
    pthread_join(write_tid, NULL);
    
    
    pthread_cancel(detector_tid);
    pthread_join(detector_tid, NULL);
    
    
    printf("\n=== ESTATÍSTICAS FINAIS ===\n");
    printf("Itens processados: %d/%d\n", pipeline->items_processed, NUM_ITEMS);
    printf("Itens gravados: %d/%d\n", pipeline->items_written, NUM_ITEMS);
    
    if (pipeline->items_processed == NUM_ITEMS && 
        pipeline->items_written == NUM_ITEMS) {
        printf("✓ TODOS os itens processados e gravados com sucesso!\n");
    } else {
        printf("✗ ERRO: Itens perdidos durante o processamento!\n");
    }
    
    
    pipeline_destroy(pipeline);
    
    return 0;
}