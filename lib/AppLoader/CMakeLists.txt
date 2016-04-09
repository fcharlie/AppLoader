cmake_minimum_required(VERSION 3.2)
project(AppLoader)

add_library(libAppLoader STATIC
    AppLoaderFile.cc
)
