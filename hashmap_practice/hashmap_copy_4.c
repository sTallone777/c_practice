#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *key;
    void *value;
    struct Node *next;
} Node;

typedef struct HashMap {
    int capacity;
    int size;
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

void put(HashMap *map, char *key, void *value) {
    unsigned int index = hash(key, map->capacity);
    Node *node = map->buckets[index];

    while (node != NULL) {
        if (strcmp (key, node->key) == 0) {
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

void *get(HashMap *map, char *key) {
    unsigned int index = hash(key, map->capacity);
    Node *node = map->buckets[index];

    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }

    return NULL;
}

HashMap *create_map(int capacity) {
    HashMap *map = malloc(sizeof(HashMap));
    map->capacity = capacity;
    map->size = 0;
    map->buckets = calloc(map->capacity, sizeof(Node));
    return map;
}

void free_map(HashMap *map) {
    for (int i = 0; i <= map->capacity; i++) {
        Node *node = map->buckets[i];

        while(node != NULL) {
            Node *temp = node;
            node = node->next;
            free(node->key);
            free(node);
        }
    }
    free(map->buckets);
    free(map);
}

int main() {
    HashMap *map = create_map(16);
    printf("HashMap 初期容量：%d\n", map->capacity);

    int user1_value = 98;
    int user2_value = 99;
    put(map, "user1", &user1_value);
    put(map, "user2", &user2_value);

    int *getUser1Value = get(map, "user1");
    int *getUser2Value = get(map, "user3");

    if (!getUser1Value) {
        printf("User1の値取得できません\n");
    } else {
        printf("User1の値は：%d\n", *getUser1Value);
    }

    if (!getUser2Value) {
        printf("User2の値取得できません\n");
    } else {
        printf("User2の値は：%d\n", *getUser2Value);
    }

    free(map);
    printf("HashMapを解放しました。\n");

    return 0;
}