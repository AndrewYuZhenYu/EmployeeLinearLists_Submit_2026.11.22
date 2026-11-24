#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPLOYEE_NAME_SIZE 64
#define EMPLOYEE_POSITION_SIZE 128
#define EMPLOYEE_ID_SIZE 64

typedef int Status;
#define OK 1
#define ERROR 0

typedef struct
{
    char name[EMPLOYEE_NAME_SIZE];
    char position[EMPLOYEE_POSITION_SIZE];
    char id[EMPLOYEE_ID_SIZE];
}
ElemType;

typedef struct LNode
{
    ElemType data;
    struct LNode *next;
}
LNode;

typedef struct
{
    LNode *head;
    LNode *tail;
    int len;
}
LinkList;

static void PrintEmployee(const ElemType *e)
{
    printf("%s | %s | %s\n", e->name, e->position, e->id);
}

Status InitList_L(LinkList *L)
{
    L->head = (LNode *) malloc(sizeof(LNode));
    if (L->head == NULL)
        return ERROR;
    L->head->next = NULL;
    L->tail = L->head;
    L->len = 0;
    return OK;
}

void DestroyList_L(LinkList *L)
{
    LNode *p = L->head;
    while (p != NULL)
    {
        LNode *q = p->next;
        free(p);
        p = q;
    }
    L->head = NULL;
    L->tail = NULL;
    L->len = 0;
}

Status GetElem_L(const LinkList *L, int i, ElemType *e)
{
    if (i < 1 || i > L->len)
        return ERROR;

    const LNode *p = L->head->next;
    for (int j = 1; j < i; j++)
        p = p->next;
    *e = p->data;
    return OK;
}

static int LocateElem_L(const LinkList *L, const char *id)
{
    int i = 0;
    for (const LNode *p = L->head->next; p != NULL; p = p->next)
    {
        i++;
        if (strcmp(p->data.id, id) == 0)
            return i;
    }
    return 0;
}

Status ListInsert_L(LinkList *L, int i, ElemType e)
{
    if (i < 1 || i - 1 > L->len || L->len == INT_MAX)
        return ERROR;

    LNode *p = L->tail;
    if (i - 1 != L->len)
    {
        p = L->head;
        for (int j = 0; j < i - 1; j++)
            p = p->next;
    }

    LNode *s = (LNode *) malloc(sizeof(LNode));
    if (s == NULL)
        return ERROR;
    s->data = e;
    s->next = p->next; // 保留前驱原来的后继。
    p->next = s;
    if (L->tail == p)
        L->tail = s;
    L->len++;
    return OK;
}

Status ListDelete_L(LinkList *L, int i, ElemType *e)
{
    if (i < 1 || i > L->len)
        return ERROR;

    LNode *p = L->head;
    for (int j = 0; j < i - 1; j++)
        p = p->next;
    LNode *q = p->next;
    p->next = q->next;
    if (L->tail == q)
        L->tail = p;
    *e = q->data;
    free(q);
    L->len--;
    return OK;
}

static void ListTraverse_L(const LinkList *L)
{
    printf("在职员工人数：%d\n", L->len);
    if (L->len == 0)
    {
        puts("暂无员工。");
        return;
    }
    puts("姓名 | 职务 | 工号");
    for (const LNode *p = L->head->next; p != NULL; p = p->next)
        PrintEmployee(&p->data);
}

static int readText(const char *prompt, char *text, int size, int allowEmpty)
{
    for (;;)
    {
        printf("%s", prompt);
        fflush(stdout);
        int ch, length = 0, tooLong = 0;
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
            if (ch == '\r')
                continue;
            if (length < size - 1)
                text[length++] = (char) ch;
            else
                tooLong = 1;
        }

        if (ch == EOF && length == 0)
            return 0;
        text[length] = '\0';
        if (tooLong)
            printf("输入过长，最多 %d 字节，请重试。\n", size - 1);
        else if (allowEmpty || text[0] != '\0')
            return 1;
        else
            puts("输入不能为空，请重试。");
    }
}

