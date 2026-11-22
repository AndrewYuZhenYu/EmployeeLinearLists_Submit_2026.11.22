#ifndef LINKED_LIST_H  // 第一次包含单链表头文件时才继续处理下面的声明。
#define LINKED_LIST_H  // 定义保护宏，后续重复包含时跳过本文件内容。

#include "employee.h"  // 引入用于结点数据域的员工类型和操作状态类型。

typedef struct LNode  // 声明链表结点结构，使用 LNode 标签来引用同类型结点。
{
    ElemType data;  // 保存这个结点对应的员工记录。
    struct LNode *next;  // 指向后继结点，链尾的后继为空。
} LNode;  // 结束结点定义，并将类型命名为 LNode。

typedef struct  // 声明单链表的管理结构。
{
    LNode *head;  // 指向头结点，头结点不计入员工记录。
    LNode *tail;  // 指向尾结点，便于直接向表尾插入。
    int len;  // 保存实际员工结点数。
} LinkList;  // 结束链表结构定义，并将类型命名为 LinkList。

Status InitList_L(LinkList *L);  // 声明初始化函数，申请头结点并建立空链表。

void DestroyList_L(LinkList *L);  // 声明销毁函数，依次释放链表结点。

int ListLength_L(const LinkList *L);  // 声明取表长函数，返回有效结点数。

Status GetElem_L(const LinkList *L, int i, ElemType *e);
/* 位序从 1 开始，LocateElem_L 找不到时返回 0。 */  // 声明按位序取元素的函数，通过 e 返回读取的记录。

int LocateElem_L(const LinkList *L, const ElemType *e,
// 声明查找函数，参数 e 指向要匹配的元素。
                 Status (*compare)(const ElemType *, const ElemType *));
                 // compare 指定比较规则，查找员工时可以按工号匹配。

Status ListInsert_L(LinkList *L, int i, ElemType e);
// 声明插入函数，把 e 放到位序 i 对应的位置。

Status ListDelete_L(LinkList *L, int i, ElemType *e);
// 声明删除函数，删除第 i 个结点并把原记录写入 e。

Status ListTraverse_L(const LinkList *L, Status (*visit)(const ElemType *));
// 声明遍历函数，把每条链表记录交给 visit 处理。

#endif  // 结束单链表头文件的重复包含保护。
