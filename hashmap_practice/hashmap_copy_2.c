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

// Putメソッド
void put(HashMap* map, char* key, void* value) {
    unsigned int index = hash(key, map->capacity);
    Node* node = map->buckets[index];

    while (node != NULL) {
        if (strcmp(key, node->key) == 0) {
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

// Getメソッド
void* get(HashMap* map, char* key) {
    unsigned int idx = hash(key, map->capacity);
    Node* node = map->buckets[idx];

    while (node != NULL) {
        if (strcmp(key, node->key) == 0) {
            return node->value;
        }
        node = node->next;
    }

    return NULL;
}

// Freeメソッド
void free_map(HashMap* map) {
    for (int i = 0; i < map->capacity; i++) {
        Node* node = map->buckets[i];
        while (node != NULL) {
            Node* tmp = node;
            node = node->next;
            free(tmp->key);
            free(tmp);
        }
    }
    free(map->buckets);
    free(map);
}

// HashMap作成の工場関数
HashMap* create_map(int capacity) {
    HashMap* map = malloc(sizeof(HashMap));
    map->size = 0;
    map->capacity = capacity;
    map->buckets = calloc(capacity, sizeof(Node));
    return map;
}

// テスト用Main関数
int main() {
    HashMap* map = create_map(16);
    printf("HashMap 初期化容量：%d\n", map->capacity);

    int sorce_alice = 96;
    int sorce_bob = 88;

    put(map, "Alice", &sorce_alice);
    put(map, "Bob", &sorce_bob);

    int* value_alice = get(map, "Alice");
    int* value_tid = get(map, "Tid");

    if (value_alice != NULL) {
        printf("Aliceの点数：%d\n", *value_alice);
    } else {
        printf("Aliceの点数を見つかりません。\n");
    }

    if (value_tid != NULL) {
        printf("Tidの点数：%d\n", *value_tid);
    } else {
        printf("Tidの点数を見つかりません。\n");
    }

    free_map(map);
    printf("メモリを解放しました。\n");

    return 0;
}