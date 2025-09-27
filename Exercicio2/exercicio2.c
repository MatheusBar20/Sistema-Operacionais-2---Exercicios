// Pontifícia Universidade Católica 
// Goiânia, 25 de setembro de 2025
// Alunos: Matheus Barbosa Silva, Yan Menezes
// Professor: Fernando Abadia

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_BUFFER_SIZE 100
#define MAX_PRODUCERS 5
#define MAX_CONSUMERS 5
#define MAX_ITEMS 1000

typedef struct {
    int *buffer;
    int size;
    int in;
    int out;
    int count;
    
    pthread_mutex_t mutex;
    sem_t empty;
    sem_t full;
    
    long total_items_produced;
    long total_items_consumed;
    double total_producer_wait_time;
    double total_consumer_wait_time;
    struct timeval start_time;
} CircularBuffer;

typedef struct {
    int id;
    CircularBuffer *cb;
    int items_to_produce;
} ProducerArgs;

typedef struct {
    int id;
    CircularBuffer *cb;
    int items_to_consume;
} ConsumerArgs;
int init_circular_buffer(CircularBuffer *cb, int size) {
    cb->buffer = (int *)malloc(size * sizeof(int));
    if (!cb->buffer) return -1;
    
    cb->size = size;
    cb->in = 0;
    cb->out = 0;
    cb->count = 0;
    
    pthread_mutex_init(&cb->mutex, NULL);
    sem_init(&cb->empty, 0, size);
    sem_init(&cb->full, 0, 0);
    
    cb->total_items_produced = 0;
    cb->total_items_consumed = 0;
    cb->total_producer_wait_time = 0.0;
    cb->total_consumer_wait_time = 0.0;
    gettimeofday(&cb->start_time, NULL);
    
    return 0;
}
void destroy_circular_buffer(CircularBuffer *cb) {
    free(cb->buffer);
    pthread_mutex_destroy(&cb->mutex);
    sem_destroy(&cb->empty);
    sem_destroy(&cb->full);
}
int produce_item(int producer_id) {
    usleep(100 + (rand() % 400));
    return rand() % 1000;
}
void consume_item(int item, int consumer_id) {
    usleep(100 + (rand() % 400));
}
void* producer(void *arg) {
    ProducerArgs *args = (ProducerArgs *)arg;
    CircularBuffer *cb = args->cb;
    
    for (int i = 0; i < args->items_to_produce; i++) {
        struct timeval wait_start, wait_end;
        gettimeofday(&wait_start, NULL);
        
        int item = produce_item(args->id);
        
        sem_wait(&cb->empty);
        
        gettimeofday(&wait_end, NULL);
        double wait_time = (wait_end.tv_sec - wait_start.tv_sec) * 1000000.0 +
                          (wait_end.tv_usec - wait_start.tv_usec);
        
        pthread_mutex_lock(&cb->mutex);
        cb->total_producer_wait_time += wait_time;
        
        cb->buffer[cb->in] = item;
        cb->in = (cb->in + 1) % cb->size;
        cb->count++;
        cb->total_items_produced++;
        
        pthread_mutex_unlock(&cb->mutex);
        
        sem_post(&cb->full);
    }
    
    return NULL;
}
void* consumer(void *arg) {
    ConsumerArgs *args = (ConsumerArgs *)arg;
    CircularBuffer *cb = args->cb;
    
    for (int i = 0; i < args->items_to_consume; i++) {
        struct timeval wait_start, wait_end;
        gettimeofday(&wait_start, NULL);
        
        sem_wait(&cb->full);
        
        gettimeofday(&wait_end, NULL);
        double wait_time = (wait_end.tv_sec - wait_start.tv_sec) * 1000000.0 +
                          (wait_end.tv_usec - wait_start.tv_usec);
        
        pthread_mutex_lock(&cb->mutex);
        cb->total_consumer_wait_time += wait_time;
        
        int item = cb->buffer[cb->out];
        cb->out = (cb->out + 1) % cb->size;
        cb->count--;
        cb->total_items_consumed++;
        
        pthread_mutex_unlock(&cb->mutex);
        
        sem_post(&cb->empty);
        
        consume_item(item, args->id);
    }
    
    return NULL;
}
void print_statistics(CircularBuffer *cb, int buffer_size, int num_producers, int num_consumers) {
    struct timeval end_time;
    gettimeofday(&end_time, NULL);
    
    double total_time = (end_time.tv_sec - cb->start_time.tv_sec) +
                       (end_time.tv_usec - cb->start_time.tv_usec) / 1000000.0;
    
    double throughput = cb->total_items_consumed / total_time;
    double avg_producer_wait = cb->total_producer_wait_time / cb->total_items_produced / 1000.0;
    double avg_consumer_wait = cb->total_consumer_wait_time / cb->total_items_consumed / 1000.0;
    
    printf("\n=== ESTATÍSTICAS (Buffer: %d) ===\n", buffer_size);
    printf("Tempo total: %.3f segundos\n", total_time);
    printf("Throughput: %.2f items/segundo\n", throughput);
    printf("Itens produzidos: %ld\n", cb->total_items_produced);
    printf("Itens consumidos: %ld\n", cb->total_items_consumed);
    printf("Tempo médio de espera produtor: %.3f ms\n", avg_producer_wait);
    printf("Tempo médio de espera consumidor: %.3f ms\n", avg_consumer_wait);
    printf("Produtores: %d, Consumidores: %d\n", num_producers, num_consumers);
    printf("==================================\n\n");
}
void run_experiment(int buffer_size, int num_producers, int num_consumers, int total_items) {
    CircularBuffer cb;
    pthread_t producers[MAX_PRODUCERS];
    pthread_t consumers[MAX_CONSUMERS];
    ProducerArgs p_args[MAX_PRODUCERS];
    ConsumerArgs c_args[MAX_CONSUMERS];
    
    if (init_circular_buffer(&cb, buffer_size) != 0) {
        fprintf(stderr, "Erro ao inicializar buffer\n");
        return;
    }
    
    int items_per_producer = total_items / num_producers;
    int items_per_consumer = total_items / num_consumers;
    
    for (int i = 0; i < num_producers; i++) {
        p_args[i].id = i;
        p_args[i].cb = &cb;
        p_args[i].items_to_produce = items_per_producer;
        pthread_create(&producers[i], NULL, producer, &p_args[i]);
    }
    
    for (int i = 0; i < num_consumers; i++) {
        c_args[i].id = i;
        c_args[i].cb = &cb;
        c_args[i].items_to_consume = items_per_consumer;
        pthread_create(&consumers[i], NULL, consumer, &c_args[i]);
    }
    
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }
    
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], NULL);
    }
    
    print_statistics(&cb, buffer_size, num_producers, num_consumers);
    destroy_circular_buffer(&cb);
}

int main() {
    srand(time(NULL));
    
    printf("=== EXPERIMENTO: EFEITO DO TAMANHO DO BUFFER ===\n");
    
    int buffer_sizes[] = {5, 10, 20, 50, 100};
    int num_experiments = sizeof(buffer_sizes) / sizeof(buffer_sizes[0]);
    
    for (int i = 0; i < num_experiments; i++) {
        printf("Executando experimento com buffer size: %d\n", buffer_sizes[i]);
        run_experiment(buffer_sizes[i], 3, 3, 1000);
    }
    
    return 0;
}