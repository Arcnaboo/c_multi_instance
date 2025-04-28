/*
 * Multi-Instance Accessor System
 * 
 * - This system allows multiple instances of objects identified by an integer ID.
 * - It is NOT a set system: ID uniqueness is NOT enforced.
 * - When querying by ID, the first matching instance is returned.
 * - Signal handlers (SIGUSR1, SIGUSR2, SIGINT, SIGHUP) trigger ID-based access.
 * - SIGHUP triggers a graceful program exit after accessing ID4.
 */
 /*
 MIT License

Copyright (c) 2024 Arda Akgur

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define ID1 1
#define ID2 2
#define ID3 3
#define ID4 4

typedef struct {
    int id;
    void *data;
} Class;

// Helper to find instance by ID
static Class *get_instance(int id, Class **instances, int size) {
    for (int i = 0; i < size; i++) {
        if (instances[i]->id == id) {
            return instances[i];
        }
    }
    return NULL;
}

// Main accessor function
Class *getset_class(Class *obj, int *id) {
    static Class **instances = NULL;
    static int len = 10;
    static int size = 0;

    if (obj && !id) {
        if (!instances) {
            instances = malloc(sizeof(Class *) * len);
            if (!instances) {
                perror("malloc failed");
                exit(EXIT_FAILURE);
            }
        }
        if (size >= len) {
            len *= 2;
            instances = realloc(instances, sizeof(Class *) * len);
            if (!instances) {
                perror("realloc failed");
                exit(EXIT_FAILURE);
            }
        }
        instances[size++] = obj;
        return NULL;
    }
    else if (!obj && id) {
        return get_instance(*id, instances, size);
    }
    return NULL;
}

void handle_signal(int sig) {
    int id = 0;
    int should_exit = 0;

    switch (sig) {
        case SIGUSR1:
            id = ID1;
            break;
        case SIGUSR2:
            id = ID2;
            break;
        case SIGINT:
            id = ID3;
            break;
        case SIGHUP:
            id = ID4;
            should_exit = 1;
            break;
        default:
            return; // Ignore unknown signals
    }

    Class *obj = getset_class(NULL, &id);
    if (obj) {
        printf("Received signal for id=%d, data=%s\n", id, (char *)obj->data);
    } else {
        printf("No instance found for id=%d\n", id);
    }

    if (should_exit) {
        printf("SIGHUP received, exiting program.\n");
        exit(0);
    }
}

int main() {
    // Create instances
    Class *obj1 = malloc(sizeof(Class));
    obj1->id = ID1;
    obj1->data = "Hello from ID1";

    Class *obj2 = malloc(sizeof(Class));
    obj2->id = ID2;
    obj2->data = "Hello from ID2";

    Class *obj3 = malloc(sizeof(Class));
    obj3->id = ID3;
    obj3->data = "Goodbye from ID3";

    Class *obj4 = malloc(sizeof(Class));
    obj4->id = ID4;
    obj4->data = "Exit triggered by ID4";

    // Register instances
    getset_class(obj1, NULL);
    getset_class(obj2, NULL);
    getset_class(obj3, NULL);
    getset_class(obj4, NULL);

    // Setup signal handlers
    signal(SIGUSR1, handle_signal);
    signal(SIGUSR2, handle_signal);
    signal(SIGINT, handle_signal);
    signal(SIGHUP, handle_signal);

    printf("Program running. PID: %d\n", getpid());
    printf("Send signals (SIGUSR1, SIGUSR2, SIGINT, SIGHUP) to interact.\n");

    // Main loop
    while (1) {
        pause(); // Wait for signals
    }

    return 0;
}
