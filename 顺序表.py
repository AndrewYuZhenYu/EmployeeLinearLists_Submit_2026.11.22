class SqList:
    def __init__(self):
        self.elem = [None] * 80
        self.length = 0

    def insert(self, i, employee):
        if not 1 <= i <= self.length + 1:
            return False
        if self.length == len(self.elem):
            self.elem += [None] * 10
        for j in range(self.length, i - 1, -1):  # 从后向前搬。
            self.elem[j] = self.elem[j - 1]
        self.elem[i - 1] = employee
        self.length += 1
        return True

    def delete(self, i):
        if not 1 <= i <= self.length:
            return None
        employee = self.elem[i - 1]
        for j in range(i, self.length):
            self.elem[j - 1] = self.elem[j]
        self.length -= 1
        self.elem[self.length] = None
        return employee

    def locate(self, employee_id):
        for j in range(self.length):
            if self.elem[j][2] == employee_id:
                return j + 1
        return 0

    def get(self, i):
        if 1 <= i <= self.length:
            return self.elem[i - 1]
        return None

    def traverse(self):
        for j in range(self.length):
            print_employee(self.elem[j])


def print_employee(employee):
    print(*employee, sep=" | ")


def print_roster(employees):
    print(f"在职员工人数：{employees.length}")
    if employees.length == 0:
        print("暂无员工。")
    else:
        print("姓名 | 职务 | 工号")
        employees.traverse()


def read_number(prompt, maximum=2147483647, allow_blank=False):
    while True:
        text = input(prompt)
        if allow_blank and text == "":
            return -1
        try:
            number = int(text)
        except ValueError:
            pass
        else:
            if 0 <= number <= maximum:
                return number
        print(f"输入无效，请输入 0 到 {maximum} 之间的整数。")


def read_field(prompt, maximum):
    while True:
        text = input(prompt)
        if not text:
            print("输入不能为空，请重试。")
        elif len(text.encode("utf-8")) > maximum:
            print(f"输入过长，最多 {maximum} 字节，请重试。")
        else:
            return text


def read_employee():
    name = read_field("姓名：", 63)
    position = read_field("职务：", 127)
    employee_id = read_field("工号：", 63)
    return name, position, employee_id


def hire(employees, employee):
    if employees.locate(employee[2]):
        return False
    return employees.insert(employees.length + 1, employee)


def dismiss(employees, employee_id):
    return employees.delete(employees.locate(employee_id)) is not None


def query(employees, employee_id):
    return employees.get(employees.locate(employee_id))


def main():
    print("线性表上机 · 员工管理")
    print("\n当前存储方式：顺序表")
    employees = SqList()
    try:
        count = read_number("请输入初始员工人数：")
        while employees.length < count:
            print(f"\n录入第 {employees.length + 1} 名初始员工")
            if not hire(employees, read_employee()):
                print("工号已存在，请重新录入该员工。")
        remaining = read_number("请输入操作次数（直接回车可连续操作）：",
                                allow_blank=True)
    except EOFError:
        return

    try:
        while remaining != 0:
            print("\n员工管理菜单\n1. 员工入职\n2. 员工离职\n"
                  "3. 查询员工\n4. 查看员工名单\n0. 退出")
            choice = read_number("请选择操作：", 4)
            if choice == 0:
                break
            if choice == 4:
                print_roster(employees)
                continue

            if choice == 1:
                if hire(employees, read_employee()):
                    print("入职成功。")
                else:
                    print("入职失败，工号已存在。")
            else:
                employee_id = read_field("请输入工号：", 63)
                if choice == 2:
                    if dismiss(employees, employee_id):
                        print("离职成功。")
                    else:
                        print("离职失败，未找到该工号。")
                else:
                    employee = query(employees, employee_id)
                    if employee is None:
                        print("未找到该工号。")
                    else:
                        print("查询结果（姓名 | 职务 | 工号）：", end="")
                        print_employee(employee)

            if remaining > 0:
                remaining -= 1
    except EOFError:
        pass

    print("\n最终员工名单")
    print_roster(employees)


if __name__ == "__main__":
    main()
    print("\n程序结束。")
