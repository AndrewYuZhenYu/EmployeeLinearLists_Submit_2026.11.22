cmake_minimum_required(VERSION 3.16)  # 指定配置此项目所需的最低 CMake 版本。
project(EmployeeLinearLists LANGUAGES CXX)
# 声明项目名称，并为 .cpp 源文件启用 C++ 编译器。

set(CMAKE_CXX_STANDARD 17)  # 选择 C++17 作为编译标准。
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 要求编译器支持指定标准。
set(CMAKE_CXX_EXTENSIONS OFF)  # 关闭编译器的语言扩展，尽量按标准模式构建。

add_executable(employee_manager  # 定义多文件版可执行目标并列出参与编译的源文件。
    main.cpp  # 编译多文件版的入口及交互菜单。
    src/employee.cpp  # 编译员工工号比较和记录输出的实现。
    src/sequential_list.cpp  # 编译顺序表的基本操作。
    src/linked_list.cpp  # 编译单链表的基本操作。
)
target_include_directories(employee_manager PRIVATE include)
# 为多文件目标添加 include 头文件搜索目录。

add_executable(employee_sequential "顺序表.cpp")  # 第一题。
add_executable(employee_linked "单链表.cpp")  # 第二题。

option(BUILD_EMPLOYEE_MANAGER_SINGLE "Build the single-file employee manager" OFF)
# 默认关闭单文件版，需要时可在 CMake 中启用。
if(BUILD_EMPLOYEE_MANAGER_SINGLE)
    add_executable(employee_manager_single test.cpp)
endif()
