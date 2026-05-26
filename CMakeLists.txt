cmake_minimum_required(VERSION 3.28.3)

project(lorem-ipsum)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
if(WIN32)
    set(GLFW_LIB "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw3-mingw/libglfw3.a")
    set(PLATFORM_LIBS OpenGL::GL Threads::Threads)
    add_subdirectory(external/glm)

    file(GLOB_RECURSE SOURCES "src/*.cpp" "external/glad/*.c")
    add_executable(lorem-ipsum ${SOURCES})

    target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE "include/" "src/" "external/")
    target_link_libraries("${CMAKE_PROJECT_NAME}" glm "${GLFW_LIB}" ${PLATFORM_LIBS})

elseif(UNIX)
    find_package(glfw3 REQUIRED)
    set(PLATFORM_LIBS OpenGL::GL Threads::Threads ${CMAKE_DL_LIBS})
    add_subdirectory(external/glm)

    file(GLOB_RECURSE SOURCES "src/*.cpp" "external/glad/*.c")
    add_executable(lorem-ipsum ${SOURCES})

    target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE "include/" "src/" "external/")
    target_link_libraries("${CMAKE_PROJECT_NAME}" glm glfw ${PLATFORM_LIBS})
endif()



#set(ASSETS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/assets")
#set(ASSETS_DST "${CMAKE_BINARY_DIR}/assets")
#add_custom_command(TARGET "${CMAKE_PROJECT_NAME}" POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_SRC}" "${ASSETS_DST}"
#    COMMENT "Copying assets to build directory"
#)