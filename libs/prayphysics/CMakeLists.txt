
cmake_minimum_required(VERSION 3.10)

project(prayphysics VERSION 0.1.0)

FetchContent_Declare(
	box2d
	GIT_REPOSITORY https://github.com/erincatto/box2d.git
	GIT_TAG main
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(box2d)

file(GLOB SOURCES src/main.c)

add_library(${PROJECT_NAME} ${SOURCES})

