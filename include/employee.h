#ifndef EMPLOYEE_H  // 第一次包含此头文件时才继续处理内部声明。
#define EMPLOYEE_H  // 记录当前头文件已经包含，避免重复定义。

#define EMPLOYEE_NAME_SIZE 64  // 姓名数组占 64 字节，其中一字节留给字符串结束符。
#define EMPLOYEE_POSITION_SIZE 128  // 职务数组占 128 字节，其中一字节留给字符串结束符。
#define EMPLOYEE_ID_SIZE 64  // 工号数组占 64 字节，按字符串存储可以保留前导零。

typedef int Status;  // 用整数类型表示各线性表操作的状态。
#define OK 1  // 约定 1 表示操作成功或比较匹配。
#define ERROR 0  // 约定 0 表示操作失败或比较不匹配。

typedef struct ElemType  // 定义带 ElemType 标签的员工结构体，存放一条完整记录。
{
    char name[EMPLOYEE_NAME_SIZE];  // 字符数组保存姓名。
    char position[EMPLOYEE_POSITION_SIZE];  // 字符数组保存职务。
    char id[EMPLOYEE_ID_SIZE];  // 字符数组保存工号。
};

Status CompareById(const ElemType *a, const ElemType *b);
// 声明按工号比较的函数，定位函数可将它作为比较条件。

Status PrintEmployee(const ElemType *e);  // 声明打印一条员工记录的函数，可作为遍历回调。

#endif  // 结束员工头文件的重复包含保护。
