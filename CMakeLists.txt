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

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# Добавление исполняемого файла с именем main.out
add_executable(
    main.out
    ./project/main.cpp
    ./project/requestcurl.cpp
    ./project/soup.cpp
    ./project/parser.cpp
    ./project/page.cpp
    ./project/parserpool.cpp
    )

# Включение заголовочных файлов и библиотек
target_link_libraries(main.out CURL::libcurl)
target_link_libraries(main.out ${CURL_LIBRARIES})
target_include_directories(main.out PRIVATE ${CURL_INCLUDE_DIRS})
