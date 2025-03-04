#ifndef STRING_STACK_H
#define STRING_STACK_H

#include <stdbool.h>

#define MAX_STACK_SIZE 100 // 定义栈的最大容量

// 定义栈结构
typedef struct
{
    char *data[MAX_STACK_SIZE]; // 存储字符串指针的数组
    int top;                    // 栈顶指针
} StringStack;

// 初始化栈
void init_string_stack(StringStack *stack);

// 判断栈是否为空
bool is_string_stack_empty(StringStack *stack);

// 判断栈是否已满
bool is_string_stack_full(StringStack *stack);

// 入栈操作
void push_string(StringStack *stack, const char *value);

// 出栈操作
char *pop_string(StringStack *stack);

// 获取栈顶元素（不移除）
char *peek_string(StringStack *stack);

// 打印栈内容（从栈底到栈顶）
void print_string_stack(StringStack *stack);

#endif // STRING_STACK_H

