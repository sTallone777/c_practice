#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node定义结构体
typedef struct Node {
    char* key;
    void* value;
    struct Node* next;
} Node;

// 定义HashMap结构体
typedef struct {
    Node** buckets;
    int capacity;
    int size;
} HashMap;

// 哈希函数
unsigned int hash(char* key, int capacity) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;
    return hash % capacity;
}

// put函数
void put(HashMap* map, char* key, void* value) {
    unsigned int index = hash(key, map->capacity);
    Node* node = map->buckets[index];

    while (node != NULL) {
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

// get函数
void* get(HashMap* map, char* key) {
    unsigned int index = hash(key, map->capacity);
    Node* node = map->buckets[index];

    while (node != NULL) {
        if (strcmp(key, node->key) == 0) {
            return node->value;
        }
        node = node->next;
    }

    return NULL;
}

// 释放HashMap内存
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

// 创建HashMap的工厂函数
HashMap* create_map(int capacity) {
    HashMap* map = malloc(sizeof(HashMap));
    map->capacity = capacity;
    map->size = 0;
    map->buckets = calloc(capacity, sizeof(Node*));
    return map;
}

// main函数测试HashMap
int main() {
    HashMap* map = create_map(16);
    printf("HashMap 初始化容量：%d\n", map->capacity);

    int sorce_alice = 96;
    int sorce_bob = 88;

    put(map, "Alice", &sorce_alice);
    put(map, "Bob", &sorce_bob);

    int* value_alice = get(map, "Alice");
    if (value_alice != NULL) {
        printf("Alice 的分数：%d\n", *value_alice);
    }

    int* value_petter = get(map, "Petter");
    if (value_petter != NULL) {
        printf("Petter 的分数：%d\n", *value_petter);
    } else {
        printf("Petter 的分数未找到\n");
    }

    free_map(map);
    printf("内存已释放，程序结束。\n");

    return 0;
}