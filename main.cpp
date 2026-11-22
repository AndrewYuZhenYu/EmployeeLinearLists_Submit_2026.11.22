#include "linked_list.h"  // 引入单链表的类型和操作声明，菜单可据此调用单链表实现。
#include "sequential_list.h"  // 引入顺序表的类型和操作声明。

#include <limits.h>  // 提供 INT_MAX，用来限制输入数字和避免整数溢出。
#include <stdio.h>  // 提供终端输入、提示和打印需要的 C 标准函数。

#define USE_SEQUENTIAL 1  // 用菜单选项 1 表示顺序表。
#define USE_LINKED 2  // 用菜单选项 2 表示单链表。

typedef struct {  // 声明菜单操作使用的管理结构，将模式与两种表的存储字段放在一起。
    int mode;  // 记下当前选择，后续操作按它调用对应的表函数。
    SqList sq;  // 选择顺序表时使用这张表。
    LinkList link;  // 选择单链表时使用这张表。
} EmployeeList;  // 为员工表管理结构定义类型名 EmployeeList。

static int readLine(const char *prompt, char *text, int size) {
/* 正常读入返回 1，输入结束返回 0，超长返回 -1。 */  // 从标准输入读一行并写入 text，size 是缓冲区总容量。
    printf("%s", prompt);  // 把传入的提示文字显示在终端。
    fflush(stdout);  // 提示没有换行，刷新 stdout 确保等待输入前就能看到提示。

    int length = 0;  // 记录已经写进缓冲区的字节数。
    int tooLong = 0;  // 记录本行是否超过缓冲区可接受的长度。
    int ch;  // 用 int 保存读取结果，以便区分任意字节和 EOF。
    while ((ch = getchar()) != '\n' && ch != EOF) {
    // 逐字节读取，遇到换行或输入结束时停止。
        if (ch == '\r') {  // 识别回车字节，以兼容以回车加换行结尾的文本。
            continue;  // 跳过回车，继续读同一行。
        }
        if (length < size - 1) {  // 留出最后一个字节作为字符串结束符，剩余空间才能存内容。
            text[length] = (char)ch;  // 将一个有效范围内的输入字节写入当前下标。
            length++;  // 已写入长度加 1，指向下一个空位。
        } else {  // 缓冲区满了仍继续消耗本行，防止剩余字符被当成下一次回答。
            tooLong = 1;  // 标记本行超长，下一步返回错误。
        }
    }
    if (ch == EOF && length == 0) {  // 判断这次是否在没有读到内容的情况下遇到 EOF。
        return 0;  // 返回 0，通知上层停止读取。
    }
    if (tooLong) {  // 这一行已经全部读过，现在处理超长情况。
        printf("输入过长，最多 %d 字节，请重试。\n", size - 1);
        // 提示最大内容字节数，空出一字节用于结束符。
        return -1;  // 返回无效输入状态。
    }

    int start = 0;  // 从行首的下标 0 开始找有效内容。
    while (start < length && (text[start] == ' ' || text[start] == '\t')) {
    // 逐个检查开头的空格和制表符。
        start++;  // 将起始下标向后移，略过一个行首空白。
    }
    while (length > start && (text[length - 1] == ' ' || text[length - 1] == '\t')) {
    // 从行尾往前检查空白，但不越过有效内容起点。
        length--;  // 缩短有效内容的结束位置，丢弃一个行尾空白。
    }
    int i = 0;  // 准备从缓冲区下标 0 开始保存去掉首尾空白后的文本。
    while (start < length) {  // 遍历第一个有效字符到修剪后的行尾。
        text[i] = text[start];  // 将一个有效字符移到新位置，保留内容中的内部空格。
        i++;  // 将写入位置向后推进一格。
        start++;  // 将读取位置向后推进一格。
    }
    text[i] = '\0';  // 在有效文本末尾补上字符串结束符。
    return 1;  // 返回 1，表示这次成功读取一行。
}

