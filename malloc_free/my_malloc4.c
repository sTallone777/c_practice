#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

// メモリ配置
#define ALIGN(size) (((size) + 7) & ~7)
#define BLOCK_SIZE sizeof(struct Block)

// STRUCT定義
typedef struct Block {
    size_t size;
    int is_free;
    struct Block *next;
}

Block *global_base = NULL;

// フリーのブロックを探す
Block *find_free_block(Block **last, size_t size) {
    Block *current = global_base;

    if (current) {
        if (current->is_free && current->size >= size) break;
        *last = current;
        current = current->next;
    }

    return current;
}

// OSにメモリブロックを申請
Block *request_space(Block *last, size_t size) {
    Block block = sbrk(0);
    void *request = sbrk(size + BLOCK_SIZE);

    if (request == (void*)-1) return NULL;

    if (last) last->next = block;

    block->size = size;
    block->is_free = 0;
    block->next = NULL;
    return block;
}

// malloc
void *my_malloc(size_t size){
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
    if (!ptr) return;

    Block *block = (Block*)ptr - 1;
    block->is_free = 1;

    if(block->next && block->next->is_free) {
        block->size += BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
    }
}