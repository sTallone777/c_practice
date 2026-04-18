#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

/* --- 1. 基础宏定义与结构体 --- */

// 保持内存 8 字节对齐
#define ALIGN(size) (((size) + 7) & ~7)
#define BLOCK_SIZE sizeof(struct Block)

typedef struct Block {
    size_t size;           // 实际用户可用数据的大小
    int is_free;           // 标志位：1 为空闲，0 为占用
    struct Block *next;    // 链表指针
} Block;

void *global_base = NULL;  // 堆内存链表的起始点

/* --- 2. 辅助函数 --- */

// 遍历链表寻找合适的空闲块 (First Fit)
Block *find_free_block(Block **last, size_t size) {
    Block *current = global_base;
    while (current && !(current->is_free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    return current;
}

// 向操作系统申请增加堆空间
Block *request_space(Block *last, size_t size) {
    Block *block = sbrk(0); // 获取当前断点
    void *request = sbrk(size + BLOCK_SIZE);
    
    if (request == (void*)-1) {
        return NULL; // sbrk 失败（内存不足）
    }

    if (last) {
        last->next = block;
    }
    
    block->size = size;
    block->next = NULL;
    block->is_free = 0;
    return block;
}

/* --- 3. 核心 API 实现 --- */

// 自定义 malloc
void *my_malloc(size_t size) {
    if (size <= 0) return NULL;

    size = ALIGN(size); // 对齐字节

    if (!global_base) { // 第一次分配
        Block *block = request_space(NULL, size);
        if (!block) return NULL;
        global_base = block;
        return (block + 1);
    }

    Block *last = global_base;
    Block *found = find_free_block(&last, size);

    if (!found) {
        // 没有找到合适的空闲块，申请新的
        found = request_space(last, size);
        if (!found) return NULL;
    } else {
        // 找到了合适的空闲块
        found->is_free = 0;
    }

    // 返回 Block 结构体之后的地址，即用户数据区的起点
    return (found + 1);
}

// 自定义 free
void my_free(void *ptr) {
    if (!ptr) return;

    // 将指针向回偏移，找到 Block 首部
    Block *block = (Block*)ptr - 1;
    block->is_free = 1;
    
    // 注意：真实实现中此处应有 coalesce (合并相邻空闲块) 操作
    // 以防止内存碎片化严重。
    // coalesce：合并后继空闲块
    if (block->next && block->next->is_free) {
        block->size += BLOCK_SIZE + block->next->size;  // 吸收下一个块
        block->next = block->next->next;                // 跳过已合并的节点
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