static int readNumber(const char *prompt, int maximum, int allowBlank, int *result) {
// 读取范围在 0 到 maximum 之间的整数，通过 result 交回结果。
    char text[64];  // 为待解析的数字文本准备一个 64 字节缓冲区。
    int status;  // 保存 readLine 的状态，用来区分成功、无效和输入结束。
    while ((status = readLine(prompt, text, sizeof(text))) != 0) {
    // 持续提示并读取，直到收到可用的整数或遇到 EOF。
        if (status < 0) {  // 检查这次读取的行是否被判为无效。
            continue;  // 跳过无效行，重新提示输入。
        }
        if (allowBlank && text[0] == '\0') {  // 只有调用者允许时，空行才能作为有效选择。
            *result = -1;  /* 不限操作次数。 */  // 用负值表示不限操作次数，与正常的非负次数区分。
            return 1;  // 告诉调用者已接受空行这一选择。
        }

        int i = 0;  // 从文本首字符开始解析。
        if (text[0] == '+' || text[0] == '-') {
        // 识别可选的正负号，便于之后验证输入是否为非负数。
            i++;  // 数字解析从符号后面开始。
        }
        int start = i;  // 记住首个应当出现数字的下标。
        int value = 0;  // 初始化累计值，随后逐位构成十进制整数。
        while (text[i] >= '0' && text[i] <= '9') {
        // 只要当前位置还是十进制数字，就继续读取。
            int digit = text[i] - '0';  // 用字符减去字符 0，把数字字符转成对应的整数。
            if (value > (INT_MAX - digit) / 10) {
            // 执行乘 10 加下一位前，先判断结果是否会超出 int 上限。
                break;  // 无法安全累加时停止，未解析的数字会使后面的完整性检查失败。
            }
            value = value * 10 + digit;  // 将已解析的值左移一位十进制，再补上当前数字。
            i++;  // 移到下一个数字。
        }

        if (i > start && text[i] == '\0' && value <= maximum &&
        // 要求至少有一位数字、整行已经解析完，并且结果不超过指定的上限。
            (text[0] != '-' || value == 0)) {
            // 排除带负号且值非零的输入，允许带符号的零。
            *result = value;  // 把验证通过的整数写入调用者提供的地址。
            return 1;  // 返回 1，表示这次读数成功。
        }
        printf("输入无效，请输入 0 到 %d 之间的整数。\n", maximum);
        // 输入格式或数值不符合条件时，说明允许的范围。
    }
    return 0;  // 输入结束后返回 0，调用者据此退出当前流程。
}

static int readField(const char *prompt, char *text, int size) {
// 读取一项不能为空的文本，例如姓名或工号。
    int status;  // 保存本轮读入的状态。
    while ((status = readLine(prompt, text, size)) != 0) {
    // 遇到空白或无效输入时可以重试，遇到 EOF 才结束。
        if (status < 0) {  // 检查刚读到的整行是否有效。
            continue;  // 丢弃无效输入并重新提示。
        }
        if (text[0] != '\0') {  // 用第一个字节是否是结束符来判断字符串是否为空。
            return 1;  // 字段有内容，向调用者报告读取成功。
        }
        puts("输入不能为空，请重试。");  // 空行或只有首尾空白的行需要重新输入。
    }
    return 0;  // 到达 EOF，报告本字段未读到。
}

static int readEmployee(ElemType *employee) {  // 逐项读取一名员工的完整记录。
    return readField("姓名：", employee->name, EMPLOYEE_NAME_SIZE) &&
    // 先读取姓名，成功后才会继续执行右侧的读取。
           readField("职务：", employee->position, EMPLOYEE_POSITION_SIZE) &&
           // 接着读取职务，前一项失败时这一项不会执行。
           readField("工号：", employee->id, EMPLOYEE_ID_SIZE);
           // 最后读工号，三项都成功时才整体返回 1。
}

static int listLength(const EmployeeList *list) {
// 根据当前模式取得正在使用的表的长度。
    if (list->mode == USE_SEQUENTIAL) {  // 判断当前是否使用顺序表。
        return ListLength_Sq(&list->sq);  // 调用顺序表的基本操作取得员工人数。
    }
    return ListLength_L(&list->link);  // 顺序表分支已返回，其余情况读取单链表长度。
}

