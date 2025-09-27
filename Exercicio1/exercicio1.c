// Pontifícia Universidade Católica 
// Goiânia, 25 de setembro de 2025
// Alunos: Matheus Barbosa Silva, Yan Menezes
// Professor: Fernando Abadia

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define NUM_CAVALOS 5
#define DISTANCIA_TOTAL 100


typedef struct {
    int id;
    int posicao;
    bool finalizou;
    int colocacao;
} Cavalo;


Cavalo cavalos[NUM_CAVALOS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t start_cond = PTHREAD_COND_INITIALIZER;
bool corrida_iniciada = false;
int colocacao_atual = 1;
int vencedor = -1;
int apostas[NUM_CAVALOS] = {0};


void atualizar_placar(int cavalo_id, int nova_posicao) {
    pthread_mutex_lock(&mutex);
    
    cavalos[cavalo_id].posicao = nova_posicao;
    
    
    if (nova_posicao >= DISTANCIA_TOTAL && !cavalos[cavalo_id].finalizou) {
        cavalos[cavalo_id].finalizou = true;
        cavalos[cavalo_id].colocacao = colocacao_atual;
        
        
        if (colocacao_atual == 1) {
            vencedor = cavalo_id;
        }
        
        colocacao_atual++;
        printf("🎯 Cavalo %d cruzou a linha de chegada em %dº lugar!\n", 
               cavalo_id + 1, cavalos[cavalo_id].colocacao);
    }
    
    pthread_mutex_unlock(&mutex);
}


void* correr(void* arg) {
    int cavalo_id = *((int*)arg);
    
    
    pthread_mutex_lock(&mutex);
    while (!corrida_iniciada) {
        pthread_cond_wait(&start_cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    
    while (cavalos[cavalo_id].posicao < DISTANCIA_TOTAL) {
        
        int avanco = (rand() % 10) + 1;
        int nova_posicao = cavalos[cavalo_id].posicao + avanco;
        
        atualizar_placar(cavalo_id, nova_posicao);
        
        usleep(100000 + (rand() % 200000)); 
    }
    
    return NULL;
}


void exibir_placar() {
    pthread_mutex_lock(&mutex);
    
    printf("\n📊 PLACAR ATUAL:\n");
    printf("---------------------------------\n");
    for (int i = 0; i < NUM_CAVALOS; i++) {
        printf("Cavalo %d: [", i + 1);
        int progresso = (cavalos[i].posicao * 50) / DISTANCIA_TOTAL;
        for (int j = 0; j < 50; j++) {
            if (j < progresso) printf("=");
            else if (j == progresso) printf(">");
            else printf(" ");
        }
        printf("] %3d/%d", cavalos[i].posicao, DISTANCIA_TOTAL);
        
        if (cavalos[i].finalizou) {
            printf(" - %dº lugar", cavalos[i].colocacao);
        }
        printf("\n");
    }
    printf("---------------------------------\n");
    
    pthread_mutex_unlock(&mutex);
}


void inicializar_cavalos() {
    for (int i = 0; i < NUM_CAVALOS; i++) {
        cavalos[i].id = i;
        cavalos[i].posicao = 0;
        cavalos[i].finalizou = false;
        cavalos[i].colocacao = 0;
    }
}

int main() {
    srand(time(NULL));
    pthread_t threads[NUM_CAVALOS];
    int ids[NUM_CAVALOS];
    
    inicializar_cavalos();
    
    printf("🏇 CORRIDA DE CAVALOS 🏇\n");
    printf("========================\n\n");
    
    
    printf("Faça sua aposta! Escolha um cavalo (1-%d): ", NUM_CAVALOS);
    int aposta;
    scanf("%d", &aposta);
    
    if (aposta < 1 || aposta > NUM_CAVALOS) {
        printf("Aposta inválida! Apostando no cavalo 1 por padrão.\n");
        aposta = 1;
    }
    
    printf("Você apostou no cavalo %d!\n\n", aposta);
    
    for (int i = 0; i < NUM_CAVALOS; i++) {
        ids[i] = i;
        if (pthread_create(&threads[i], NULL, correr, &ids[i]) != 0) {
            perror("Erro ao criar thread");
            return 1;
        }
    }
    
    printf("Cavalos prontos na largada...\n");
    sleep(2);
    printf("3...\n");
    sleep(1);
    printf("2...\n");
    sleep(1);
    printf("1...\n");
    sleep(1);
    printf("🏁 LARGADA! 🏁\n\n");
    
    
    pthread_mutex_lock(&mutex);
    corrida_iniciada = true;
    pthread_cond_broadcast(&start_cond);
    pthread_mutex_unlock(&mutex);
    
    
    while (colocacao_atual <= NUM_CAVALOS) {
        exibir_placar();
        sleep(1);
        
        
        bool todos_finalizados = true;
        for (int i = 0; i < NUM_CAVALOS; i++) {
            if (!cavalos[i].finalizou) {
                todos_finalizados = false;
                break;
            }
        }
        if (todos_finalizados) break;
    }
    
    
    for (int i = 0; i < NUM_CAVALOS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    
    printf("\n🎉 RESULTADO FINAL 🎉\n");
    printf("=====================\n");
    
    for (int i = 0; i < NUM_CAVALOS; i++) {
        printf("%dº lugar: Cavalo %d\n", cavalos[i].colocacao, i + 1);
    }
    
    printf("\n🏆 VENCEDOR: Cavalo %d!\n", vencedor + 1);
    
    if (vencedor + 1 == aposta) {
        printf("🎊 PARABÉNS! Sua aposta foi correta! 🎊\n");
    } else {
        printf("😢 Que pena! Você apostou no cavalo %d.\n", aposta);
    }
    
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&start_cond);
    
    return 0;
}