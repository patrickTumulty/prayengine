
cmake_minimum_required(VERSION 3.10)

project(prayevents VERSION 0.1.0)

include_directories(include)

file(GLOB SOURCES src/prayevents.c)

add_library(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC common)

target_include_directories(${PROJECT_NAME} PUBLIC include)

# Find CUnit
find_package(PkgConfig REQUIRED)
pkg_check_modules(CUNIT REQUIRED cunit)

add_executable(${PROJECT_NAME}_test
    tests/main.c
    tests/prayevents_tests.c 
)

target_include_directories(${PROJECT_NAME}_test PUBLIC 
    ${CUNIT_INCLUDE_DIRS}
    .
)

target_link_libraries(${PROJECT_NAME}_test PUBLIC prayevents common ${CUNIT_LIBRARIES})
