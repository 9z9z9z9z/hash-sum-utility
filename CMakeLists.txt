cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


project(hash-sum-utility)

add_executable(hash-sum-utility 
                    main.cpp
                    test.cpp
                )
