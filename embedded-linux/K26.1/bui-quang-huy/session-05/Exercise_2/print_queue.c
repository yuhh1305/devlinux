#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int  doc_id;
    char filename[60];
    int  pages;
} Document;

Document queue[5];
int head = 0;
int tail = 0;
int count = 0;
int all_sent = 0;

pthread_mutex_t q_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t  not_empty = PTHREAD_COND_INITIALIZER;

int total_submitted = 0;
int total_printed = 0;
int total_pages_printed = 0;

const Document doc_payloads[3][3] = {
    {
        {1, "report_Q1.pdf", 12},
        {4, "slides.pdf",    20},
        {7, "summary.pdf",   4}
    },
    {
        {2, "contract.pdf",  5},
        {5, "memo.pdf",      2},
        {8, "budget.pdf",    7}
    },
    {
        {3, "invoice.pdf",   3},
        {6, "proposal.pdf",  8},
        {9, "review.pdf",    5} 
    }
};

void* producer(void* arg) {
    long p_id = (long)arg;

    for (int i = 0; i < 3; i++) {
        Document doc = doc_payloads[p_id - 1][i];

        pthread_mutex_lock(&q_lock);

        
        while (count == 5) {
            printf("[Producer %ld] Queue full — waiting...\n", p_id);
            pthread_cond_wait(&not_full, &q_lock);
        }

        queue[tail] = doc;
        tail = (tail + 1) % 5;
        count++;
        total_submitted++;

        printf("[Producer %ld] Submitting: %s (%d pages) — queue: %d/5\n", 
               p_id, doc.filename, doc.pages, count);

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&q_lock);
    }
    return NULL;
}

void* printer(void* arg) {
    (void)arg; 

    while (1) {
        pthread_mutex_lock(&q_lock);

        while (count == 0 && !all_sent) {
            pthread_cond_wait(&not_empty, &q_lock);
        }

        if (count == 0 && all_sent) {
            printf("[Printer]    All documents printed. Exiting.\n");
            pthread_mutex_unlock(&q_lock);
            break;
        }

        Document doc = queue[head];
        head = (head + 1) % 5;
        count--;
        total_printed++;
        total_pages_printed += doc.pages;

        printf("[Printer]    Printing:   %s (%d pages) — queue: %d/5\n", 
               doc.filename, doc.pages, count);

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&q_lock);

        sleep(1);
    }
    return NULL;
}

int main(void) {
    pthread_t producers[3];
    pthread_t printer_th;

    printf("==============================================\n");
    printf("   OFFICE PRINT QUEUE (3 producers, 1 printer)\n");
    printf("   Queue capacity: 5 documents\n");
    printf("==============================================\n\n");

    if (pthread_create(&printer_th, NULL, printer, NULL) != 0) {
        perror("Failed to initialize printer worker");
        return EXIT_FAILURE;
    }

    for (long i = 0; i < 3; i++) {
        if (pthread_create(&producers[i], NULL, producer, (void*)(i + 1)) != 0) {
            perror("Failed to initialize producer worker");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(producers[i], NULL);
    }

    pthread_mutex_lock(&q_lock);
    all_sent = 1;
    pthread_cond_broadcast(&not_empty); 
    pthread_mutex_unlock(&q_lock);

    pthread_join(printer_th, NULL);

    printf("\n================ SUMMARY ================\n");
    printf("  Documents submitted : %d\n", total_submitted);
    printf("  Documents printed   : %d\n", total_printed);
    printf("  Total pages printed : %d\n", total_pages_printed);
    printf("=========================================\n");

    pthread_mutex_destroy(&q_lock);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return EXIT_SUCCESS;
}

/*
================================================================================
CRITICAL ARCHITECTURAL QUESTION ANSWER:
Why must pthread_cond_wait() be inside a 'while' loop rather than an 'if'? 
What is a 'spurious wakeup'?
================================================================================
1. The Predicate Re-evaluation Imperative:
A thread suspended on `pthread_cond_wait` must re-evaluate its predicate state via a 
'while' loop because a signal notification does not guarantee exclusive ownership of 
the resource modification condition. Under a multi-core kernel environment, between the 
instant a thread is signaled and the instant it re-acquires the associated mutex, 
another thread could intercept the state lock, consume the resource token, and reset 
the state boundary back to empty/full. Using an 'if' block would allow a thread to pass 
blindly, resulting in buffer overflows or invalid memory access.

2. Spurious Wakeups Defined:
A spurious wakeup occurs when a thread is unblocked from a condition variable without 
any explicit software invocation of `pthread_cond_signal` or `pthread_cond_broadcast`. 
This behavior is rooted in low-level kernel optimizations and interrupt handling on 
POSIX compliant architectures (like Linux pthreads), where forcing condition tracking 
to be fully atomic across context switches without occasional "false wakeups" would 
severely degrade kernel context switching speed. The POSIX standard explicitly documents 
and allows these anomalies, placing the safety burden on the user application to always 
wrap condition waits inside a loop structure.
*/