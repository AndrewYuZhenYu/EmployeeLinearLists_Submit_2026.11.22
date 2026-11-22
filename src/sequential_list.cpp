#include "sequential_list.h"  // 引入 SqList 结构、容量常量和顺序表接口声明。

#include <limits.h>  // 提供 INT_MAX，扩容前用它检查容量是否能继续增加。
#include <stdlib.h>  // 提供 malloc、free 和 size_t，用来管理数组空间。

Status InitList_Sq(SqList *L)  // 申请初始空间并建立一个空的顺序表。
{
    L->elem = (ElemType *) malloc(LIST_INIT_SIZE * sizeof(ElemType));
    // 容量乘以单个员工记录的大小，得到申请的字节数。
    if (L->elem == NULL)  // 检查 malloc 是否返回空指针。
    {
        return ERROR;  // 内存不足时告诉调用者初始化失败。
    }
    L->length = 0;  // 已有空间但尚未录入员工，因此有效长度为 0。
    L->listsize = LIST_INIT_SIZE;  // 记录本次申请得到的初始容量。
    return OK;  // 初始化完成，顺序表可以使用。
}

void DestroyList_Sq(SqList *L)  // 释放顺序表管理的存储空间。
{
    free(L->elem);  // 释放连续数组的内存。
    L->elem = NULL;  // 将数组指针置空，避免继续使用已释放的地址。
    L->length = 0;  // 清零有效元素数。
    L->listsize = 0;  // 清零已分配容量。
}

int ListLength_Sq(const SqList *L)  // 从顺序表取得当前元素数。
{
    return L->length;  // 直接返回维护好的 length，无须扫描数组。
}

static Status ExtendList_Sq(SqList *L)  // 为顺序表增加容量，此辅助函数只在本文件内使用。
{
    if (L->listsize > INT_MAX - LISTINCREMENT)
    // 在相加前判断容量是否可能超过 int 的最大值。
    {
        return ERROR;  // 容量加法无法安全进行时，保留原数组并报告失败。
    }

    int listsize = L->listsize + LISTINCREMENT;  // 得到扩容后的元素总容量。
    if ((size_t) listsize > (size_t) -1 / sizeof(ElemType))
    // 先用 size_t 的最大值除以单个元素的大小，检查字节数相乘会不会溢出。
    {
        return ERROR;  // 新数组的字节数无法表示时，返回扩容失败。
    }
    ElemType *newbase = (ElemType *) malloc((size_t) listsize * sizeof(ElemType));
    // 申请一块更大的连续空间，暂时保留旧数组。
    if (newbase == NULL)  // 检查新空间是否申请成功。
    {
        return ERROR;  // 新空间申请失败时直接返回，原数组仍可使用。
    }

    for (int j = 0; j < L->length; j++)  // 遍历旧数组中已经存放的有效元素。
    {
        newbase[j] = L->elem[j];  // 按原来的下标逐个复制员工记录。
    }
    free(L->elem);  // 复制完成后再释放旧数组。
    L->elem = newbase;  // 将顺序表的数组指针改为新地址。
    L->listsize = listsize;  // 把容量改为本次申请的新值，员工人数保持原值。
    return OK;  // 已完成扩容，调用者可以继续插入。
}

Status GetElem_Sq(const SqList *L, int i, ElemType *e)
// 按从 1 开始的位序取元素，结果写入 e 指向的对象。
{
    if (i < 1 || i > L->length)  // 取元素的有效范围是 1 到当前表长。
    {
        return ERROR;  // 位序越界时返回失败，避免访问无效下标。
    }
    *e = L->elem[i - 1];  // 从位序 i 减去 1 得到数组下标，再拷贝记录。
    return OK;  // 告诉调用者已经取得该记录。
}

int LocateElem_Sq(const SqList *L, const ElemType *e,
// 在顺序表中查找第一个满足比较条件的元素。
                  Status (*compare)(const ElemType *, const ElemType *))
                  // compare 是接收两条员工记录的函数指针，由调用者决定比较条件。
{
    for (int j = 0; j < L->length; j++)  // 从数组首元素向后逐个检查。
    {
        if (compare(&L->elem[j], e))  // 将当前记录和查找目标交给比较函数。
        {
            return j + 1;  // 第一个匹配项的位序是数组下标加 1。
        }
    }
    return 0;  // 扫描完整张表仍无匹配时，以 0 表示未找到。
}

Status ListInsert_Sq(SqList *L, int i, ElemType e)
// 在指定的位序前插入一条员工记录。
{
    if (i < 1 || i - 1 > L->length)  // 允许插入的位置是 1 到表长加 1，最后一个位置表示追加。
    {
        return ERROR;  // 遇到非法位序时终止插入。
    }
    if (L->length == L->listsize && !ExtendList_Sq(L))
    // 只有当前数组已满才扩容，并检查扩容结果。
    {
        return ERROR;  // 扩容失败时原表仍有效，本次插入取消。
    }

    for (int j = L->length; j >= i; j--)
    /* 第 i 个元素及其后面的元素依次右移。 */  // 从表尾向插入位置倒着移动，避免覆盖尚未搬走的元素。
    {
        L->elem[j] = L->elem[j - 1];  // 把下标 j 减 1 的记录复制到右边相邻的位置。
    }
    L->elem[i - 1] = e;  // 把新员工放进位序 i 对应的数组位置。
    L->length++;  // 成功放入新元素后将表长加 1。
    return OK;  // 返回插入成功。
}

Status ListDelete_Sq(SqList *L, int i, ElemType *e)
// 删除指定顺序表元素，并通过 e 返回其原值。
{
    if (i < 1 || i > L->length)  // 删除前检查位序是否对应已经存在的元素。
    {
        return ERROR;  // 位序无效时，顺序表保持原样。
    }

    *e = L->elem[i - 1];  // 先复制被删记录，再开始覆盖它原来的位置。
    for (int j = i; j < L->length; j++)  // 从被删元素后面开始向表尾扫描。
    {
        L->elem[j - 1] = L->elem[j];  // 把后面的记录向左搬一格，填补空位。
    }
    L->length--;  // 有效长度减 1，数组容量保持原值。
    return OK;  // 返回删除成功。
}

Status ListTraverse_Sq(const SqList *L, Status (*visit)(const ElemType *))
// 按表中顺序调用 visit，输出就是一种具体的访问方式。
{
    for (int j = 0; j < L->length; j++)  // 按数组下标从小到大访问每条记录。
    {
        if (!visit(&L->elem[j]))  // 将当前记录地址传给访问函数并检查返回状态。
        {
            return ERROR;  // 任一访问失败就结束遍历。
        }
    }
    return OK;  // 全部记录访问完成，返回成功。
}
