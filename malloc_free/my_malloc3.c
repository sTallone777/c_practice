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

// 未使用のブロックを探す処理
Block *find_free_block(Block **last, size_t size) {
    Block *current = (Block*)global_base;

    while (current) {
        if (current->is_free && current->size >= size) break;
        *last = current;
        current = current->next;
    }
    return current;
}

// OSに新しいメモリを申請する処理
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

// my_malloc
void *my_malloc(size_t size) {
    if (size <= 0) return NULL;

    size = ALIGN(size);

    if (!global_base) {
        Block *block = request_space(NULL, size);
        if (!block) {
            return NULL;
        }
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

// my_free
void my_free(void *ptr) {
    if (!ptr) return;

    Block *block = (Block*)ptr - 1;
    block->is_free = 1;

    if (block->next && block->next->is_free) {
        block->size += BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
    }
}

// main
int main() {
    printf("メモリ振り分けテスト...\n");

    int *arr = (int*)my_malloc(10 * sizeof(int));
    if (arr) {
        for (int i = 0; i < 10; i++) {
            arr[i] = i * 10;
            printf("%d", arr[i]);
            printf(":%p ", (void*)(arr+i));
        }
        printf("\nメモリ振り分け成功、アドレス：%p\n", (void*)arr);
    }

    char *str = (char*)my_malloc(20);
    if (str) {
        printf("文字列のメモリ振り分け成功、アドレス：%p\n", (void*)str);
    }

    my_free(arr);
    printf("arr 解放済\n");

    int *arr2 = (int*)my_malloc(5 * sizeof(int));
    printf("再度メモリ振り分け、アドレス：%p\n", (void*)arr2);

    my_free(arr2);
    my_free(str);

    printf("テスト終了。\n");
    return 0;
}