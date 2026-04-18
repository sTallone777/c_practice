#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. 定义节点和表结构
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

// 2. 哈希函数
unsigned int hash(char* key, int capacity) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;
    return hash % capacity;
}

// 3. 插入 (Put)
void put(HashMap* map, char* key, void* value) {
    unsigned int index = hash(key, map->capacity);
    Node* node = map->buckets[index];

    // 检查是否存在相同的 Key
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            node->value = value; // 更新值
            return;
        }
        node = node->next;
    }

    // 头插法创建新节点
    Node* newNode = malloc(sizeof(Node));
    newNode->key = strdup(key); // 复制字符串，防止外部修改
    newNode->value = value;
    newNode->next = map->buckets[index];
    map->buckets[index] = newNode;
    map->size++;
}

// 4. 查找 (Get)
void* get(HashMap* map, char* key) {
    unsigned int index = hash(key, map->capacity);
    Node* node = map->buckets[index];

    // 遍历链表寻找对应的 key
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }
    return NULL; // 没找到
}

// 5. 释放内存 (非常重要，防止内存泄漏)
void free_map(HashMap* map) {
    for (int i = 0; i < map->capacity; i++) {
        Node* node = map->buckets[i];
        while (node != NULL) {
            Node* temp = node;
            node = node->next;
            free(temp->key); // strdup 分配的内存需要 free
            free(temp);      // 释放节点本身
        }
    }
    free(map->buckets); // 释放桶数组
}

// 6. 测试主函数
int main() {
    // 初始化 HashMap
    HashMap map;
    map.capacity = 16;
    map.size = 0;
    map.buckets = calloc(map.capacity, sizeof(Node*));

    printf("HashMap 初始化完成，容量: %d\n", map.capacity);

    // 存入数据
    int score_alice = 95;
    int score_bob = 88;
    
    put(&map, "Alice", &score_alice);
    put(&map, "Bob", &score_bob);

    // 查找并打印数据
    int* ptr_alice = (int*)get(&map, "Alice");
    if (ptr_alice) {
        printf("Alice 的分数是: %d\n", *ptr_alice);
    }

    int* ptr_charlie = (int*)get(&map, "Charlie");
    if (ptr_charlie == NULL) {
        printf("找不到 Charlie 的分数。\n");
    }

    // 释放内存
    free_map(&map);
    printf("内存已清理，程序结束。\n");

    return 0;
}