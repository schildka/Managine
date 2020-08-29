cmake_minimum_required(VERSION 3.12)

project(GLAD C)

set(GLAD_HEADER
    include/glad/glad.h
    include/KHR/khrplatform.h)

set(GLAD_SRC
    src/glad.c)

add_library(glad STATIC
            ${GLAD_HEADER}
            ${GLAD_SRC}
            )

target_compile_features(glad PRIVATE cxx_std_17)
target_include_directories(glad PUBLIC include/)

