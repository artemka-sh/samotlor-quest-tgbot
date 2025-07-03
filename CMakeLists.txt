cmake_minimum_required(VERSION 3.0)

project(samotlor-quest-tgbot VERSION 1.0)


set(TGBOT_CPP_PATH "${CMAKE_SOURCE_DIR}/deps/tgbot-cpp")

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(Boost_USE_MULTITHREADED ON)
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)


# Включаем автоматическую обработку Qt (moc, uic, rcc)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)



# Находим Qt6 Core
find_package(Qt6 COMPONENTS Core Sql REQUIRED)

add_subdirectory(${TGBOT_CPP_PATH} ${CMAKE_BINARY_DIR}/tgbot-cpp)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)
include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR} include)
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} TgBot Qt6::Core Qt6::Sql)

#file(COPY ${CMAKE_SOURCE_DIR}/res/ DESTINATION ${CMAKE_BINARY_DIR})

add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)
