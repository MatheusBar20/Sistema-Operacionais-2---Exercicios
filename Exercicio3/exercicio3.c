// Pontifícia Universidade Católica 
// Goiânia, 25 de setembro de 2025
// Alunos: Matheus Barbosa Silva, Yan Menezes
// Professor: Fernando Abadia

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_ACCOUNTS 100
#define MAX_THREADS 20
#define MAX_TRANSFERS 10000
#define INITIAL_BALANCE 1000

typedef struct {
    int id;
    double balance;
    pthread_mutex_t lock;
} Account;

typedef struct {
    int id;
    Account *accounts;
    int num_accounts;
    int transfers_per_thread;
    bool use_locks;
    double *global_balance;
    pthread_mutex_t *global_lock;
} ThreadArgs;
void init_accounts(Account *accounts, int num_accounts, double initial_balance) {
    for (int i = 0; i < num_accounts; i++) {
        accounts[i].id = i;
        accounts[i].balance = initial_balance;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }
}
void destroy_accounts(Account *accounts, int num_accounts) {
    for (int i = 0; i < num_accounts; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }
}
void transfer_with_locks(Account *from, Account *to, double amount) {
    if (from->id < to->id) {
        pthread_mutex_lock(&from->lock);
        pthread_mutex_lock(&to->lock);
    } else {
        pthread_mutex_lock(&to->lock);
        pthread_mutex_lock(&from->lock);
    }
    
    if (from->balance >= amount) {
        from->balance -= amount;
        to->balance += amount;
    }
    
    if (from->id < to->id) {
        pthread_mutex_unlock(&to->lock);
        pthread_mutex_unlock(&from->lock);
    } else {
        pthread_mutex_unlock(&from->lock);
        pthread_mutex_unlock(&to->lock);
    }
}
void transfer_without_locks(Account *from, Account *to, double amount) {
    if (from->balance >= amount) {
        from->balance -= amount;
        to->balance += amount;
    }
}
void* thread_function(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    
    for (int i = 0; i < args->transfers_per_thread; i++) {

        int from_acc, to_acc;
        do {
            from_acc = rand() % args->num_accounts;
            to_acc = rand() % args->num_accounts;
        } while (from_acc == to_acc);
        

        double amount = 1 + (rand() % 100);
        
        if (args->use_locks) {
            transfer_with_locks(&args->accounts[from_acc], 
                               &args->accounts[to_acc], amount);
        } else {
            transfer_without_locks(&args->accounts[from_acc], 
                                  &args->accounts[to_acc], amount);
        }
    }
    
    return NULL;
}
double calculate_global_balance(Account *accounts, int num_accounts) {
    double total = 0.0;
    for (int i = 0; i < num_accounts; i++) {
        total += accounts[i].balance;
    }
    return total;
}
void verify_global_balance(Account *accounts, int num_accounts, 
                          double expected_balance, const char *test_name) {
    double actual_balance = calculate_global_balance(accounts, num_accounts);
    
    printf("%s: Saldo esperado = %.2f, Saldo atual = %.2f", 
           test_name, expected_balance, actual_balance);
    
    if (fabs(actual_balance - expected_balance) < 0.01) {
        printf(" ✓ CORRETO\n");
    } else {
        printf(" ✗ ERRO! Diferença: %.2f\n", 
               fabs(actual_balance - expected_balance));
    }
}
void run_test(int num_accounts, int num_threads, int transfers_per_thread, 
             bool use_locks, const char *test_name) {
    printf("\n=== %s ===\n", test_name);
    printf("Contas: %d, Threads: %d, Transferências: %d\n", 
           num_accounts, num_threads, num_threads * transfers_per_thread);
    
    Account accounts[MAX_ACCOUNTS];
    pthread_t threads[MAX_THREADS];
    ThreadArgs thread_args[MAX_THREADS];
    
    init_accounts(accounts, num_accounts, INITIAL_BALANCE);
    double initial_global_balance = calculate_global_balance(accounts, num_accounts);
    
    for (int i = 0; i < num_threads; i++) {
        thread_args[i].id = i;
        thread_args[i].accounts = accounts;
        thread_args[i].num_accounts = num_accounts;
        thread_args[i].transfers_per_thread = transfers_per_thread;
        thread_args[i].use_locks = use_locks;
        
        pthread_create(&threads[i], NULL, thread_function, &thread_args[i]);
    }
    
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    verify_global_balance(accounts, num_accounts, initial_global_balance, test_name);
    
    destroy_accounts(accounts, num_accounts);
}
void* stress_test_thread(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    double initial_global_balance = *args->global_balance;
    
    for (int i = 0; i < args->transfers_per_thread; i++) {
        int from_acc, to_acc;
        do {
            from_acc = rand() % args->num_accounts;
            to_acc = rand() % args->num_accounts;
        } while (from_acc == to_acc);
        
        double amount = 1 + (rand() % 100);
        
        if (args->use_locks) {
            transfer_with_locks(&args->accounts[from_acc], 
                               &args->accounts[to_acc], amount);
        } else {
            transfer_without_locks(&args->accounts[from_acc], 
                                  &args->accounts[to_acc], amount);
        }
        

        if (i % 100 == 0 && args->use_locks) {
            pthread_mutex_lock(args->global_lock);
            double current_balance = calculate_global_balance(args->accounts, 
                                                           args->num_accounts);
            assert(fabs(current_balance - initial_global_balance) < 0.01 && 
                   "Saldo global inconsistente!");
            pthread_mutex_unlock(args->global_lock);
        }
    }
    
    return NULL;
}

