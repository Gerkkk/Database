cmake_minimum_required(VERSION 3.27)
project(untitled)

set(CMAKE_CXX_STANDARD 20)

#add_library(MEMDB/Token STATIC MEMDB/Token/Token.cpp)
#add_library(MEMDB/Preprocessor STATIC MEMDB/Preprocessor/Preprocessor.cpp)
#add_library(MEMDB/Result STATIC MEMDB/Result/Result.cpp)
#add_library(MathExpression STATIC MathExpression/MathExpression.cpp)

add_executable(main main.cpp
        MEMDB/Token/Token.cpp
        MEMDB/Token/Token.h
        MEMDB/Preprocessor/Preprocessor.cpp
        MEMDB/Preprocessor/Preprocessor.h
        MEMDB/Result/Result.cpp
        MEMDB/Result/Result.h
        MEMDB/MathExpression/MathExpression.h
        MEMDB/MathExpression/MathExpression.cpp
        MEMDB/Table/Table.h
        MEMDB/Table/Table.cpp
        MEMDB/Database/Database.h
        MEMDB/Database/Database.cpp)
