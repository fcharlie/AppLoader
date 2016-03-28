cmake_minimum_required(VERSION 3.2)
project(AppLoaderHandle)

add_library(AppLoaderHandle SHARED
    AppLoaderHandle.cc
    AppLoaderHandle.rc
)

target_link_libraries(AppLoaderHandle
    libAppLoader
    kernel32
    user32
)
