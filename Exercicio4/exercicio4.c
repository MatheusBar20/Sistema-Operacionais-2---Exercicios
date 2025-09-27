// Pontifícia Universidade Católica 
// Goiânia, 25 de setembro de 2025
// Alunos: Matheus Barbosa Silva, Yan Menezes
// Professor: Fernando Abadia

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define QUEUE_SIZE 5
#define NUM_ITEMS 20
#define POISON_PILL -1


typedef struct {
    int *items;
    int size;
    int count;
    int front;
    int rear;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} Queue;


typedef struct {
    Queue *input_queue;
    Queue *output_queue;
    int thread_id;
} ThreadData;


Queue* queue_init(int size) {
    Queue *q = malloc(sizeof(Queue));
    q->items = malloc(size * sizeof(int));
    q->size = size;
    q->count = 0;
    q->front = 0;
    q->rear = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
    return q;
}


void queue_destroy(Queue *q) {
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
    free(q->items);
    free(q);
}


void queue_put(Queue *q, int item) {
    pthread_mutex_lock(&q->mutex);
    
    while (q->count == q->size) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }
    
    q->items[q->rear] = item;
    q->rear = (q->rear + 1) % q->size;
    q->count++;
    
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}


int queue_get(Queue *q) {
    pthread_mutex_lock(&q->mutex);
    
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    
    int item = q->items[q->front];
    q->front = (q->front + 1) % q->size;
    q->count--;
    
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    
    return item;
}


void* captura_thread(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    printf("Thread Captura iniciada\n");
    
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = i + 1;
        printf("Captura: gerando item %d\n", item);
        queue_put(data->output_queue, item);
        usleep(100000); 
    }
    
    
    printf("Captura: enviando poison pill\n");
    queue_put(data->output_queue, POISON_PILL);
    
    printf("Thread Captura finalizada\n");
    return NULL;
}


void* processamento_thread(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    printf("Thread Processamento iniciada\n");
    
    while (1) {
        int item = queue_get(data->input_queue);
        
        if (item == POISON_PILL) {
            
            printf("Processamento: recebido poison pill, repassando\n");
            queue_put(data->output_queue, POISON_PILL);
            break;
        }
        
        
        printf("Processamento: processando item %d\n", item);
        int processed_item = item * 2; 
        usleep(150000); 
        
        queue_put(data->output_queue, processed_item);
    }
    
    printf("Thread Processamento finalizada\n");
    return NULL;
}


void* gravacao_thread(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    int items_processed = 0;
    printf("Thread Gravação iniciada\n");
    
    while (1) {
        int item = queue_get(data->input_queue);
        
        if (item == POISON_PILL) {
            printf("Gravação: recebido poison pill\n");
            break;
        }
        
        
        printf("Gravação: gravando item processado %d\n", item);
        items_processed++;
        usleep(200000); 
    }
    
    printf("Thread Gravação finalizada. Itens processados: %d\n", items_processed);
    return NULL;
}

int main() {
    printf("Iniciando pipeline com %d itens...\n", NUM_ITEMS);
    
    
    Queue *queue1 = queue_init(QUEUE_SIZE);
    Queue *queue2 = queue_init(QUEUE_SIZE);
    
    
    ThreadData captura_data = {NULL, queue1, 1};
    ThreadData processamento_data = {queue1, queue2, 2};
    ThreadData gravacao_data = {queue2, NULL, 3};
    
    pthread_t captura, processamento, gravacao;
    
    
    pthread_create(&captura, NULL, captura_thread, &captura_data);
    pthread_create(&processamento, NULL, processamento_thread, &processamento_data);
    pthread_create(&gravacao, NULL, gravacao_thread, &gravacao_data);
    
    
    pthread_join(captura, NULL);
    pthread_join(processamento, NULL);
    pthread_join(gravacao, NULL);
    
    
    queue_destroy(queue1);
    queue_destroy(queue2);
    
    printf("Pipeline finalizado com sucesso!\n");
    return 0;
}