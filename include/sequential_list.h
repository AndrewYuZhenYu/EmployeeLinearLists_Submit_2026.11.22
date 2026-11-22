#ifndef SEQUENTIAL_LIST_H  // 第一次包含顺序表头文件时才继续处理下面的声明。
#define SEQUENTIAL_LIST_H  // 定义保护宏，后续重复包含时会跳过本文件内容。

#include "employee.h"  // 引入顺序表需要的员工类型 ElemType 和返回状态类型 Status。

#define LIST_INIT_SIZE 80  // 初始数组可容纳 80 条员工记录。
#define LISTINCREMENT 10  // 每次扩容多预留 10 个元素的位置。

typedef struct  // 声明顺序表的管理结构。
{
    ElemType *elem;  // 指向保存员工记录的连续数组。
    int length;  // 记录当前已有多少条有效记录。
    int listsize;  // 记录当前数组一共能存多少条记录。
} SqList;  // 结束结构体声明，并将类型命名为 SqList。

Status InitList_Sq(SqList *L);  // 声明初始化函数，申请连续空间并建立空表。

void DestroyList_Sq(SqList *L);  // 声明销毁函数，释放顺序表申请的数组空间。

int ListLength_Sq(const SqList *L);  // 声明取表长函数，返回有效元素数。

Status GetElem_Sq(const SqList *L, int i, ElemType *e);
/* 位序从 1 开始，LocateElem_Sq 找不到时返回 0。 */  // 声明按位序取元素的函数，结果写入 e 指向的变量。

int LocateElem_Sq(const SqList *L, const ElemType *e,
    // 声明顺序表的定位函数，用 e 传入待比较的目标。
                  Status (*compare)(const ElemType *, const ElemType *));
// compare 是由调用者提供的比较函数指针。

Status ListInsert_Sq(SqList *L, int i, ElemType e);
// 声明在第 i 个位置插入元素 e 的函数。

Status ListDelete_Sq(SqList *L, int i, ElemType *e);
// 声明删除第 i 个元素的函数，通过 e 写回被删元素。

Status ListTraverse_Sq(const SqList *L, Status (*visit)(const ElemType *));
// 声明遍历函数，把每个元素依次交给 visit 处理。

#endif  // 结束顺序表头文件的重复包含保护。
