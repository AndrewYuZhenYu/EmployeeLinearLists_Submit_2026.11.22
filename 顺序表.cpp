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

#define LIST_INIT_SIZE 80
#define LISTINCREMENT 10

typedef struct
{
    ElemType *elem;
    int length;
    int listsize;
}
SqList;

static void PrintEmployee(const ElemType *e)
{
    printf("%s | %s | %s\n", e->name, e->position, e->id);
}

Status InitList_Sq(SqList *L)
{
    L->elem = (ElemType *) malloc(LIST_INIT_SIZE * sizeof(ElemType));
    if (L->elem == NULL)
        return ERROR;
    L->length = 0;
    L->listsize = LIST_INIT_SIZE;
    return OK;
}

void DestroyList_Sq(SqList *L)
{
    free(L->elem);
    L->elem = NULL;
    L->length = 0;
    L->listsize = 0;
}

static Status ExtendList_Sq(SqList *L)
{
    if (L->listsize > INT_MAX - LISTINCREMENT)
        return ERROR;

    int listsize = L->listsize + LISTINCREMENT;
    if ((size_t) listsize > (size_t) -1 / sizeof(ElemType))
        return ERROR;
    ElemType *newbase = (ElemType *) malloc((size_t) listsize * sizeof(ElemType));
    if (newbase == NULL)
        return ERROR;

    for (int j = 0; j < L->length; j++)
        newbase[j] = L->elem[j];
    free(L->elem);
    L->elem = newbase;
    L->listsize = listsize;
    return OK;
}

Status GetElem_Sq(const SqList *L, int i, ElemType *e)
{
    if (i < 1 || i > L->length)
        return ERROR;
    *e = L->elem[i - 1];
    return OK;
}

static int LocateElem_Sq(const SqList *L, const char *id)
{
    for (int j = 0; j < L->length; j++)
    {
        if (strcmp(L->elem[j].id, id) == 0)
            return j + 1;
    }
    return 0;
}

Status ListInsert_Sq(SqList *L, int i, ElemType e)
{
    if (i < 1 || i - 1 > L->length)
        return ERROR;
    if (L->length == L->listsize && !ExtendList_Sq(L))
        return ERROR;

    for (int j = L->length; j >= i; j--) // 从后向前搬。
        L->elem[j] = L->elem[j - 1];
    L->elem[i - 1] = e;
    L->length++;
    return OK;
}

Status ListDelete_Sq(SqList *L, int i, ElemType *e)
{
    if (i < 1 || i > L->length)
        return ERROR;

    *e = L->elem[i - 1];
    for (int j = i; j < L->length; j++)
        L->elem[j - 1] = L->elem[j];
    L->length--;
    return OK;
}

static void ListTraverse_Sq(const SqList *L)
{
    printf("在职员工人数：%d\n", L->length);
    if (L->length == 0)
    {
        puts("暂无员工。");
        return;
    }
    puts("姓名 | 职务 | 工号");
    for (int j = 0; j < L->length; j++)
        PrintEmployee(&L->elem[j]);
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

static int hire(SqList *list, ElemType employee)
{
    if (LocateElem_Sq(list, employee.id) != 0)
        return 0;

    if (list->length == INT_MAX || !ListInsert_Sq(list, list->length + 1, employee))
        return -1;
    return 1;
}

static Status dismiss(SqList *list, const char *id)
{
    int i = LocateElem_Sq(list, id);
    ElemType removed;
    return ListDelete_Sq(list, i, &removed);
}

static Status query(const SqList *list, const char *id,
                    ElemType *employee)
{
    int i = LocateElem_Sq(list, id);
    return GetElem_Sq(list, i, employee);
}

static int loadEmployees(SqList *list, int count)
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
    puts("\n当前存储方式：顺序表");
    int count;
    if (!readNumber("请输入初始员工人数：", INT_MAX, 0, &count))
    {
        puts("\n程序结束。");
        return 0;
    }

    SqList list;
    if (!InitList_Sq(&list))
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
            ListTraverse_Sq(&list);
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
        ListTraverse_Sq(&list);
    }

    DestroyList_Sq(&list);
    puts("\n程序结束。");
    return 0;
}
