cmake_minimum_required(VERSION 3.8)
project(program)

# based largely on: 
# https://github.com/Polytonic/Glitter/blob/master/CMakeLists.txt

# turn off compiler warnings
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++17")
endif()

# setup GLFW CMake project
add_subdirectory("${PROJECT_SOURCE_DIR}/external/glfw")

# include headers
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/external/glfw/include/GLFW")
include_directories("${PROJECT_SOURCE_DIR}/external/imgui/include")
include_directories("${PROJECT_SOURCE_DIR}/external/glad/include")
include_directories("${PROJECT_SOURCE_DIR}/external/glm/glm")
file(GLOB PROJECT_HEADERS "include/*.h*")

# include source files
file(GLOB PROJECT_SOURCES "src/*.cpp")
file(GLOB IMGUI_HEADERS "external/imgui/include/*.h")
file(GLOB IMGUI_SOURCES "external/imgui/src/*.cpp")
file(GLOB GLAD_SOURCES "external/glad/src/*.c")

# group files in IDE
source_group("include" FILES ${PROJECT_HEADERS} ${IMGUI_HEADERS})
source_group("src" FILES ${PROJECT_SOURCES})
source_group("external" FILES ${IMGUI_SOURCES} ${GLAD_SOURCES})

# create the executable
add_executable(program ${PROJECT_SOURCES}
					${PROJECT_HEADERS} # if not included here, it won't show up in the IDE?
					${IMGUI_SOURCES}
					${IMGUI_HEADERS}
					${GLAD_SOURCES}
		)
					
set_target_properties(program PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)
					
# add libraries
target_link_libraries(program glfw ${GLFW_LIBRARIES})