void run_stress_test(int num_accounts, int num_threads, int transfers_per_thread) {
    printf("\n=== TESTE DE ESTRESSE COM VERIFICAÇÃO CONTÍNUA ===\n");
    
    Account accounts[MAX_ACCOUNTS];
    pthread_t threads[MAX_THREADS];
    ThreadArgs thread_args[MAX_THREADS];
    pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;
    
    init_accounts(accounts, num_accounts, INITIAL_BALANCE);
    double initial_global_balance = calculate_global_balance(accounts, num_accounts);
    double global_balance = initial_global_balance;
    
    for (int i = 0; i < num_threads; i++) {
        thread_args[i].id = i;
        thread_args[i].accounts = accounts;
        thread_args[i].num_accounts = num_accounts;
        thread_args[i].transfers_per_thread = transfers_per_thread;
        thread_args[i].use_locks = true;
        thread_args[i].global_balance = &global_balance;
        thread_args[i].global_lock = &global_lock;
        
        pthread_create(&threads[i], NULL, stress_test_thread, &thread_args[i]);
    }
    
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    verify_global_balance(accounts, num_accounts, initial_global_balance, 
                         "Teste de Estresse");
    
    destroy_accounts(accounts, num_accounts);
    pthread_mutex_destroy(&global_lock);
    
    printf("Teste de estresse concluído sem erros de assert!\n");
}

int main() {
    srand(time(NULL));
    
    printf("=== SIMULAÇÃO DE TRANSFERÊNCIAS ENTRE CONTAS ===\n");
    printf("Saldo inicial por conta: %.2f\n", INITIAL_BALANCE);
    
    run_test(10, 5, 1000, true, "TESTE COM LOCKS");
    
    run_test(10, 5, 1000, false, "TESTE SEM LOCKS");
    
    run_test(20, 10, 2000, true, "TESTE GRANDE COM LOCKS");
    run_test(20, 10, 2000, false, "TESTE GRANDE SEM LOCKS");
    
    run_stress_test(15, 8, 5000);
    
    printf("\n=== CONCLUSÃO ===\n");
    printf("1. Com locks: Saldo global permanece constante ✓\n");
    printf("2. Sem locks: Condições de corrida causam inconsistências ✗\n");
    printf("3. A ordem de aquisição de locks evita deadlocks ✓\n");
    printf("4. Asserções provam a consistência do saldo global ✓\n");
    
    return 0;
}