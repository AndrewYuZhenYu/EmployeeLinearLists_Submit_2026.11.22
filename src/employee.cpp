#include "employee.h"  // 引入员工记录、操作状态和这两个函数的声明。

#include <stdio.h>  // 提供 printf，用来输出格式化的员工记录。

Status CompareById(const ElemType *a, const ElemType *b)
// 比较两个员工的工号，供两种表的定位函数调用。
{
    int i = 0;  // 字符数组的比较从下标 0 开始。
    while (a->id[i] != '\0' && a->id[i] == b->id[i])
    // 当前字符相同且尚未遇到工号末尾时继续比较。
    {
        i++;  // 跳过已经确认相同的字符。
    }
    return a->id[i] == b->id[i] ? OK : ERROR;
    // 比较停止处的字符，同时到达字符串末尾时判定两个工号完全相同。
}

Status PrintEmployee(const ElemType *e)  // 输出一条员工记录，也可以作为遍历回调传入。
{
    return printf("%s | %s | %s\n", e->name, e->position, e->id) < 0 ? ERROR : OK;
    // 按姓名、职务、工号的顺序输出，printf 返回负值时报告失败。
}
