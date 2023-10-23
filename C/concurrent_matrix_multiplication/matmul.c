#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <sys/time.h>

#define N 1200
#define M 1000
#define P 500


int MA[N][M];
int MB[M][P];
int RefC[N][P];
int MC[N][P];

struct timespec start, finish;
double elapsed;

typedef struct {
    int       m_start;
    int       m_finish;
} args_t;

void serial_matmul() {
    int i, j, k;
    for (i = 0; i < N; i++) {
        for (j = 0; j < P; j++) {
            RefC[i][j] = 0;
            for (k = 0; k < M; k++) {
                RefC[i][j] += MA[i][k] * MB[k][j];
            }
        }
    }
}

void* matmul(void *args) {
    args_t *targs = args;
    int m_start = targs->m_start;
    int m_finish = targs->m_finish;

    int i, j, k;
    for (i=m_start; i<m_finish; i++) {
        for (j=0; j<P; j++) {
            MC[i][j] = 0;
            for (k = 0; k < M; k++) {
                MC[i][j] += MA[i][k] * MB[k][j];
            }
        }
    }
    pthread_exit(NULL);
}


void print_matrix(char* name, int Mat[N][P]){
    printf("Matrix: The %s array is :\n", name);
    int r, c;
    for(r=0; r < N; r++) {
        for (c=0; c < P; c++) {
            printf("%d ", Mat[r][c]);
        }
        printf("\n");
    }
}

int check_matrix() {
    int r, c;
    for(r=0; r<N; r++) {
        for (c=0; c<P; c++) {
            if (RefC[r][c] != MC[r][c]) {
                return 0;
            }
        }
    }
    return 1;
}

int main(int argc, char* argv[]) {
    assert (argc == 2);
    int total_num_threads = atoi(argv[1]);
    assert(1 <= total_num_threads);
    assert(total_num_threads <= 5);

    int* print_threads = (int*)malloc(total_num_threads*sizeof(int));
    double* print_times = (double*)malloc(total_num_threads*sizeof(double));
    int* print_errors = (int*)malloc(total_num_threads*sizeof(int));

    int iter = 0;
    int num_threads, i, j, k;
    for (num_threads=1; num_threads<=total_num_threads; num_threads++) {
        print_threads[iter] = num_threads;

        for (i = 0; i < N; i++) {
            for (j = 0; j < M; j++) {
                MA[i][j] = i + j;
            }
        }

        for (i = 0; i < M; i++) {
            for (j = 0; j < P; j++) {
                MB[i][j] = j;
            }
        }

        //serial multiplication
        serial_matmul();
//        if (iter == 0) {
//            print_matrix("RefC", RefC);
//        }

        pthread_t *threads = (pthread_t *) calloc(num_threads, sizeof(pthread_t));
        args_t *thread_data = (args_t *) calloc(num_threads, sizeof(args_t));
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        if (threads == NULL || thread_data == NULL) {
            fprintf(stderr, "Thread data creation failed!\n");
            exit(EXIT_FAILURE);
        }

        int thread_num = 0;
        int chunk_size = N / num_threads;
        int m_start, m_finish;
        int chunk_index = 0;
	int t;
        for (t = 0; t < num_threads-1; t++) {
            m_start = t * chunk_size;
            m_finish = (t + 1) * chunk_size;
            thread_data[thread_num].m_start = m_start;
            thread_data[thread_num].m_finish = m_finish;
            thread_num++;
            chunk_index++;
        }
        m_start = chunk_index * chunk_size;
        m_finish = N;
        thread_data[thread_num].m_start = m_start;
        thread_data[thread_num].m_finish = m_finish;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for (t = 0; t < num_threads; t++) {
            pthread_create(&threads[t],
                           &attr,
                           matmul,
                           (void *) &thread_data[t]);
        }

        for (t = 0; t < num_threads; t++) {
            pthread_join(threads[t], NULL);
        }
        clock_gettime(CLOCK_MONOTONIC, &finish);
        elapsed = (finish.tv_sec - start.tv_sec);
        elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

//        print_matrix("C", MC);

        int err = check_matrix(RefC, MC);
        print_errors[iter] = err;

        print_times[iter] = elapsed;

        free(threads);
        free(thread_data);
        iter++;
    }

    char err[10];
    printf("Threads\tSeconds\tErrors\n");
    
    for (i=1; i<=total_num_threads; i++) {
        if(print_errors[iter] == 0)
        {
            strcpy(err, "No Errors");
        }
        else
        {
            strcpy(err, "Error");
        }

        int iter = i-1;
//        printf("%d\t%ld.%ld\t%s\n", print_threads[iter], print_times_s[iter], print_times_us[iter], err);
        printf("%d\t%f\t%s\n", print_threads[iter], print_times[iter], err);
    }

    exit(EXIT_SUCCESS);
}
