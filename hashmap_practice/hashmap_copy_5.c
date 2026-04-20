#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *key;
    void *value;
    struct Node *next;
} Node;

typedef struct HashMap {
    int size;
    int capacity;
    Node **buckets;
} HashMap;

unsigned int hash(char *key, int capacity) {
    unsigned long hash = 5381;
    int c;
    
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % capacity;
}

// put
void put(HashMap *map, char *key, void* value) {
    unsigned int index = hash(key, map->capacity);
    Node *node = map->buckets[index];

    while (node) {
        if (strcmp(key, node->key) == 0) {
            node->value = value;
            return;
        }
        node = node->next;
    } 

    Node *newNode = malloc(sizeof(Node));
    newNode->key = strdup(key);
    newNode->value = value;
    newNode->next = map->buckets[index];
    map->buckets[index] = newNode;
    map->size++;
}

// get
void *get(HashMap *map, char *key) {
    unsigned int index = hash(key, map->capacity);
    Node *node = map->buckets[index];

    while (node) {
        if (strcmp(key, node->key) == 0) {
            return node->value;
        }
        node = node->next;
    }

    return NULL;
}

// create
HashMap *create_map(int capacity) {
    HashMap *map = malloc(sizeof(HashMap));
    map->capacity = capacity;
    map->size = 0;
    map->buckets = calloc(map->capacity, sizeof(Node *));
    return map;
}

// free_map
void free_map(HashMap *map) {
    if (!map) return;
    for (int i = 0; i < map->capacity; i++) {
        Node *node = map->buckets[i];

        while (node) {
            Node *temp = node;
            node = node->next;
            free(temp->key);
            free(temp);
        }
    }
    free(map->buckets);
    free(map);
}

// test
int main() {
    HashMap *map = create_map(16);
    printf("マップの初期化、容量：%d\n", map->capacity);

    int socre1 = 99;
    int socre2 = 88;

    put(map, "alice", &socre1);
    put(map, "bob", &socre2);

    int *value1 = get(map, "alice");
    int *value2 = get(map, "tom");

    if (value1) {
        printf("aliceの成績：%d\n", *value1);
    } else {
        printf("aliceの成績を見つけません。\n");
    }

    if (value2) {
        printf("bobの成績：%d\n", *value2);
    } else {
        printf("bobの成績を見つけません。\n");
    }

    free_map(map);
    printf("マップのメモリを解放しました。\n");
    
    return 0;
}

