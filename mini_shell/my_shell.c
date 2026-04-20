#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_ARGS 64
#define DELIM " \t\r\n\a"

// --- 函数声明 ---
void shell_loop();
char *read_line();
char **split_line(char *line);
int execute(char **args);
void handle_redirect(char **args);

// 内建命令声明
int shell_cd(char **args);
int shell_exit(char **args);
int shell_help(char **args);

char *builtin_str[] = {"cd", "exit", "help"};
int (*builtin_func[]) (char **) = {&shell_cd, &shell_exit, &shell_help};

// --- 内建命令实现 ---
int shell_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mini-shell: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) perror("mini-shell");
    }
    return 1;
}

int shell_help(char **args) {
    printf("Mini-Shell 完整版\n");
    printf("支持功能: 内建命令, 单管道 (|), 重定向 (> <)\n");
    return 1;
}

int shell_exit(char **args) {
    return 0;
}

// --- 核心解析与执行逻辑 ---

/**
 * 处理重定向符号 > 和 <
 * 如果发现符号，则打开文件并重定向标准 IO，然后将符号及文件名从参数列表中移除
 */
void handle_redirect(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            if (args[i + 1] == NULL) {
                fprintf(stderr, "mini-shell: syntax error near >\n");
                exit(EXIT_FAILURE);
            }
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) { perror("open"); exit(EXIT_FAILURE); }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            // 移除 > 和文件名，确保 execvp 不会处理它们
            args[i] = NULL;
        } else if (strcmp(args[i], "<") == 0) {
            if (args[i + 1] == NULL) {
                fprintf(stderr, "mini-shell: syntax error near <\n");
                exit(EXIT_FAILURE);
            }
            int fd = open(args[i + 1], O_RDONLY);
            if (fd < 0) { perror("open"); exit(EXIT_FAILURE); }
            dup2(fd, STDIN_FILENO);
            close(fd);
            args[i] = NULL;
        }
    }
}

/**
 * 执行普通命令 (fork + handle_redirect + execvp)
 */
int launch(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        handle_redirect(args); // 在子进程执行前先处理重定向
        if (execvp(args[0], args) == -1) perror("mini-shell");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
    } else {
        waitpid(pid, NULL, 0);
    }
    return 1;
}

/**
 * 执行管道命令 (cmd1 | cmd2)
 */
int execute_pipe(char **args, int pipe_pos) {
    args[pipe_pos] = NULL; // 将数组拆分为前后两部分
    char **args1 = args;
    char **args2 = &args[pipe_pos + 1];

    int pipefd[2];
    if (pipe(pipefd) == -1) { perror("pipe"); return 1; }

    pid_t p1 = fork();
    if (p1 == 0) {
        dup2(pipefd[1], STDOUT_FILENO); // 写端对接标准输出
        close(pipefd[0]); close(pipefd[1]);
        handle_redirect(args1); // 管道左侧支持重定向
        if (execvp(args1[0], args1) == -1) perror("mini-shell");
        exit(EXIT_FAILURE);
    }

    pid_t p2 = fork();
    if (p2 == 0) {
        dup2(pipefd[0], STDIN_FILENO); // 读端对接标准输入
        close(pipefd[1]); close(pipefd[0]);
        handle_redirect(args2); // 【关键】管道右侧现在也支持重定向了！
        if (execvp(args2[0], args2) == -1) perror("mini-shell");
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]); close(pipefd[1]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
    return 1;
}

/**
 * 命令调度中心
 */
int execute(char **args) {
    if (args[0] == NULL) return 1;

    // 1. 检查管道
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "|") == 0) return execute_pipe(args, i);
    }

    // 2. 检查内建命令
    for (int i = 0; i < sizeof(builtin_str) / sizeof(char *); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    // 3. 执行普通外部命令
    return launch(args);
}

// --- 基础工具函数 ---

char *read_line() {
    char *line = NULL;
    size_t bufsize = 0;
    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) exit(EXIT_SUCCESS);
        else { perror("readline"); exit(EXIT_FAILURE); }
    }
    return line;
}

char **split_line(char *line) {
    int bufsize = MAX_ARGS, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    if (!tokens) { fprintf(stderr, "alloc error\n"); exit(EXIT_FAILURE); }

    char *token = strtok(line, DELIM);
    while (token != NULL) {
        tokens[position++] = token;
        if (position >= bufsize) {
            bufsize += MAX_ARGS;
            tokens = realloc(tokens, bufsize * sizeof(char*));
        }
        token = strtok(NULL, DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

void shell_loop() {
    char *line;
    char **args;
    int status;
    do {
        printf("\033[1;36mmini-shell>\033[0m ");
        line = read_line();
        args = split_line(line);
        status = execute(args);
        free(line);
        free(args);
    } while (status);
}

int main() {
    shell_loop();
    return 0;
}