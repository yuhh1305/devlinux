#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int  agent_id;
    char customer[50];
    int  seats_wanted;
} BookingRequest;

BookingRequest requests[5] = {
    {1, "Nguyen Van An",  2},
    {2, "Tran Thi Bich",  1},
    {3, "Le Van Cuong",   3},
    {4, "Pham Thi Dung",  1},
    {5, "Hoang Van Em",   4}
};

int seats_available = 10;
int seats_sold = 0;
int failed_bookings = 0;
pthread_mutex_t seat_lock;

void* book_ticket(void* arg) {
    BookingRequest* req = (BookingRequest*)arg;
    pthread_t tid = pthread_self();

    printf("[Agent %d | TID %lu] Booking %d seats for %s...\n", 
           req->agent_id, (unsigned long)tid, req->seats_wanted, req->customer);

    sleep(1);

    if (pthread_mutex_lock(&seat_lock) != 0) {
        perror("Failed to lock mutex");
        pthread_exit(NULL);
    }

    if (seats_available >= req->seats_wanted) {
        seats_available -= req->seats_wanted;
        seats_sold += req->seats_wanted;
        printf("[Agent %d] CONFIRMED: %d seats for %s.  Remaining: %d\n", 
               req->agent_id, req->seats_wanted, req->customer, seats_available);
    } else {
        failed_bookings++;
        printf("[Agent %d] SOLD OUT:  needs %d seats, only %d left — booking failed.\n", 
               req->agent_id, req->seats_wanted, seats_available);
    }

    if (pthread_mutex_unlock(&seat_lock) != 0) {
        perror("Failed to unlock mutex");
        pthread_exit(NULL);
    }

    return NULL;
}

int main(void) {
    pthread_t threads[5];
    int i;

    printf("==============================================\n");
    printf("   TICKET BOOKING SYSTEM (5 agents, 10 seats)\n");
    printf("==============================================\n");

    if (pthread_mutex_init(&seat_lock, NULL) != 0) {
        perror("Mutex initialization failed");
        return EXIT_FAILURE;
    }

    for (i = 0; i < 5; i++) {
        if (pthread_create(&threads[i], NULL, book_ticket, &requests[i]) != 0) {
            perror("Failed to create thread");
            pthread_mutex_destroy(&seat_lock);
            return EXIT_FAILURE;
        }
    }

    for (i = 0; i < 5; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }

    printf("\n================ SUMMARY ================\n");
    printf("  Total seats     : 10\n");
    printf("  Seats sold      : %d\n", seats_sold);
    printf("  Seats remaining : %d\n", seats_available);
    printf("  Failed bookings : %d\n", failed_bookings);
    printf("=========================================\n");

    if (pthread_mutex_destroy(&seat_lock) != 0) {
        perror("Mutex destruction failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/*
================================================================================
CRITICAL ARCHITECTURAL QUESTION ANSWER:
Why must check and deduct be inside the SAME lock/unlock critical section block?
================================================================================
Splitting the 'check' and 'deduct' operations into two distinct lock-unlock blocks 
introduces a severe synchronization vulnerability known as a TOCTOU (Time-of-Check 
to Time-of-Use) Race Condition.

If split:
1. Thread A acquires the lock, verifies that (seats_available >= wanted), 
   and immediately releases the lock.
2. Context switch occurs before Thread A can acquire the second lock to deduct.
3. Thread B acquires the lock, checks the exact same 'seats_available' value, 
   validates it, and proceeds to deduct.
4. When Thread A resumes and re-locks to execute its deduction, the original state 
   it verified has changed. 

This causes system invariant corruption, resulting in overbooking (negative inventory), 
which violates strict deterministic constraints required in safety-critical firmware and backend services.
*/