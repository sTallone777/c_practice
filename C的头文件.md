## 这三个头文件的作用
头文件	作用
stdio.h	标准输入输出，提供 printf, scanf, fopen 等
stddef.h	提供 size_t, NULL, ptrdiff_t 等基础类型定义
unistd.h	POSIX 系统调用接口，提供 sbrk, read, write, fork 等
在这个文件里，unistd.h 是给 sbrk() 用的，stddef.h 是给 size_t 用的，stdio.h 是给 printf 用的。

## C 常用头文件
标准 C 库（跨平台）

头文件	常用内容
stdio.h	printf, scanf, fopen, fclose
stdlib.h	malloc, free, exit, atoi, rand
string.h	strcpy, strlen, memcpy, memset
stdint.h	int32_t, uint8_t 等固定宽度整型
stddef.h	size_t, NULL, offsetof
stdbool.h	bool, true, false
math.h	sqrt, pow, sin, cos
time.h	time, clock, difftime
assert.h	assert() 断言
errno.h	errno 错误码
limits.h	INT_MAX, CHAR_MIN 等类型边界值
ctype.h	isdigit, isalpha, toupper
POSIX / Linux 系统编程

头文件	常用内容
unistd.h	fork, exec, sbrk, read, write
fcntl.h	open, O_RDONLY, O_WRONLY 文件标志
sys/types.h	pid_t, uid_t, off_t 等系统类型
sys/stat.h	stat, chmod, 文件状态
sys/mman.h	mmap, munmap 内存映射
pthread.h	多线程 API
signal.h	signal, kill, 信号处理
