#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Linked listのNode定義
typedef struct Node {
    char* key;
    void* value;
    struct Node* next;
} Node;

// HashMapの定義
typedef struct HashMap {
    int size;
    int capacity;
    Node** buckets;
} HashMap;

// Hashの算出メソッド
unsigned int hash(char* key, int capacity) {
    unsigned long hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % capacity;
}

// Put
void put(HashMap* map, char* key, void* value) {
    unsigned int index = hash(key, map->capacity);
    Node* node = map->buckets[index];

    while ((node != NULL)) {
        if (strcmp(node->key, key) == 0) {
            node->value = value;
            return;
        }
        node = node->next; 
    }

    Node* newNode = malloc(sizeof(Node));
    newNode->key = strdup(key);
    newNode->value = value;
    newNode->next = map->buckets[index];
    map->buckets[index] = newNode;
    map->size++;
}

// Get
void* get(HashMap* map, char* key) {
    unsigned int index = hash(key, map->capacity);
    Node* node = map->buckets[index];

    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }

    return NULL;
}

// Free
void free_map(HashMap* map) {
    for (int i = 0; i < map->capacity; i++) {
        Node* node = map->buckets[i];

        while (node != NULL) {
            Node* temp = node;
            node = node->next;

            free(temp->key);
            free(temp);
        }
    }

    free(map->buckets);
    free(map);
}

// Create
HashMap* create_map(int capacity) {
    HashMap* map = malloc(sizeof(HashMap));
    map->size = 0;
    map->capacity = capacity;
    map->buckets = calloc(map->capacity, sizeof(Node));

    return map;
}

// Main関数
int main() {
    HashMap* map = create_map(16);
    printf("HashMapの容量：%d\n", map->capacity);

    int val1 = 100;
    int val2 = 200;

    put(map, "Val1", &val1);
    put(map, "Val2", &val2);

    int* getVal1 = get(map, "Val1");
    int* getVal2 = get(map, "Val3");

    if (getVal1 != NULL) {
        printf("Value1の値：%d\n", *getVal1);
    } else {
        printf("Value1の値を見つかりません。\n");
    }

    if (getVal2 != NULL) {
        printf("Value2の値：%d\n", *getVal2);
    } else {
        printf("Value2の値を見つかりません。\n");
    }

    free_map(map);
    printf("HashMapを解放しました。\n");

    return 0;
}