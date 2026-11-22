#include <limits.h>  // 提供 INT_MAX，供整数解析和容量计算检查溢出。
#include <stdio.h>  // 提供 printf、getchar、puts、fflush 和 EOF，用于终端交互。
#include <stdlib.h>  // 提供 malloc 和 free，用于管理数组和结点占用的内存。

#define EMPLOYEE_NAME_SIZE 64
/* 员工记录。 */  // 姓名数组占 64 字节，其中一字节留给字符串结束符。
#define EMPLOYEE_POSITION_SIZE 128  // 职务数组占 128 字节，其中一字节留给字符串结束符。
#define EMPLOYEE_ID_SIZE 64  // 工号数组占 64 字节，按字符串存储可保留前导零。

typedef int Status;  // Status 是整数类型的别名，用来表示操作成功或失败。
#define OK 1  // 约定用 1 表示操作成功或比较匹配。
#define ERROR 0  // 约定用 0 表示操作失败或比较不匹配。

typedef struct  // 声明员工记录的结构体。
{
    char name[EMPLOYEE_NAME_SIZE];  // 保存员工姓名。
    char position[EMPLOYEE_POSITION_SIZE];  // 保存员工职务。
    char id[EMPLOYEE_ID_SIZE];  // 保存员工工号。
}
ElemType;  // 为员工记录定义类型名 ElemType，它是线性表中的一个元素。

#define LIST_INIT_SIZE 80  /* 顺序表。 */  // 顺序表初始化时为 80 个元素预留空间。
#define LISTINCREMENT 10  // 顺序表每次扩容时增加 10 个元素的空间。

typedef struct  // 声明顺序表的管理结构。
{
    ElemType *elem;  // 指向顺序表连续数组的首地址。
    int length;  // 记录数组中实际存放的员工人数。
    int listsize;  // 记录已分配空间最多能容纳的元素数。
}
SqList;  // 为顺序表结构定义类型名 SqList。

typedef struct LNode
/* 带头结点的单链表。 */  // 声明单链表结点，用 LNode 标签让结点可以引用同类型的后继。
{
    ElemType data;  // 保存当前结点对应的员工记录。
    struct LNode *next;  // 指向下一个结点，尾结点的后继为空。
}
LNode;  // 定义结点类型名 LNode。

typedef struct  // 声明带头结点的单链表管理结构。
{
    LNode *head;  // 指向头结点，头结点本身不存放员工。
    LNode *tail;  // 指向最后一个有效结点，空表时也指向头结点。
    int len;  // 记录有效结点数，头结点不计入长度。
}
LinkList;  // 为单链表结构定义类型名 LinkList。

Status CompareById(const ElemType *a, const ElemType *b)
// 比较两个员工的工号，供两种表的定位函数调用。
{
    int i = 0;  // 字符数组的比较从下标 0 开始。
    while (a->id[i] != '\0' && a->id[i] == b->id[i])
    // 当前字符相同且尚未遇到工号末尾时继续比较。
    {
        i++;  // 跳过已经确认相同的字符。
    }
    if (a->id[i] == b->id[i])  // 检查停止处字符是否相同，同时到达结束符才代表两个工号完全相同。
    {
        return OK;  // 两个工号相同，返回匹配状态。
    }
    return ERROR;  // 字符不同或一个工号先结束，返回不匹配状态。
}

Status PrintEmployee(const ElemType *e)  // 输出一条员工记录，也可以作为遍历回调传入。
{
    if (printf("%s | %s | %s\n", e->name, e->position, e->id) < 0)
    // 按姓名、职务、工号的顺序打印，检查 printf 是否返回负值。
    {
        return ERROR;  // 输出失败时返回 ERROR，遍历函数可据此停止。
    }
    return OK;  // 记录输出成功，返回 OK。
}

