cmake_minimum_required(VERSION 3.4)
project(AppLoaderExe)

add_executable(AppLoader
    AppContainerExecute.cc
    BatchExecute.cc
    PowerShellExecute.cc
    Executable.cc
    Process.cc
    AppLoader.rc
    AppLoader.manifest
    main.cc
)

target_link_libraries(AppLoader
    AppLoader
    Shlwapi
    Kernel32
    Gdi32
    User32
)
