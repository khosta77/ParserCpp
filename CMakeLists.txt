cmake_minimum_required(VERSION 3.0)
project(MyProject)

if(APPLE)  # Код для MacOS
    execute_process(
        COMMAND sw_vers -productVersion
        OUTPUT_VARIABLE OSX_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message("-- build for MacOS ${OSX_VERSION}")
endif()

find_package(CURL REQUIRED)

find_package(Boost 1.85.0 COMPONENTS system filesystem REQUIRED)
if(Boost_FOUND)
    #message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    #message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}") 
    include_directories(${Boost_INCLUDE_DIRS})
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# Добавление исполняемого файла с именем main.out
add_executable(
    main.out
    ./project/main.cpp
    )

# Включение заголовочных файлов и библиотек
target_link_libraries(main.out CURL::libcurl)
if(Boost_FOUND)
    target_link_libraries(main.out ${Boost_LIBRARIES})
endif()
