#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

#define ALIGN(size) (((size) + 7) & ~7)
#define BLOCK_SIZE sizeof(struct Block)

typedef struct Block {
    size_t size;
    int is_free;
    struct Block *next;
} Block;

void *global_base = NULL;

Block *find_free_block(Block **last, size_t size) {
    Block *current = global_base;
    while (current && !(current->is_free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    return current;
}

Block *request_space(Block *last, size_t size) {
    Block *block = sbrk(0);
    void *request = sbrk(size + BLOCK_SIZE);

    if(request == (void*) -1) {
        return NULL;
    }

    if (last) {
        last->next = block;
    }

    block->size = size;
    block->next = NULL;
    block->is_free = 0;
    return block;
}

void *my_malloc(size_t size) {
    if(size <= 0) return NULL;

    size = ALIGN(size);

    if (!global_base) {
        Block *block =request_space(NULL, size);
        if (!block) return NULL;
        global_base = block;
        return (block +1);
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
        block->size = block->size + BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
    }
}

int main() {
    printf("start test malloc...\n");

    int *arr = (int*)my_malloc(10 * sizeof(int));
    if (arr) {
        for (int i = 0; i < 10; i++) {
            arr[i] = i * 10;
            printf("%d", arr[i]);
            printf(":%p ", (void*)(arr + i));
        }
        printf("\n create memory success, address: %p\n", (void*)arr);
    }

    char *str = (char*)my_malloc(20);
    if (str) {
        printf("malloc memory for string success. address: %p\n", (void*)str);
    }

    my_free(arr);
    printf("free memory successed. \n");

    int *arr2 = (int*)my_malloc(5 * sizeof(int));
    if(arr2) {
        for (int i = 0; i < 5; i++) {
            arr2[i] = i * 10;
            printf("%d", arr2[i]);
            printf(":%p ", (void*)(arr2 + i));
        }
    }
    printf("remalloc arr2, address: %p\n", (void*)arr2);

    my_free(str);
    my_free(arr2);

    printf("test completed. \n");
    return 0;
}