Status InitList_Sq(SqList *L)
/* 顺序表基本操作，位序从 1 开始。 */  // 申请初始空间并建立一个空的顺序表。
{
    L->elem = (ElemType *)malloc(LIST_INIT_SIZE * sizeof(ElemType));
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
    if ((size_t)listsize > (size_t)-1 / sizeof(ElemType))
    // 先用 size_t 的最大值除以单个元素的大小，检查字节数相乘会不会溢出。
    {
        return ERROR;  // 新数组的字节数无法表示时，返回扩容失败。
    }
    ElemType *newbase = (ElemType *)malloc((size_t)listsize * sizeof(ElemType));
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

Status InitList_L(LinkList *L)
/* 单链表基本操作，位序从 1 开始。 */  // 申请一个头结点并初始化空的单链表。
{
    L->head = (LNode *)malloc(sizeof(LNode));
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

    LNode *s = (LNode *)malloc(sizeof(LNode));  // 为新员工分配一个结点。
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

#define USE_SEQUENTIAL 1  /* 员工管理和交互菜单。 */  // 用菜单选项 1 表示顺序表。
#define USE_LINKED 2  // 用菜单选项 2 表示单链表。

typedef struct  // 声明菜单操作使用的管理结构，将模式与两种表的存储字段放在一起。
{
    int mode;  // 记下当前选择，后续操作按它调用对应的表函数。
    SqList sq;  // 选择顺序表时使用这张表。
    LinkList link;  // 选择单链表时使用这张表。
}
EmployeeList;  // 为员工表管理结构定义类型名 EmployeeList。

static int readLine(const char *prompt, char *text, int size)
/* 正常读入返回 1，输入结束返回 0，输入行无效返回 -1。 */  // 从标准输入读一行并写入 text，size 是缓冲区总容量。
{
    printf("%s", prompt);  // 把传入的提示文字显示在终端。
    fflush(stdout);  // 提示没有换行，刷新 stdout 确保等待输入前就能看到提示。

    int length = 0;  // 记录已经写进缓冲区的字节数。
    int tooLong = 0;  // 记录本行是否超过缓冲区可接受的长度。
    int hasNull = 0;  // 记录输入中是否出现了会截断 C 字符串的空字符。
    int ch;  // 用 int 保存读取结果，以便区分任意字节和 EOF。
    while ((ch = getchar()) != '\n' && ch != EOF)
    // 逐字节读取，遇到换行或输入结束时停止。
    {
        if (ch == '\r')  // 识别回车字节，以兼容以回车加换行结尾的文本。
        {
            continue;  // 跳过回车，继续读同一行。
        }
        if (ch == '\0')  // 检查读到的是否是 C 字符串使用的结束字节。
        {
            hasNull = 1;  // 记下无效字符，稍后在读完这一行后统一报错。
        }
        if (length < size - 1)  // 留出最后一个字节作为字符串结束符，剩余空间才能存内容。
        {
            text[length] = (char)ch;  // 将一个有效范围内的输入字节写入当前下标。
            length++;  // 已写入长度加 1，指向下一个空位。
        }
        else  // 缓冲区满了仍继续消耗本行，防止剩余字符被当成下一次回答。
        {
            tooLong = 1;  // 标记本行超长，下一步返回错误。
        }
    }
    if (ch == EOF && length == 0)  // 判断这次是否在没有读到内容的情况下遇到 EOF。
    {
        return 0;  // 返回 0，通知上层停止读取。
    }
    if (tooLong)  // 这一行已经全部读过，现在处理超长情况。
    {
        printf("输入过长，最多 %d 字节，请重试。\n", size - 1);
        // 提示最大内容字节数，空出一字节用于结束符。
        return -1;  // 返回无效输入状态，使调用者可以重试。
    }
    if (hasNull)  // 检查是否因输入中包含字符串结束字节而无效。
    {
        puts("输入含有无效字符，请重试。");  // 提示重输，避免工号或数字被中途截断。
        return -1;  // 返回无效输入状态。
    }

    int start = 0;  // 从行首的下标 0 开始找有效内容。
    while (start < length && (text[start] == ' ' || text[start] == '\t'))
    // 逐个检查开头的空格和制表符。
    {
        start++;  // 将起始下标向后移，略过一个行首空白。
    }
    while (length > start && (text[length - 1] == ' ' || text[length - 1] == '\t'))
    // 从行尾往前检查空白，但不越过有效内容起点。
    {
        length--;  // 缩短有效内容的结束位置，丢弃一个行尾空白。
    }
    int i = 0;  // 准备从缓冲区下标 0 开始保存去掉首尾空白后的文本。
    while (start < length)  // 遍历第一个有效字符到修剪后的行尾。
    {
        text[i] = text[start];  // 将一个有效字符移到新位置，保留内容中的内部空格。
        i++;  // 将写入位置向后推进一格。
        start++;  // 将读取位置向后推进一格。
    }
    text[i] = '\0';  // 在有效文本末尾补上字符串结束符。
    return 1;  // 返回 1，表示这次成功读取一行。
}

static int readNumber(const char *prompt, int maximum, int allowBlank, int *result)
// 读取范围在 0 到 maximum 之间的整数，通过 result 交回结果。
{
    char text[64];  // 为待解析的数字文本准备一个 64 字节缓冲区。
    int status;  // 保存 readLine 的状态，用来区分成功、无效和输入结束。
    while ((status = readLine(prompt, text, sizeof(text))) != 0)
    // 持续提示并读取，直到收到可用的整数或遇到 EOF。
    {
        if (status < 0)  // 检查这次读取的行是否被判为无效。
        {
            continue;  // 跳过无效行，重新提示输入。
        }
        if (allowBlank && text[0] == '\0')  // 只有调用者允许时，空行才能作为有效选择。
        {
            *result = -1;  /* 不限操作次数。 */  // 用负值表示不限操作次数，与正常的非负次数区分。
            return 1;  // 告诉调用者已接受空行这一选择。
        }

        int i = 0;  // 从文本首字符开始解析。
        if (text[0] == '+' || text[0] == '-')
        // 识别可选的正负号，便于之后验证输入是否为非负数。
        {
            i++;  // 数字解析从符号后面开始。
        }
        int start = i;  // 记住首个应当出现数字的下标。
        int value = 0;  // 初始化累计值，随后逐位构成十进制整数。
        while (text[i] >= '0' && text[i] <= '9')
        // 只要当前位置还是十进制数字，就继续读取。
        {
            int digit = text[i] - '0';  // 用字符减去字符 0，把数字字符转成对应的整数。
            if (value > (INT_MAX - digit) / 10)
            // 执行乘 10 加下一位前，先判断结果是否会超出 int 上限。
            {
                break;  // 无法安全累加时停止，未解析的数字会使后面的完整性检查失败。
            }
            value = value * 10 + digit;  // 将已解析的值左移一位十进制，再补上当前数字。
            i++;  // 移到下一个数字。
        }

        if (i > start && text[i] == '\0' && value <= maximum &&
        // 要求至少有一位数字、整行已经解析完，并且结果不超过指定的上限。
            (text[0] != '-' || value == 0))  // 排除带负号且值非零的输入，允许带符号的零。
            {
            *result = value;  // 把验证通过的整数写入调用者提供的地址。
            return 1;  // 返回 1，表示这次读数成功。
        }
        printf("输入无效，请输入 0 到 %d 之间的整数。\n", maximum);
        // 输入格式或数值不符合条件时，说明允许的范围。
    }
    return 0;  // 输入结束后返回 0，调用者据此退出当前流程。
}

static int readField(const char *prompt, char *text, int size)
// 读取一项不能为空的文本，例如姓名或工号。
{
    int status;  // 保存本轮读入的状态。
    while ((status = readLine(prompt, text, size)) != 0)
    // 遇到空白或无效输入时可以重试，遇到 EOF 才结束。
    {
        if (status < 0)  // 检查刚读到的整行是否有效。
        {
            continue;  // 丢弃无效输入并重新提示。
        }
        if (text[0] != '\0')  // 用第一个字节是否是结束符来判断字符串是否为空。
        {
            return 1;  // 字段有内容，向调用者报告读取成功。
        }
        puts("输入不能为空，请重试。");  // 空行或只有首尾空白的行需要重新输入。
    }
    return 0;  // 到达 EOF，报告本字段未读到。
}

static int readEmployee(ElemType *employee)  // 逐项读取一名员工的完整记录。
{
    return readField("姓名：", employee->name, EMPLOYEE_NAME_SIZE) &&
    // 先读取姓名，成功后才会继续执行右侧的读取。
           readField("职务：", employee->position, EMPLOYEE_POSITION_SIZE) &&
           // 接着读取职务，前一项失败时这一项不会执行。
           readField("工号：", employee->id, EMPLOYEE_ID_SIZE);
           // 最后读工号，三项都成功时才整体返回 1。
}

static int listLength(const EmployeeList *list)  // 根据当前模式取得正在使用的表的长度。
{
    if (list->mode == USE_SEQUENTIAL)  // 判断当前是否使用顺序表。
    {
        return ListLength_Sq(&list->sq);  // 调用顺序表的基本操作取得员工人数。
    }
    return ListLength_L(&list->link);  // 顺序表分支已返回，其余情况读取单链表长度。
}

static int findEmployee(const EmployeeList *list, const ElemType *key)
// 按工号查找员工，返回从 1 开始的位序或未找到时的 0。
{
    if (list->mode == USE_SEQUENTIAL)  // 根据模式选择使用哪种表的定位函数。
    {
        return LocateElem_Sq(&list->sq, key, CompareById);
        // 在顺序表中调用 LocateElem_Sq，比较条件传入 CompareById。
    }
    return LocateElem_L(&list->link, key, CompareById);
    // 在单链表中调用 LocateElem_L，使用同样的工号比较条件。
}

static int hire(EmployeeList *list, ElemType employee)
/* 入职成功返回 1，工号重复返回 0，插入失败返回 -1。 */  // 办理入职并检查工号唯一性，成功后将员工追加到表尾。
{
    if (findEmployee(list, &employee) != 0)  // 先用新员工的工号在当前表中查找。
    {
        return 0;  // 位序非零代表工号已存在，返回 0 供菜单提示重复。
    }
    int length = listLength(list);  // 读取表长，用来计算尾插的位序。
    if (length == INT_MAX)  // 判断再加 1 是否会超出 int 的范围。
    {
        return -1;  // 无法计算有效插入位序时报告插入失败。
    }

    Status inserted;  // 记录底层插入操作的状态。
    if (list->mode == USE_SEQUENTIAL)  // 根据模式决定操作的是顺序表还是单链表。
    {
        inserted = ListInsert_Sq(&list->sq, length + 1, employee);
        // 将记录插在顺序表的第 length 加 1 个位置，也就是表尾。
    }
    else  // 当前使用单链表，使用链表的插入操作。
    {
        inserted = ListInsert_L(&list->link, length + 1, employee);
        // 将员工接到单链表表尾，保存返回状态。
    }
    if (inserted)  // 把底层的插入状态转换为入职函数的返回值。
    {
        return 1;  // 插入成功，返回 1。
    }
    return -1;  // 底层操作失败时返回负值，与工号重复区分。
}

static Status dismiss(EmployeeList *list, const ElemType *key)
// 按工号办理离职，在找到记录后调用对应的删除函数。
{
    int i = findEmployee(list, key);  // 将工号换算成当前线性表中的位序。
    if (i == 0)  // 通过位序 0 判断没有这名员工。
    {
        return ERROR;  // 找不到工号时直接返回失败。
    }

    ElemType removed;  // 接收删除函数写回的原员工记录。
    if (list->mode == USE_SEQUENTIAL)  // 选择与当前存储方式对应的删除操作。
    {
        return ListDelete_Sq(&list->sq, i, &removed);
        // 从顺序表删除第 i 条记录，将状态返回给菜单。
    }
    return ListDelete_L(&list->link, i, &removed);
    // 从单链表删除第 i 条记录，将状态返回给菜单。
}

static Status query(const EmployeeList *list, const ElemType *key, ElemType *employee)
// 查找员工并通过输出参数返回完整记录。
{
    int i = findEmployee(list, key);  // 先按工号确定记录位序。
    if (i == 0)  // 判断查找是否成功。
    {
        return ERROR;  // 找不到员工时返回失败，输出参数不使用。
    }
    if (list->mode == USE_SEQUENTIAL)  // 根据当前模式选择取元素的实现。
    {
        return GetElem_Sq(&list->sq, i, employee);
        // 以位序 i 从顺序表复制出员工记录。
    }
    return GetElem_L(&list->link, i, employee);  // 以位序 i 从单链表读取员工记录。
}

static void printRoster(const EmployeeList *list)  // 输出在职员工人数和当前名单。
{
    int count = listLength(list);  // 先从实际使用的表取得人数。
    printf("在职员工人数：%d\n", count);  // 打印人数，空表时也会显示 0。
    if (count == 0)  // 判断当前名单是否为空。
    {
        puts("暂无员工。");  // 空表时显示提示文字。
        return;  // 此时没有记录可打印，提前结束。
    }

    puts("姓名 | 职务 | 工号");  // 输出三列标题，与单条记录的打印顺序对应。
    if (list->mode == USE_SEQUENTIAL)  // 根据存储方式选择遍历函数。
    {
        ListTraverse_Sq(&list->sq, PrintEmployee);
        // 遍历顺序表，每条记录都交给 PrintEmployee 打印。
    }
    else  // 当前使用单链表，改走链表遍历。
    {
        ListTraverse_L(&list->link, PrintEmployee);
        // 沿单链表逐条调用 PrintEmployee 输出记录。
    }
}

static int loadEmployees(EmployeeList *list, int count)
// 按指定人数读取初始员工，并调用入职函数存入当前表。
{
    int i = 0;  // 用 i 记录已经成功存入的初始员工数量。
    while (i < count)  // 未达到预定人数时继续录入。
    {
        ElemType employee =  // 创建这一次录入使用的临时员工记录。
        {
            "", "", ""  // 将姓名、职务和工号三个字符数组初始化为空字符串。
        }
        ;
        printf("\n录入第 %d 名初始员工\n", i + 1);  // 按从 1 开始的顺序提示当前录到第几名员工。
        if (!readEmployee(&employee))  // 读取包含姓名、职务和工号的完整员工记录。
        {
            return 0;  // 尚未录完就遇到 EOF 时取消初始化录入。
        }
        int result = hire(list, employee);  // 使用和菜单入职相同的查重和尾插逻辑。
        if (result < 0)  // 负值表示插入过程中出错。
        {
            puts("录入失败，无法继续添加员工。");  // 通知用户初始名单无法继续录入。
            return 0;  // 结束录入，将失败状态交给 main。
        }
        if (result == 0)  // 返回 0 代表工号重复，当前员工没有录入。
        {
            puts("工号已存在，请重新录入该员工。");  // 提示重新录入这一名员工。
        }
        else  // 插入成功才增加已录入人数。
        {
            i++;  // 将成功计数加 1，下一轮录入后续员工。
        }
    }
    return 1;  // 达到指定人数后返回成功。
}

int main(void)  // 程序入口，控制存储方式选择、初始化、菜单循环和退出清理。
{
    puts("线性表上机 · 员工管理\n"  // 起始页面显示程序名称及存储方式选项。
         "1. 顺序表\n"  // 菜单选项 1 对应顺序表。
         "2. 单链表\n"  // 菜单选项 2 对应单链表。
         "0. 退出");  // 菜单选项 0 可以直接退出程序。

    int mode;  // 用 mode 接收用户选中的菜单编号。
    if (!readNumber("请选择存储方式：", USE_LINKED, 0, &mode) || mode == 0)
    // 读取 0 到 2 的选项，遇到输入结束或选择 0 时退出。
    {
        puts("\n程序结束。");  // 显示程序结束的信息。
        return 0;  // 退出入口函数，返回正常结束状态。
    }

    const char *modeName = "单链表";  // 默认为所选链表模式准备显示名称。
    if (mode == USE_SEQUENTIAL)  // 如果菜单选的是顺序表，就改用顺序表的显示名。
    {
        modeName = "顺序表";  // 将显示名称设置成顺序表。
    }
    printf("\n当前存储方式：%s\n", modeName);  // 把选中的存储方式显示给用户。
    int count;  // 用 count 接收启动时需要录入的员工总数。
    if (!readNumber("请输入初始员工人数：", INT_MAX, 0, &count))
    // 读取非负员工人数，遇到 EOF 时终止程序。
    {
        puts("\n程序结束。");  // 还未创建表，直接提示程序结束。
        return 0;  // 结束程序，此时没有需要销毁的表。
    }

    EmployeeList list;  // 创建本次会话使用的员工表管理变量。
    list.mode = mode;  // 保存用户选择，入职、离职等函数会据此选择底层操作。
    Status initialized;  // 用 initialized 记录表初始化的结果。
    if (mode == USE_SEQUENTIAL)  // 选择顺序表时只初始化顺序表字段。
    {
        initialized = InitList_Sq(&list.sq);  // 为顺序表申请初始数组空间。
    }
    else  // 当前选择了单链表。
    {
        initialized = InitList_L(&list.link);  // 为单链表创建一个空的头结点。
    }
    if (!initialized)  // 确认所选的表初始化成功，之后才能录入记录。
    {
        puts("初始化失败，无法建立员工表。");  // 初始化失败时显示原因并停止。
        puts("\n程序结束。");  // 打印退出提示。
        return 0;  // 结束程序，不进入业务菜单。
    }

    int remaining = 0;  // 初始化剩余操作次数，后续由输入结果覆盖。
    int ready = loadEmployees(&list, count) &&  // 先录入初始员工，成功后才读取操作次数。
                readNumber("请输入操作次数（直接回车可连续操作）：", INT_MAX, 1, &remaining);
                // 允许空行，直接回车时 readNumber 返回不限次数标记。

    while (ready && remaining != 0)
    // 只有初始化录入和次数读取都成功，且剩余次数还没到 0，才继续菜单。
    {
        puts("\n员工管理菜单\n"  // 输出本轮员工管理菜单。
             "1. 员工入职\n"  // 选项 1 办理入职。
             "2. 员工离职\n"  // 选项 2 办理离职。
             "3. 查询员工\n"  // 选项 3 根据工号查询。
             "4. 查看员工名单\n"  // 选项 4 查看当前在职名单。
             "0. 退出");  // 选项 0 退出业务菜单。

        int choice;  // 用 choice 保存本轮选择。
        if (!readNumber("请选择操作：", 4, 0, &choice) || choice == 0)
        // 读入 0 到 4 的选项，输入结束或选择 0 时结束循环。
        {
            break;  // 跳出菜单，转到最终输出和资源清理。
        }
        if (choice == 4)  // 单独处理查看名单，因为这项不占入职、离职或查询的次数。
        {
            printRoster(&list);  // 输出当前名单。
            continue;  // 直接开始下一轮菜单，跳过本轮的次数扣减。
        }

        if (choice == 1)  // 根据选项 1 进入入职流程。
        {
            ElemType employee =  // 创建临时员工变量，等待读入入职信息。
            {
                "", "", ""  // 将临时员工记录的三个文本字段都设为空字符串。
            }
            ;
            if (!readEmployee(&employee))  // 依次读取新员工的姓名、职务和工号。
            {
                break;  // 输入结束时停止菜单，本次未录完整的员工不会被加入。
            }
            int result = hire(&list, employee);
            // 把输入交给 hire 查重和插入，保存返回结果。
            if (result == 1)  // 结果为 1 代表入职成功。
            {
                puts("入职成功。");  // 向用户确认员工已经加入。
            }
            else if (result == 0)  // 结果为 0 代表工号已存在。
            {
                puts("入职失败，工号已存在。");  // 提示工号重复，本次入职未生效。
            }
            else  // 剩下的结果表示插入出错。
            {
                puts("入职失败，无法添加员工。");  // 向用户报告本次无法添加员工。
            }
        }
        else  // 剩余菜单选项是离职和查询，两者都先读取工号。
        {
            ElemType key =  // 准备查找条件，只使用这条临时记录的工号字段。
            {
                "", "", ""  // 将查找条件的三个字段先置为空字符串。
            }
            ;
            if (!readField("请输入工号：", key.id, EMPLOYEE_ID_SIZE))
            // 读取要查找的工号，并写入 key.id。
            {
                break;  // 读不到工号就退出循环，本轮不执行离职或查询。
            }
            if (choice == 2)  // 选项 2 按工号办理离职。
            {
                if (dismiss(&list, &key))
                // 调用 dismiss 定位记录并删除，判断操作是否成功。
                {
                    puts("离职成功。");  // 删除成功后提示已离职。
                }
                else  // 删除操作未成功时给出工号未找到的提示。
                {
                    puts("离职失败，未找到该工号。");  // 提示列表中没有对应工号。
                }
            }
            else  // 余下的有效选项是查询工号。
            {
                ElemType employee;  // 准备接收查询返回的完整员工记录。
                if (!query(&list, &key, &employee))
                // 调用 query，找到的记录会写入 employee。
                {
                    puts("未找到该工号。");  // 查找失败时提示未找到该工号。
                }
                else  // 查询成功时可以使用 employee 中的记录。
                {
                    printf("查询结果（姓名 | 职务 | 工号）：");  // 先说明接下来三个字段的顺序。
                    PrintEmployee(&employee);  // 打印通过查询取得的员工记录。
                }
            }
        }

        if (remaining > 0)  // 只在剩余次数为正时扣减，不限次数模式的负值保持不变。
        {
            remaining--;  // 一次入职、离职或查询结束后将剩余次数减 1。
        }
    }

    if (ready)  // 已完成初始录入和次数设置时才输出最终名单。
    {
        puts("\n最终员工名单");  // 标明接下来打印的是结束时的员工名单。
        printRoster(&list);  // 输出最终表长及员工记录。
    }

    if (mode == USE_SEQUENTIAL)  // 选择顺序表时销毁它使用的连续数组。
    {
        DestroyList_Sq(&list.sq);  // 调用顺序表的销毁函数回收存储空间。
    }
    else  // 选择单链表时改为回收链表结点。
    {
        DestroyList_L(&list.link);  // 调用单链表的销毁函数逐个释放结点。
    }
    puts("\n程序结束。");  // 在清理完成后显示程序结束。
    return 0;  // 向操作系统返回 0，表示程序正常结束。
}
