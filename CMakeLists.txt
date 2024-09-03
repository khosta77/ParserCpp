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
#find_package(gsoap REQUIRED)
#include_directories(/opt/homebrew/Cellar/gsoap/2.8.135/include)
#link_directories(/opt/homebrew/Cellar/gsoap/2.8.135/lib)

# Установите путь к gSOAP
#set(GSOAP_DIR "/opt/homebrew/Cellar/gsoap/2.8.135/share/gsoap")
set(GSOAP_DIR "/opt/homebrew/Cellar/gsoap/2.8.135")
# Добавьте директории включаемых файлов
#include_directories("/opt/homebrew/Cellar/gsoap/2.8.135/share/gsoap/import")
#include_directories(${GSOAP_DIR}/soap)
#include_directories(${GSOAP_DIR}/schemas)
include_directories(${GSOAP_DIR}/include)
link_directories(${GSOAP_DIR}/lib)

#add_library(gsoap STATIC IMPORTED)
#set_target_properties(gsoap PROPERTIES IMPORTED_LOCATION "${GSOAP_DIR}/libgsoap.a")



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
target_link_libraries(main.out PRIVATE CURL::libcurl)
#target_include_directories(main.out PRIVATE ${gsoap_INCLUDE_DIRS})
#target_link_libraries(main.out PRIVATE ${gsoap_LIBRARIES})
target_link_libraries(main.out PRIVATE gsoap)
