cmake_minimum_required(VERSION 3.15...4.0)
project(MyProject VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8 -fexec-charset=UTF-8")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)


set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pdb)
set(CMAKE_PDB_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/pdb/Debug)
set(CMAKE_PDB_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/pdb/Release)

find_package(Python REQUIRED COMPONENTS Interpreter Development)


if(WIN32)
  set(pybind11_DIR "D:\\Environment\\Miniconda3\\envs\\compile_env\\Lib\\site-packages\\pybind11\\share\\cmake\\pybind11\\")
else()
  set(pybind11_DIR "~/pybind11/build")
endif()
find_package(pybind11 CONFIG REQUIRED)


# set(MODULES_LIST lexical syntactic AST)
file(GLOB MODULES_LIST LIST_DIRECTORIES TRUE "${CMAKE_SOURCE_DIR}/src/*")

foreach(MOD_DIR IN LISTS MODULES_LIST)
    if(IS_DIRECTORY ${MOD_DIR})
        # 提取子目录名作为模块名
        get_filename_component(MOD_NAME ${MOD_DIR} NAME)
        file(GLOB MOD_SOURCES "${MOD_DIR}/*.cpp")
        file(GLOB MOD_HEADERS "${MOD_DIR}/*.h" "${MOD_DIR}/*.hpp")
        list(FILTER MOD_SOURCES EXCLUDE REGEX "bindings\\.cpp$")
        

        if(MOD_SOURCES)
            add_library(${MOD_NAME}_core STATIC ${MOD_SOURCES} ${MOD_HEADERS})
            set_target_properties(${MOD_NAME}_core PROPERTIES
                OUTPUT_NAME ${MOD_NAME}_core
            )

            target_link_libraries(${MOD_NAME}_core PUBLIC Python::Python)
        endif()
    endif()
endforeach()

target_link_libraries(syntactic_core PUBLIC lexical_core)
target_link_libraries(TAC_core PUBLIC lexical_core)
target_link_libraries(AST_core PUBLIC lexical_core)

# （如需AST依赖其他模块，取消下面注释并调整）
# target_include_directories(AST_core PUBLIC 
#     "${CMAKE_SOURCE_DIR}/src/lexical"
#     "${CMAKE_SOURCE_DIR}/src/syntactic"
# )
# target_link_libraries(AST_core PUBLIC lexical_core syntactic_core)


foreach(MOD_DIR IN LISTS MODULES_LIST)
    if(IS_DIRECTORY ${MOD_DIR})
        get_filename_component(MOD_NAME ${MOD_DIR} NAME)
        set(BINDINGS_FILE "${MOD_DIR}/bindings.cpp")
        if(EXISTS ${BINDINGS_FILE})   
            pybind11_add_module(${MOD_NAME} MODULE ${BINDINGS_FILE})
            set_target_properties(${MOD_NAME} PROPERTIES
                OUTPUT_NAME ${MOD_NAME}  
                PREFIX ""               
                SUFFIX ".pyd"           
            )
            target_link_libraries(${MOD_NAME} PRIVATE ${MOD_NAME}_core)
        endif()
    endif()
endforeach()