static int findEmployee(const EmployeeList *list, const ElemType *key) {
// 按工号查找员工，返回从 1 开始的位序或未找到时的 0。
    if (list->mode == USE_SEQUENTIAL) {  // 根据模式选择使用哪种表的定位函数。
        return LocateElem_Sq(&list->sq, key, CompareById);
        // 在顺序表中调用 LocateElem_Sq，比较条件传入 CompareById。
    }
    return LocateElem_L(&list->link, key, CompareById);
    // 在单链表中调用 LocateElem_L，使用同样的工号比较条件。
}

static int hire(EmployeeList *list, ElemType employee) {
/* 入职成功返回 1，工号重复返回 0，插入失败返回 -1。 */  // 办理入职并检查工号唯一性，成功后将员工追加到表尾。
    if (findEmployee(list, &employee) != 0) {  // 先用新员工的工号在当前表中查找。
        return 0;  // 位序非零代表工号已存在，返回 0 供菜单提示重复。
    }
    int length = listLength(list);  // 读取表长，用来计算尾插的位序。
    if (length == INT_MAX) {  // 判断再加 1 是否会超出 int 的范围。
        return -1;  // 无法计算有效插入位序时报告插入失败。
    }

    Status inserted;  // 记录底层插入操作的状态。
    if (list->mode == USE_SEQUENTIAL) {  // 根据模式决定操作的是顺序表还是单链表。
        inserted = ListInsert_Sq(&list->sq, length + 1, employee);
        // 将记录插在顺序表的第 length 加 1 个位置，也就是表尾。
    } else {  // 当前使用单链表，使用链表的插入操作。
        inserted = ListInsert_L(&list->link, length + 1, employee);
        // 将员工接到单链表表尾，保存返回状态。
    }
    return inserted ? 1 : -1;  // 底层插入成功返回 1，失败返回负值，与重复工号的 0 区分。
}

static Status dismiss(EmployeeList *list, const ElemType *key) {
// 按工号办理离职，在找到记录后调用对应的删除函数。
    int i = findEmployee(list, key);  // 将工号换算成当前线性表中的位序。
    if (i == 0) {  // 通过位序 0 判断没有这名员工。
        return ERROR;  // 找不到工号时直接返回失败。
    }

    ElemType removed;  // 接收删除函数写回的原员工记录。
    if (list->mode == USE_SEQUENTIAL) {  // 选择与当前存储方式对应的删除操作。
        return ListDelete_Sq(&list->sq, i, &removed);
        // 从顺序表删除第 i 条记录，将状态返回给菜单。
    }
    return ListDelete_L(&list->link, i, &removed);
    // 从单链表删除第 i 条记录，将状态返回给菜单。
}

static Status query(const EmployeeList *list, const ElemType *key, ElemType *employee) {
// 查找员工并通过输出参数返回完整记录。
    int i = findEmployee(list, key);  // 先按工号确定记录位序。
    if (i == 0) {  // 判断查找是否成功。
        return ERROR;  // 找不到员工时返回失败，输出参数不使用。
    }
    if (list->mode == USE_SEQUENTIAL) {  // 根据当前模式选择取元素的实现。
        return GetElem_Sq(&list->sq, i, employee);
        // 以位序 i 从顺序表复制出员工记录。
    }
    return GetElem_L(&list->link, i, employee);  // 以位序 i 从单链表读取员工记录。
}

static void printRoster(const EmployeeList *list) {  // 输出在职员工人数和当前名单。
    int count = listLength(list);  // 先从实际使用的表取得人数。
    printf("在职员工人数：%d\n", count);  // 打印人数，空表时也会显示 0。
    if (count == 0) {  // 判断当前名单是否为空。
        puts("暂无员工。");  // 空表时显示提示文字。
        return;  // 此时没有记录可打印，提前结束。
    }

    puts("姓名 | 职务 | 工号");  // 输出三列标题，与单条记录的打印顺序对应。
    if (list->mode == USE_SEQUENTIAL) {  // 根据存储方式选择遍历函数。
        ListTraverse_Sq(&list->sq, PrintEmployee);
        // 遍历顺序表，每条记录都交给 PrintEmployee 打印。
    } else {  // 当前使用单链表，改走链表遍历。
        ListTraverse_L(&list->link, PrintEmployee);
        // 沿单链表逐条调用 PrintEmployee 输出记录。
    }
}

