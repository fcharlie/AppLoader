cmake_minimum_required(VERSION 3.4)
project(AppLoaderExe)

add_executable(AppLoader WIN32
    AppContainerExecute.cc
    Precompiled.cc
    BatchExecute.cc
    PowerShellExecute.cc
    Executable.cc
	Environment.cc
    Process.cc
    main.cc
    AppLoader.rc
    AppLoader.manifest
)

target_link_libraries(AppLoader
    libAppLoader
    kernel32
    advapi32
    shell32
    user32
    gdi32
    comctl32
    shlwapi
    Userenv
)
