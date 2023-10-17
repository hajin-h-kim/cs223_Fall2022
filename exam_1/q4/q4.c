#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct wraparound_ {
        int *values;
        size_t capacity;
        size_t size;
        size_t front;  // index where first element currently sits
        size_t end; // always indexes into *next* location a value would be placed
} wraparound;

#define WRAPAROUND_INITIAL_CAPACITY (2)
#define WRAPAROUND_MINIMUM_CAPACITY (2)

/**
 * Takes as input a wraparound array (as defined in the above struct)
 * and a value v (of type int), and returns the wraparound array
 * resulting from the addition of a copy of v to the end of the
 * wraparound array.
 *
 * If there are any issues with space allocation, a NULL pointer is returned.
 *
 * Your implementation need not make any calls to functions defined below.
 */

wraparound *wraparound_add_end(wraparound *w, int v) {
    // WRITE THE wraparound_add_end FUNCTION HERE
    if(w->capacity == w->size)
    {
        w->capacity *= 2;
        int* temp = realloc(w->values, w->capacity * sizeof(int)); 
        // should I malloc and copy w for loop so I can preserve the original array if malloc fails?
        if(temp == NULL) return NULL;
        //free(w->values);
        w->values = temp;
    }
    w->values[w->end++] = v;
    w->size++;

    return w;
}

// DO NOT MODIFY THIS LINE. YOU MAY MODIFY SUBSEQUENT LINES, BUT ANY CHANGES MADE WILL NOT BE TAKEN INTO ACCOUNT DURING THE GRADING PROCESS.

// create/initialize a wraparound array
wraparound *wraparound_create() {
        // make space for the struct
        wraparound *w = malloc(sizeof(wraparound));

        // make space for all values to add
        w->values = malloc(sizeof(int) * WRAPAROUND_INITIAL_CAPACITY);

        w->front = 0;
        w->end = 0;     // points to last element in sequence + 1, equals front if empty
        w->size = 0;
        w->capacity = w->values != NULL ? WRAPAROUND_INITIAL_CAPACITY : 0;

        return w;
}

// destroy the wraparound array
void wraparound_destroy(wraparound *w) {
        free(w->values);
        free(w);
}

// get the value at the specified location
int wraparound_get(wraparound *w, size_t position) {
        return w->values[position];
}

// remove and return the value at the wraparound array's front
int wraparound_remove_front(wraparound *w) {
        int v = wraparound_get(w, w->front);

        // increment the front index
        w->front++;

        w->size--;

        // if at or past the last possible index, wraparound
        if (w->front >= w->capacity) {
                w->front = 0;
        }

        return v;
}

// remove and return the value at the wraparound array's end
int wraparound_remove_end(wraparound *w) {
        size_t end_index = (w->end == 0) ? w->capacity - 1 : w->end - 1;
        int v = wraparound_get(w, end_index);

        // decrement end index
        w->end--;

        w->size--;

        // end must index into the place at which a new value would be inserted
        if (w->end >= w->capacity) {
                w->end = w->capacity - 1;
        }

        return v;
}

// print the contents of the wraparound array
void wraparound_print(wraparound *w) {
        if (w->size > 0) {
                if (w->front > w->end || (w->front == w->end && w->front != 0)) {
                        for (size_t i = w->front; i < w->capacity; i++) {
                                printf("%d ", w->values[i]);
                        }
                        for (size_t i = 0; i < w->end; i++) {
                                printf("%d ", w->values[i]);
                        }
                } else if (w->front == w->end) {
                        for (size_t i = 0; i < w->capacity; i++) {
                                printf("%d ", w->values[i]);
                        }
                } else {
                        for (size_t i = w->front; i < w->end; i++) {
                                printf("%d ", w->values[i]);
                        }
                }
        }
        printf("\n");
}


void wraparound_error_allocation(wraparound *w) {
        wraparound_destroy(w);
        printf("Space could not be allocated");
        exit(1);
}

// Adding x to wraparound array
void test_one() {
        printf("TEST 1\n");
        wraparound *w = wraparound_create();

        w = wraparound_add_end(w, 1);
        if (w == NULL) {
                wraparound_error_allocation(w);
        }
        // output: 1
        wraparound_print(w);

        wraparound_destroy(w);
}

// Can your implementation handle a resize (adding 3 values, w is initialized to have capacity = 2)?
void test_two() {
        printf("TEST 2\n");
        wraparound *w = wraparound_create();

        w = wraparound_add_end(w, 3);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 3
        wraparound_print(w);

        w = wraparound_add_end(w, 7);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 3 7
        wraparound_print(w);

        w = wraparound_add_end(w, 10);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 3 7 10
        wraparound_print(w);

        wraparound_destroy(w);
}

// Handling a resize when front != 0
void test_three() {
        printf("TEST 3\n");
        wraparound *w = wraparound_create();

        w = wraparound_add_end(w, 3);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 3
        wraparound_print(w);

        // removing from the front so that front now != 0
        wraparound_remove_front(w);
        // output:
        wraparound_print(w);

        w = wraparound_add_end(w, 3);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 3
        wraparound_print(w);

        w = wraparound_add_end(w, 7);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 3 7
        wraparound_print(w);

        w = wraparound_add_end(w, 10);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 3 7 10
        wraparound_print(w);

        wraparound_destroy(w);
}

// Adding to/removing from the array several times
void test_four() {
        printf("TEST 4\n");
        wraparound *w = wraparound_create();

        w = wraparound_add_end(w, 3);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 3
        wraparound_print(w);

        w = wraparound_add_end(w, 7);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 3 7
        wraparound_print(w);

        wraparound_remove_front(w);
        // output: 7
        wraparound_print(w);

        w = wraparound_add_end(w, 10);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 7 10
        wraparound_print(w);

        wraparound_remove_end(w);
        // output: 7
        wraparound_print(w);

        w = wraparound_add_end(w, 3);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 7 3
        wraparound_print(w);

        w = wraparound_add_end(w, 7);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 7 3 7
        wraparound_print(w);

        wraparound_remove_end(w);
        // output: 7 3
        wraparound_print(w);

        w = wraparound_add_end(w, 10);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 7 3 10
        wraparound_print(w);

        w = wraparound_add_end(w, 10);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 7 3 10 10
        wraparound_print(w);

        w = wraparound_add_end(w, 10);
        if (w == NULL) { wraparound_error_allocation(w); }
        // output: 7 3 10 10 10
        wraparound_print(w);

        wraparound_destroy(w);
}


int main(int argc, char **argv) {
        if (argc == 1) {
                test_one();
                test_two();
                test_three();
                test_four();
        } else if (argc == 2) {
                int test_num = atoi(argv[1]);
                switch(test_num) {
                        case 1:
                                test_one();
                                break;
                        case 2:
                                test_two();
                                break;
                        case 3:
                                test_three();
                                break;
                        case 4:
                                test_four();
                                break;
                        default:
                                printf("Invalid test number %s\n", argv[1]);
                }
        }
        return 1;
}
