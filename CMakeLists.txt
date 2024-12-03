cmake_minimum_required(VERSION 3.22)
project(untitled)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_library(MEMDB/Token STATIC MEMDB/Token/Token.cpp)
#add_library(MEMDB/Preprocessor STATIC MEMDB/Preprocessor/Preprocessor.cpp)
#add_library(MEMDB/Result STATIC MEMDB/Result/Result.cpp)
#add_library(MathExpression STATIC MathExpression/MathExpression.cpp)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

#For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(Test Tests/FuncTests.cpp
                MEMDB/Token/Token.cpp
                MEMDB/Token/Token.h
                MEMDB/Preprocessor/Preprocessor.cpp
                MEMDB/Preprocessor/Preprocessor.h
                MEMDB/Result/Result.cpp
                MEMDB/Result/Result.h
                MEMDB/MathExpression/MathExpression.h
                MEMDB/MathExpression/SyntaxTree.cpp
                MEMDB/MathExpression/SyntaxTreeNode.cpp
                MEMDB/Table/Table.h
                MEMDB/Table/Table.cpp
                MEMDB/Database/Database.h
                MEMDB/Database/Database.cpp
                MEMDB/Database/Create.cpp
                MEMDB/Database/Insert.cpp
                MEMDB/Database/Select.cpp
                MEMDB/Database/Update.cpp
                MEMDB/Database/Delete.cpp
                MEMDB/Database/Join.cpp)

target_link_libraries(Test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(Test)


add_executable(main main.cpp
        MEMDB/Token/Token.cpp
        MEMDB/Token/Token.h
        MEMDB/Preprocessor/Preprocessor.cpp
        MEMDB/Preprocessor/Preprocessor.h
        MEMDB/Result/Result.cpp
        MEMDB/Result/Result.h
        MEMDB/MathExpression/MathExpression.h
        MEMDB/MathExpression/SyntaxTree.cpp
        MEMDB/MathExpression/SyntaxTreeNode.cpp
        MEMDB/Table/Table.h
        MEMDB/Table/Table.cpp
        MEMDB/Database/Database.h
        MEMDB/Database/Database.cpp
        MEMDB/Database/Create.cpp
        MEMDB/Database/Insert.cpp
        MEMDB/Database/Select.cpp
        MEMDB/Database/Update.cpp
        MEMDB/Database/Delete.cpp
        MEMDB/Database/Join.cpp)