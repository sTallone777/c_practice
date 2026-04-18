#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char* key;
    void* value;
    struct Node* next;
} Node;

typedef struct {
    Node** buckets;
    int capacity;
    int size;
} HashMap;

unsigned int hash(char* key, int capacity) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;
    return hash % capacity;
}

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

int main() {
    HashMap* map = malloc(sizeof(HashMap));
    memset(map, 0, sizeof(HashMap)); // 初始化结构体
    map->capacity = 16;
    map->size = 0;
    map->buckets = calloc(map->capacity, sizeof(Node*));

    printf("HashMap 初期化容量：%d\n", map->capacity);

    int sorce_alice = 95;
    int sorce_bob = 88;

    put(map, "Alice", &sorce_alice);
    put(map, "Bob", &sorce_bob);

    int* ptr_alice = get(map, "Alice");
    if(ptr_alice != NULL) {
        printf("Alice 的分数: %d\n", *ptr_alice);
    } else {
        printf("Alice 的分数未找到\n");
    }

    int* ptr_charlie = get(map, "Charlie");
    if(ptr_charlie != NULL){
        printf("Charlie 的分数: %d\n", *ptr_charlie);
    } else {
        printf("Charlie 的分数未找到\n");
    }

    free_map(map);
    printf("内存已释放，程序结束。\n");

    return 0;
}