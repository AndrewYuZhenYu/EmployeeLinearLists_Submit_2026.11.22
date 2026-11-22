#include "linked_list.h"  // 引入 LNode、LinkList 及对应的链表接口声明。

#include <limits.h>  // 提供 INT_MAX，插入前检查链表长度会不会溢出。
#include <stdlib.h>  // 提供 malloc 和 free，用来申请和释放结点。

Status InitList_L(LinkList *L)  // 申请一个头结点并初始化空的单链表。
{
    L->head = (LNode *) malloc(sizeof(LNode));
    // 用 malloc 分配一个 LNode 作为头结点。
    if (L->head == NULL)  // 判断头结点内存是否申请成功。
    {
        return ERROR;  // 申请失败时返回 ERROR，调用者应取消后续操作。
    }
    L->head->next = NULL;  // 空表中没有第一个有效结点，头结点的 next 为空。
    L->tail = L->head;  // 空表没有有效尾结点，暂时让尾指针也指向头结点。
    L->len = 0;  // 空表的有效元素数为 0。
    return OK;  // 链表建立成功。
}

void DestroyList_L(LinkList *L)  // 依次释放头结点及其后的所有结点。
{
    LNode *p = L->head;  // 从头结点开始清理，这样头结点自己的内存也会释放。
    while (p != NULL)  // 当前还有结点时继续释放。
    {
        LNode *q = p->next;  // 先保存后继地址，释放 p 后就不能再访问 p 的成员。
        free(p);  // 释放当前结点占用的内存。
        p = q;  // 切换到刚才保存的下一个结点。
    }
    L->head = NULL;  // 清空头指针。
    L->tail = NULL;  // 清空尾指针。
    L->len = 0;  // 链表释放后把记录数归零。
}

int ListLength_L(const LinkList *L)  // 取得单链表的有效元素数。
{
    return L->len;  // 直接返回维护好的 len，取得长度只需常数时间。
}

Status GetElem_L(const LinkList *L, int i, ElemType *e)
// 按位序从链表读取元素，将记录复制给调用者。
{
    if (i < 1 || i > L->len)  // 检查目标位序是否在 1 到当前表长之间。
    {
        return ERROR;  // 位序越界时返回失败，避免沿空指针继续走。
    }

    const LNode *p = L->head->next;  // 从头结点的后继出发，此处是第 1 个有效结点。
    for (int j = 1; j < i; j++)  // 从位序 1 开始，向后移动到指定的第 i 个结点。
    {
        p = p->next;  // 通过 next 前进一个结点。
    }
    *e = p->data;  // 把找到的结点数据写入 e 指向的变量。
    return OK;  // 返回取值成功。
}

int LocateElem_L(const LinkList *L, const ElemType *e,
// 在单链表中查找首个满足条件的元素。
                 Status (*compare)(const ElemType *, const ElemType *))
                 // compare 决定两个元素是否匹配，员工查询时传入按工号的比较函数。
{
    const LNode *p = L->head->next;  // 跳过头结点，从第一条实际员工记录开始。
    int i = 0;  // 计数从 0 起步，每访问一个有效结点就加 1。
    while (p != NULL)  // 指针未到链尾时继续搜索。
    {
        i++;  // 将计数更新为当前结点从 1 开始的位序。
        if (compare(&p->data, e))  // 让比较函数判断当前结点是否满足查找条件。
        {
            return i;  // 返回首个匹配结点的位序。
        }
        p = p->next;  // 当前记录不匹配时，沿 next 检查后继结点。
    }
    return 0;  // 遍历到链尾仍无匹配，以 0 表示未找到。
}

Status ListInsert_L(LinkList *L, int i, ElemType e)
// 分配新结点，并把它插到指定位置。
{
    if (i < 1 || i - 1 > L->len || L->len == INT_MAX)
    // 检查插入位序，并避免元素个数超过 int 的最大值。
    {
        return ERROR;  // 参数不合法或无法再增加长度时终止操作。
    }

    LNode *p = L->tail;
    /* 表尾插入时直接用尾指针，否则从头结点寻找前驱。 */  // 默认把当前尾结点当作新结点的前驱。
    if (i - 1 != L->len)  // 只有插入位置不是表尾，才需要重新寻找前驱。
    {
        p = L->head;  // 寻找前驱时从头结点开始，视头结点位序为 0。
        for (int j = 0; j < i - 1; j++)  // 沿链走 i 减 1 步，停在插入位置的前一个结点上。
        {
            p = p->next;  // 每循环一次前进到后继。
        }
    }

    LNode *s = (LNode *) malloc(sizeof(LNode));  // 为新员工分配一个结点。
    if (s == NULL)  // 检查申请过程是否返回空指针。
    {
        return ERROR;  // 申请失败时在修改链条前退出，原链表仍有效。
    }
    s->data = e;  // 将传入的员工记录保存到新结点。
    s->next = p->next;  // 先把新结点连到原前驱的后继，保住插入点后的链条。
    p->next = s;  // 再让前驱指向新结点，完成接入。
    if (L->tail == p)  // 如果前驱原来是尾结点，这次插入就产生了新的尾结点。
    {
        L->tail = s;  // 更新尾指针，使下次尾插仍可直接定位。
    }
    L->len++;  // 记入刚插入的结点，更新链表长度。
    return OK;  // 返回插入成功。
}

Status ListDelete_L(LinkList *L, int i, ElemType *e)
// 按位序删除一个结点，并通过 e 交回该结点数据。
{
    if (i < 1 || i > L->len)  // 检查待删位序是否指向实际存在的结点。
    {
        return ERROR;  // 位序无效就返回错误。
    }

    LNode *p = L->head;  // 从头结点开始寻找待删结点的前驱。
    for (int j = 0; j < i - 1; j++)  // 走 i 减 1 步，到达第 i 个结点的前面。
    {
        p = p->next;  // 每次沿 next 走一步。
    }
    LNode *q = p->next;  // 前驱的后继 q 就是要删除的结点。
    p->next = q->next;  // 让前驱直接连向 q 的后继，把 q 从链条摘下。
    if (L->tail == q)  // 判断被摘下的是否是原尾结点。
    {
        L->tail = p;  // 删除尾结点后把前驱设为新的尾结点，删空时它就是头结点。
    }
    *e = q->data;  // 释放前先复制被删员工的数据。
    free(q);  // 释放摘下结点的内存。
    L->len--;  // 从有效结点数中扣除被删元素。
    return OK;  // 返回删除成功。
}

Status ListTraverse_L(const LinkList *L, Status (*visit)(const ElemType *))
// 从头到尾对每条链表记录调用一次访问函数。
{
    const LNode *p = L->head->next;  // 从第一个有效结点开始，跳过头结点。
    while (p != NULL)  // 当前结点存在时继续处理。
    {
        if (!visit(&p->data))  // 将当前记录交给 visit，并检查它是否成功。
        {
            return ERROR;  // 回调失败立即中止遍历并报告失败。
        }
        p = p->next;  // 当前记录访问完毕，沿 next 访问下一条。
    }
    return OK;  // 所有结点均已访问，返回成功。
}
