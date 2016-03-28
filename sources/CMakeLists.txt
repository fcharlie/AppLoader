cmake_minimum_required(VERSION 3.4)
project(AppLoaderExe)

add_executable(AppLoader WIN32
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
    kernel32
    advapi32
    shell32
    user32
    gdi32
    comctl32
    shlwapi
)