static int loadEmployees(EmployeeList *list, int count) {
// 按指定人数读取初始员工，并调用入职函数存入当前表。
    int i = 0;  // 用 i 记录已经成功存入的初始员工数量。
    while (i < count) {  // 未达到预定人数时继续录入。
        ElemType employee = {"", "", ""};
        // 创建空的临时记录，随后逐项填写这名初始员工的信息。
        printf("\n录入第 %d 名初始员工\n", i + 1);  // 按从 1 开始的顺序提示当前录到第几名员工。
        if (!readEmployee(&employee)) {  // 读取包含姓名、职务和工号的完整员工记录。
            return 0;  // 尚未录完就遇到 EOF 时取消初始化录入。
        }
        int result = hire(list, employee);  // 使用和菜单入职相同的查重和尾插逻辑。
        if (result < 0) {  // 负值表示插入过程中出错。
            puts("录入失败，无法继续添加员工。");  // 通知用户初始名单无法继续录入。
            return 0;  // 结束录入，将失败状态交给 main。
        }
        if (result == 0) {  // 返回 0 代表工号重复，当前员工没有录入。
            puts("工号已存在，请重新录入该员工。");  // 提示重新录入这一名员工。
        } else {  // 插入成功才增加已录入人数。
            i++;  // 将成功计数加 1，下一轮录入后续员工。
        }
    }
    return 1;  // 达到指定人数后返回成功。
}

