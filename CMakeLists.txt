cmake_minimum_required(VERSION 3.15...4.0)
project(MyProject VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 统一设置编码（g++/MSVC均支持UTF-8）
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8 -fexec-charset=UTF-8")
endif()

# --------------------------
# 控制输出文件目录（全局统一设置，自动应用到所有目标）
# --------------------------
# 二进制文件（.pyd, .dll, .so等）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)

# 库文件
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)

# 静态库和目标文件（.obj, .a等）
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)

# 调试符号文件（PDB）
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pdb)
set(CMAKE_PDB_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/pdb/Debug)
set(CMAKE_PDB_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/pdb/Release)

# --------------------------
# 依赖配置
# --------------------------
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# pybind11路径配置
if(WIN32)
  set(pybind11_DIR "D:\\Environment\\Miniconda3\\envs\\compile_env\\Lib\\site-packages\\pybind11\\share\\cmake\\pybind11\\")
else()
  set(pybind11_DIR "~/pybind11/build")
endif()
find_package(pybind11 CONFIG REQUIRED)

# --------------------------
# 模块配置
# --------------------------
set(MODULES_LIST lexical syntactic AST)

# 1. 创建核心静态库（模块逻辑实现）
foreach(MOD_NAME IN LISTS MODULES_LIST)
    # 收集源文件（排除bindings.cpp，它是pybind11入口）
    file(GLOB MOD_SOURCES "${CMAKE_SOURCE_DIR}/src/${MOD_NAME}/*.cpp")
    file(GLOB MOD_HEADERS "${CMAKE_SOURCE_DIR}/src/${MOD_NAME}/*.h" "${CMAKE_SOURCE_DIR}/src/${MOD_NAME}/*.hpp")
    list(FILTER MOD_SOURCES EXCLUDE REGEX "bindings\\.cpp$")
    
    # 创建静态库
    add_library(${MOD_NAME}_core STATIC ${MOD_SOURCES} ${MOD_HEADERS})
    set_target_properties(${MOD_NAME}_core PROPERTIES
        OUTPUT_NAME ${MOD_NAME}_core
    )
    
    # 公开头文件目录（依赖该库的目标会自动继承）
    target_include_directories(${MOD_NAME}_core PUBLIC 
        "${CMAKE_SOURCE_DIR}/src/${MOD_NAME}"
    )
    
    # 公开链接Python（依赖该库的目标会自动继承）
    target_link_libraries(${MOD_NAME}_core PUBLIC Python::Python)
endforeach()

# 2. 配置模块间依赖（通过core库传递依赖）
# syntactic依赖lexical
target_include_directories(syntactic_core PUBLIC 
    "${CMAKE_SOURCE_DIR}/src/lexical"  # 允许syntactic_core访问lexical头文件
)
target_link_libraries(syntactic_core PUBLIC lexical_core)  # 公开依赖，下游自动继承

# （如需AST依赖其他模块，取消下面注释并调整）
# target_include_directories(AST_core PUBLIC 
#     "${CMAKE_SOURCE_DIR}/src/lexical"
#     "${CMAKE_SOURCE_DIR}/src/syntactic"
# )
# target_link_libraries(AST_core PUBLIC lexical_core syntactic_core)

# 3. 创建pybind11模块（Python接口）
foreach(MOD_NAME IN LISTS MODULES_LIST)
    set(BINDINGS_FILE "${CMAKE_SOURCE_DIR}/src/${MOD_NAME}/bindings.cpp")
    if(EXISTS ${BINDINGS_FILE})
        # 创建Python模块
        pybind11_add_module(${MOD_NAME} MODULE ${BINDINGS_FILE})
        
        # 设置模块输出属性（仅保留必要的特殊设置）
        set_target_properties(${MOD_NAME} PROPERTIES
            OUTPUT_NAME ${MOD_NAME}  # 模块名称
            PREFIX ""               # 移除默认前缀（如lib）
            SUFFIX ".pyd"           # Windows下强制.pyd后缀（其他平台pybind11会自动处理）
        )
        
        # 链接核心库（自动继承core库的头文件和Python依赖）
        target_link_libraries(${MOD_NAME} PRIVATE ${MOD_NAME}_core)
    endif()
endforeach()