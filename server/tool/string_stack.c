#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_stack.h"

// 初始化栈
void init_string_stack(StringStack *stack)
{
    stack->top = -1; // 初始化栈顶指针为 -1，表示栈为空
}

// 判断栈是否为空
bool is_string_stack_empty(StringStack *stack)
{
    return stack->top == -1;
}

// 判断栈是否已满
bool is_string_stack_full(StringStack *stack)
{
    return stack->top == MAX_STACK_SIZE - 1;
}

// 入栈操作
void push_string(StringStack *stack, const char *value)
{
    if (is_string_stack_full(stack))
    {
        printf("Error: String stack overflow!\n");
        return;
    }

    // 分配内存并复制字符串
    stack->data[++stack->top] = strdup(value);
    if (stack->data[stack->top] == NULL)
    {
        printf("Error: Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
}

// 出栈操作
char *pop_string(StringStack *stack)
{
    if (is_string_stack_empty(stack))
    {
        printf("Error: String stack underflow!\n");
        return NULL; // 返回 NULL 表示栈为空
    }

    char *popped_value = stack->data[stack->top];
    stack->data[stack->top--] = NULL; // 清空栈顶指针
    return popped_value;
}

// 获取栈顶元素（不移除）
char *peek_string(StringStack *stack)
{
    if (is_string_stack_empty(stack))
    {
        printf("Error: String stack is empty!\n");
        return NULL; // 返回 NULL 表示栈为空
    }
    return stack->data[stack->top];
}

// 打印栈内容（从栈底到栈顶）
void print_string_stack(StringStack *stack)
{
    if (is_string_stack_empty(stack))
    {
        printf("String stack is empty.\n");
        return;
    }

    printf("String stack contents (bottom to top): ");
    for (int i = 0; i <= stack->top; i++)
    {
        printf("\"%s\" ", stack->data[i]);
    }
    printf("\n");
}

