cmake_minimum_required(VERSION 3.15...4.0)
project(MyProject VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#g++,MSVC都设置为utf-8编码
# 在项目开始时设置全局编译选项
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8 -fexec-charset=UTF-8")
endif()


# --------------------------
# 控制输出文件目录
# --------------------------
# 二进制文件输出目录（.pyd, .dll, .so等）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)

# 库文件输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)

# 静态库和目标文件输出目录（.obj, .a等）
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)

# PDB文件（调试符号）输出目录
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pdb)
set(CMAKE_PDB_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/pdb/Debug)
set(CMAKE_PDB_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/pdb/Release)

# --------------------------
# Python 配置
# --------------------------
find_package(Python REQUIRED COMPONENTS Interpreter Development)
# --------------------------
# pybind11 配置
# --------------------------
if(WIN32)
  set(pybind11_DIR "D:\\Environment\\Miniconda3\\envs\\compile_env\\Lib\\site-packages\\pybind11\\share\\cmake\\pybind11\\")
else()
  set(pybind11_DIR "~/pybind11/build")
endif()
find_package(pybind11 CONFIG REQUIRED)

# --------------------------
# 模块配置（支持分离写法）
# --------------------------
# 获取src目录下的直接子目录（每个子目录为一个模块）
file(GLOB MOD_DIRS LIST_DIRECTORIES TRUE "${CMAKE_SOURCE_DIR}/src/*")

foreach(MOD_DIR IN LISTS MOD_DIRS)
    # 确保这是一个目录
    if(IS_DIRECTORY ${MOD_DIR})
        # 提取子目录名作为模块名
        get_filename_component(MOD_NAME ${MOD_DIR} NAME)
        
        # 查找该目录下的所有源文件（.cpp）和头文件（.h/.hpp）
        file(GLOB MOD_SOURCES "${MOD_DIR}/*.cpp")
        file(GLOB MOD_HEADERS "${MOD_DIR}/*.h" "${MOD_DIR}/*.hpp")  # 新增：查找头文件
        
        # 只有当存在源文件时才创建模块
        if(MOD_SOURCES)
            pybind11_add_module(${MOD_NAME} MODULE ${MOD_SOURCES} ${MOD_HEADERS})  # 新增：将头文件加入目标（方便IDE识别）
            
            # 设置模块的输出名称（去除Python版本和平台后缀）
            set_target_properties(${MOD_NAME} PROPERTIES
                OUTPUT_NAME ${MOD_NAME}
                PREFIX ""
                SUFFIX ".pyd"
            )
            
            # 设置模块的输出目录属性
            set_target_properties(${MOD_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
                RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
                LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}
                LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE}
                ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG}
                ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE}
                PDB_OUTPUT_DIRECTORY_DEBUG ${CMAKE_PDB_OUTPUT_DIRECTORY_DEBUG}
                PDB_OUTPUT_DIRECTORY_RELEASE ${CMAKE_PDB_OUTPUT_DIRECTORY_RELEASE}
            )
            
            # 核心：添加头文件目录（模块自身目录），确保.cpp能包含同目录的.h
            target_include_directories(${MOD_NAME} PRIVATE ${MOD_DIR})  # 新增：包含模块目录
            
            target_link_libraries(${MOD_NAME} PRIVATE Python::Python)
        endif()
    endif()
endforeach()