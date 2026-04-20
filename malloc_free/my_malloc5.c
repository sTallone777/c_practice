#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>

#define ALIGN(size) (((size) + 7) & ~7)
#define BLOCK_SIZE sizeof(struct Block)

typedef struct Block {
    size_t size;
    int is_free;
    struct Block *next;
} Block;

Block *global_base = NULL;

// find
Block *find_free_block(Block **last, size_t size) {
    Block *current = global_base;

    while(current) {
        if (current->is_free && current->size >= size) break;
        *last = current;
        current = current->next;
    }
    return current;
}

// request
Block *request_space(Block *last, size_t size) {
    Block *block = sbrk(0);
    void *request = sbrk(BLOCK_SIZE + size);

    if (request == (void*)-1) {
        return NULL;
    }

    if (last) last->next = block;

    block->is_free = 0;
    block->size = size;
    block->next = NULL;
    return block;
}

// my_malloc
void *my_malloc(size_t size) {
    if (size <= 0) return NULL;

    size = ALIGN(size);

    if (!global_base) {
        Block *block = request_space(NULL, size);
        if (!block) return NULL;
        global_base = block;
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

// free
void my_free(void *ptr) {
    Block *block = (Block*)ptr - 1;
    block->is_free = 1;

    if (block->next && block->next->is_free) {
        block->size += BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
    }
}

// test
int main() {
    printf("テスト開始\n");

    int *arr = (int*)my_malloc(10 * sizeof(int));
    for (int i = 0; i < 10; i++) {
        arr[i] = 5 * i;
        printf("[%d]:%d, addr: %p\n", i, arr[i], (void*)(arr + i));
    }

    char *str = (char*)my_malloc(40);
    strcpy(str, "あいうえお、かきくけこ");

    for(int i = 0; i < 40; i++) {
        printf("[%d]:0x%02x, addr: %p\n", i, (unsigned char)str[i], (void*)(str + i));
    }

    my_free(arr);
    my_free(str);

    printf("テスト終了\n");
}