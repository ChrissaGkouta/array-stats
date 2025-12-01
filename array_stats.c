#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

struct array_stats_s {
    long long int info_array_0;
    long long int info_array_1;
    long long int info_array_2;
    long long int info_array_3;
} array_stats;

// Global μεταβλητές για πρόσβαση από τα νήματα
int **arrays;
int N; // Μέγεθος κάθε πίνακα

// Συνάρτηση για μέτρηση χρόνου
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// Σειριακή συνάρτηση ελέγχου
void serial_analysis(struct array_stats_s *stats) {
    stats->info_array_0 = 0;
    stats->info_array_1 = 0;
    stats->info_array_2 = 0;
    stats->info_array_3 = 0;

    for (int i = 0; i < N; i++) if (arrays[0][i] != 0) stats->info_array_0++;
    for (int i = 0; i < N; i++) if (arrays[1][i] != 0) stats->info_array_1++;
    for (int i = 0; i < N; i++) if (arrays[2][i] != 0) stats->info_array_2++;
    for (int i = 0; i < N; i++) if (arrays[3][i] != 0) stats->info_array_3++;
}

// Συνάρτηση που εκτελεί κάθε νήμα
void *thread_func(void *arg) {
    int thread_id = *(int *)arg;

    // Κάθε νήμα διατρέχει τον δικό του πίνακα
    for (int i = 0; i < N; i++) {
        if (arrays[thread_id][i] != 0) {
            // Απευθείας αύξηση της κοινόχρηστης δομής
            switch (thread_id) {
                case 0: array_stats.info_array_0++; break;
                case 1: array_stats.info_array_1++; break;
                case 2: array_stats.info_array_2++; break;
                case 3: array_stats.info_array_3++; break;
            }
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <N elements per array>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);
    int num_threads = 4;
    double start, end;

    // --- INITIALIZATION ---
    start = get_time();
    
    // Δέσμευση μνήμης για τους 4 πίνακες
    arrays = (int **)malloc(4 * sizeof(int *));
    for (int i = 0; i < 4; i++) {
        arrays[i] = (int *)malloc(N * sizeof(int));
        // Αρχικοποίηση με τυχαίους αριθμούς 0-9
        for (int j = 0; j < N; j++) {
            arrays[i][j] = rand() % 10;
        }
    }
    
    // Αρχικοποίηση δομής
    array_stats.info_array_0 = 0;
    array_stats.info_array_1 = 0;
    array_stats.info_array_2 = 0;
    array_stats.info_array_3 = 0;

    end = get_time();
    printf("Initialization Time: %.6f seconds\n", end - start);

    // --- SERIAL EXECUTION ---
    struct array_stats_s serial_stats;
    start = get_time();
    serial_analysis(&serial_stats);
    end = get_time();
    printf("Serial Execution Time: %.6f seconds\n", end - start);

    // --- PARALLEL EXECUTION ---
    pthread_t threads[num_threads];
    int thread_ids[num_threads];

    start = get_time();
    
    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    end = get_time();
    printf("Parallel Execution Time: %.6f seconds\n", end - start);

    // --- VERIFICATION ---
    if (array_stats.info_array_0 == serial_stats.info_array_0 &&
        array_stats.info_array_1 == serial_stats.info_array_1 &&
        array_stats.info_array_2 == serial_stats.info_array_2 &&
        array_stats.info_array_3 == serial_stats.info_array_3) {
        printf("Verification: SUCCESS (Results match)\n");
    } else {
        printf("Verification: FAILURE (Results do not match)\n");
    }

    // Free memory
    for (int i = 0; i < 4; i++) free(arrays[i]);
    free(arrays);

    return 0;
}