int main(void) {  // 程序入口，控制存储方式选择、初始化、菜单循环和退出清理。
    puts("线性表上机 · 员工管理\n"  // 起始页面显示程序名称及存储方式选项。
         "1. 顺序表\n"  // 菜单选项 1 对应顺序表。
         "2. 单链表\n"  // 菜单选项 2 对应单链表。
         "0. 退出");  // 菜单选项 0 可以直接退出程序。

    int mode;  // 用 mode 接收用户选中的菜单编号。
    if (!readNumber("请选择存储方式：", USE_LINKED, 0, &mode) || mode == 0) {
    // 读取 0 到 2 的选项，遇到输入结束或选择 0 时退出。
        puts("\n程序结束。");  // 显示程序结束的信息。
        return 0;  // 退出入口函数，返回正常结束状态。
    }

    printf("\n当前存储方式：%s\n", mode == USE_SEQUENTIAL ? "顺序表" : "单链表");
    // 根据 mode 选择对应的存储方式名称并显示。
    int count;  // 用 count 接收启动时需要录入的员工总数。
    if (!readNumber("请输入初始员工人数：", INT_MAX, 0, &count)) {
    // 读取非负员工人数，遇到 EOF 时终止程序。
        puts("\n程序结束。");  // 还未创建表，直接提示程序结束。
        return 0;  // 结束程序，此时没有需要销毁的表。
    }

    EmployeeList list;  // 创建本次会话使用的员工表管理变量。
    list.mode = mode;  // 保存用户选择，入职、离职等函数会据此选择底层操作。
    Status initialized;  // 用 initialized 记录表初始化的结果。
    if (mode == USE_SEQUENTIAL) {  // 选择顺序表时只初始化顺序表字段。
        initialized = InitList_Sq(&list.sq);  // 为顺序表申请初始数组空间。
    } else {  // 当前选择了单链表。
        initialized = InitList_L(&list.link);  // 为单链表创建一个空的头结点。
    }
    if (!initialized) {  // 确认所选的表初始化成功，之后才能录入记录。
        puts("初始化失败，无法建立员工表。");  // 初始化失败时显示原因并停止。
        puts("\n程序结束。");  // 打印退出提示。
        return 0;  // 结束程序，不进入业务菜单。
    }

    int remaining = 0;  // 初始化剩余操作次数，后续由输入结果覆盖。
    int ready = loadEmployees(&list, count) &&  // 先录入初始员工，成功后才读取操作次数。
                readNumber("请输入操作次数（直接回车可连续操作）：", INT_MAX, 1, &remaining);
                // 允许空行，直接回车时 readNumber 返回不限次数标记。

    while (ready && remaining != 0) {
    // 只有初始化录入和次数读取都成功，且剩余次数还没到 0，才继续菜单。
        puts("\n员工管理菜单\n"  // 输出本轮员工管理菜单。
             "1. 员工入职\n"  // 选项 1 办理入职。
             "2. 员工离职\n"  // 选项 2 办理离职。
             "3. 查询员工\n"  // 选项 3 根据工号查询。
             "4. 查看员工名单\n"  // 选项 4 查看当前在职名单。
             "0. 退出");  // 选项 0 退出业务菜单。

        int choice;  // 用 choice 保存本轮选择。
        if (!readNumber("请选择操作：", 4, 0, &choice) || choice == 0) {
        // 读入 0 到 4 的选项，输入结束或选择 0 时结束循环。
            break;  // 跳出菜单，转到最终输出和资源清理。
        }
        if (choice == 4) {  // 单独处理查看名单，因为这项不占入职、离职或查询的次数。
            printRoster(&list);  // 输出当前名单。
            continue;  // 直接开始下一轮菜单，跳过本轮的次数扣减。
        }

        if (choice == 1) {  // 根据选项 1 进入入职流程。
            ElemType employee = {"", "", ""};
            // 创建空的临时记录，用来接收这次的入职信息。
            if (!readEmployee(&employee)) {  // 依次读取新员工的姓名、职务和工号。
                break;  // 输入结束时停止菜单，本次未录完整的员工不会被加入。
            }
            int result = hire(&list, employee);
            // 把输入交给 hire 查重和插入，保存返回结果。
            if (result == 1) {  // 结果为 1 代表入职成功。
                puts("入职成功。");  // 向用户确认员工已经加入。
            } else if (result == 0) {  // 结果为 0 代表工号已存在。
                puts("入职失败，工号已存在。");  // 提示工号重复，本次入职未生效。
            } else {  // 剩下的结果表示插入出错。
                puts("入职失败，无法添加员工。");  // 向用户报告本次无法添加员工。
            }
        } else {  // 剩余菜单选项是离职和查询，两者都先读取工号。
            ElemType key = {"", "", ""};  // 创建空的查找条件，后面只填写工号字段。
            if (!readField("请输入工号：", key.id, EMPLOYEE_ID_SIZE)) {
            // 读取要查找的工号，并写入 key.id。
                break;  // 读不到工号就退出循环，本轮不执行离职或查询。
            }
            if (choice == 2) {  // 选项 2 按工号办理离职。
                if (dismiss(&list, &key)) {
                // 调用 dismiss 定位记录并删除，判断操作是否成功。
                    puts("离职成功。");  // 删除成功后提示已离职。
                } else {  // 删除操作未成功时给出工号未找到的提示。
                    puts("离职失败，未找到该工号。");  // 提示列表中没有对应工号。
                }
            } else {  // 余下的有效选项是查询工号。
                ElemType employee;  // 准备接收查询返回的完整员工记录。
                if (!query(&list, &key, &employee)) {
                // 调用 query，找到的记录会写入 employee。
                    puts("未找到该工号。");  // 查找失败时提示未找到该工号。
                } else {  // 查询成功时可以使用 employee 中的记录。
                    printf("查询结果（姓名 | 职务 | 工号）：");  // 先说明接下来三个字段的顺序。
                    PrintEmployee(&employee);  // 打印通过查询取得的员工记录。
                }
            }
        }

        if (remaining > 0) {  // 只在剩余次数为正时扣减，不限次数模式的负值保持不变。
            remaining--;  // 一次入职、离职或查询结束后将剩余次数减 1。
        }
    }

    if (ready) {  // 已完成初始录入和次数设置时才输出最终名单。
        puts("\n最终员工名单");  // 标明接下来打印的是结束时的员工名单。
        printRoster(&list);  // 输出最终表长及员工记录。
    }

    if (mode == USE_SEQUENTIAL) {  // 选择顺序表时销毁它使用的连续数组。
        DestroyList_Sq(&list.sq);  // 调用顺序表的销毁函数回收存储空间。
    } else {  // 选择单链表时改为回收链表结点。
        DestroyList_L(&list.link);  // 调用单链表的销毁函数逐个释放结点。
    }
    puts("\n程序结束。");  // 在清理完成后显示程序结束。
    return 0;  // 向操作系统返回 0，表示程序正常结束。
}
