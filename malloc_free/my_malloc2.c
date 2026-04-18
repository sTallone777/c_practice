#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

#define ALIGN(size) (((size) + 7) & ~7)
#define BLOCK_SIZE sizeof(struct Block)

typedef struct Block{
    size_t size;
    int is_free;
    struct Block *next;
} Block;

void *global_base = NULL;

Block *find_free_block(Block **last, size_t size) {
    Block *current = global_base;

    while (current) {
        if (current->is_free && current->size >= size) break;
        *last = current;
        current = current->next;
    }
    return current;
}

Block *request_space(Block *last, size_t size) {
    Block *block = sbrk(0);
    void *request = sbrk(BLOCK_SIZE + size);

    if (request == (void*)-1) {
        return NULL;
    }

    if (last) {
        last->next = block;
    }

    block->size = size;
    block->is_free = 0;
    block->next = NULL;
    return block;
}

void *my_malloc(size_t size) {
    if (size <= 0) return NULL;

    size = ALIGN(size);

    if (!global_base) {
        Block *block = request_space(NULL, size);
        if (!block) return NULL;
        return (block + 1);
    }

    Block *last = global_base;
    Block *found = find_free_block(&last, size);

    if (!found) {
        found = request_space(last, size);
        if (!found) return NULL;
    } else {
        found->is_free = 0;
    }

    return (found + 1);
}

void my_free(void *ptr) {
    if (!ptr) return;

    Block *block = (Block*)ptr - 1;
    block->is_free = 1;

    if (block->next && block->next->is_free) {
        block->size += BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
    }
}

/* --- 4. 测试演示 --- */

int main() {
    printf("开始内存分配测试...\n");

    // 分配整数数组
    int *arr = (int*)my_malloc(10 * sizeof(int));
    if (arr) {
        for (int i = 0; i < 10; i++) {
            arr[i] = i * 10;
            printf("%d", arr[i]);
            printf(":%p ", (void*)(arr + i));
        }
        printf("\n分配成功，地址: %p\n", (void*)arr);
    }

    // 分配字符串
    char *str = (char*)my_malloc(20);
    if (str) {
        printf("分配字符串空间成功，地址: %p\n", (void*)str);
    }

    // 释放内存
    my_free(arr);
    printf("已释放 arr\n");

    // 再次尝试分配，观察是否重用了之前的内存块
    int *arr2 = (int*)my_malloc(5 * sizeof(int));
    printf("重新分配 arr2，地址: %p (应与 arr 接近或相同)\n", (void*)arr2);

    my_free(str);
    my_free(arr2);

    printf("测试完成。\n");
    return 0;
}