static int readNumber(const char *prompt, int maximum, int allowBlank, int *value)
{
    char text[64];
    while (readText(prompt, text, sizeof(text), 1))
    {
        if (allowBlank && text[0] == '\0')
        {
            *value = -1; // 不限操作次数。
            return 1;
        }

        char *end;
        errno = 0;
        long n = strtol(text, &end, 10);
        if (end != text && errno == 0 && n >= 0 && n <= maximum &&
            end[strspn(end, " \t")] == '\0')
        {
            *value = (int) n;
            return 1;
        }
        printf("输入无效，请输入 0 到 %d 之间的整数。\n", maximum);
    }
    return 0;
}

static int readEmployee(ElemType *employee)
{
    memset(employee, 0, sizeof(*employee));
    return readText("姓名：", employee->name, EMPLOYEE_NAME_SIZE, 0) &&
           readText("职务：", employee->position, EMPLOYEE_POSITION_SIZE, 0) &&
           readText("工号：", employee->id, EMPLOYEE_ID_SIZE, 0);
}

static int hire(LinkList *list, ElemType employee)
{
    if (LocateElem_L(list, employee.id) != 0)
        return 0;

    if (list->len == INT_MAX || !ListInsert_L(list, list->len + 1, employee))
        return -1;
    return 1;
}

static Status dismiss(LinkList *list, const char *id)
{
    int i = LocateElem_L(list, id);
    ElemType removed;
    return ListDelete_L(list, i, &removed);
}

static Status query(const LinkList *list, const char *id,
                    ElemType *employee)
{
    int i = LocateElem_L(list, id);
    return GetElem_L(list, i, employee);
}

static int loadEmployees(LinkList *list, int count)
{
    int i = 0;
    while (i < count)
    {
        ElemType employee;
        printf("\n录入第 %d 名初始员工\n", i + 1);
        if (!readEmployee(&employee))
            return 0;
        int result = hire(list, employee);
        if (result < 0)
        {
            puts("录入失败，无法继续添加员工。");
            return 0;
        }
        if (result == 0)
            puts("工号已存在，请重新录入该员工。");
        else
            i++;
    }
    return 1;
}

int main(void)
{
    puts("线性表 · 员工管理");
    puts("\n当前存储方式：单链表");
    int count;
    if (!readNumber("请输入初始员工人数：", INT_MAX, 0, &count))
    {
        puts("\n程序结束。");
        return 0;
    }

    LinkList list;
    if (!InitList_L(&list))
    {
        puts("初始化失败，无法建立员工表。");
        puts("\n程序结束。");
        return 0;
    }

    int remaining = 0;
    int ready = loadEmployees(&list, count) &&
                readNumber("请输入操作次数（直接回车可连续操作）：", INT_MAX, 1, &remaining);

    while (ready && remaining != 0)
    {
        puts("\n员工管理菜单\n1. 员工入职\n2. 员工离职\n"
            "3. 查询员工\n4. 查看员工名单\n0. 退出");

        int choice;
        if (!readNumber("请选择操作：", 4, 0, &choice) || choice == 0)
            break;
        if (choice == 4)
        {
            ListTraverse_L(&list);
            continue;
        }

        if (choice == 1)
        {
            ElemType employee;
            if (!readEmployee(&employee))
                break;
            int result = hire(&list, employee);
            if (result == 1)
                puts("入职成功。");
            else if (result == 0)
                puts("入职失败，工号已存在。");
            else
                puts("入职失败，无法添加员工。");
        } else
        {
            char id[EMPLOYEE_ID_SIZE];
            if (!readText("请输入工号：", id, EMPLOYEE_ID_SIZE, 0))
                break;
            if (choice == 2)
            {
                if (dismiss(&list, id))
                    puts("离职成功。");
                else
                    puts("离职失败，未找到该工号。");
            } else
            {
                ElemType employee;
                if (!query(&list, id, &employee))
                    puts("未找到该工号。");
                else
                {
                    printf("查询结果（姓名 | 职务 | 工号）：");
                    PrintEmployee(&employee);
                }
            }
        }

        if (remaining > 0)
            remaining--;
    }

    if (ready)
    {
        puts("\n最终员工名单");
        ListTraverse_L(&list);
    }

    DestroyList_L(&list);
    puts("\n程序结束。");
    return 0;
}
