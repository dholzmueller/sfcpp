cmake_minimum_required(VERSION 2.8)
project(sfcpp CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include/eigen3 -Wall -Wextra -fmessage-length=0 -fopenmp -std=c++11 -D_GNU_SOURCE=1 -D_REENTRANT -Dlinux -D__linux__ -Dx86_64 -D__x86_64__")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2") #-mtune=native -march=native
set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -L/usr/local/lib")

link_directories(/usr/local/lib)

file(GLOB SRC_CPP "src/cpp/*.cpp")
file(GLOB SRC_DATA "src/data/*.cpp")
file(GLOB SRC_FILES "src/files/*.cpp")
file(GLOB SRC_GEO "src/geo/*.cpp")
file(GLOB SRC_LATEX "src/latex/*.cpp")
file(GLOB SRC_LATEX_TIKZ "src/latex/tikz/*.cpp")
file(GLOB SRC_MATH "src/math/*.cpp")
file(GLOB SRC_STRINGS "src/strings/*.cpp")
file(GLOB SRC_TIME "src/time/*.cpp")

file(GLOB INC_CPP "src/cpp/*.hpp")
file(GLOB INC_DATA "src/data/*.hpp")
file(GLOB INC_FILES "src/files/*.hpp")
file(GLOB INC_GEO "src/geo/*.hpp")
file(GLOB INC_IMG "src/img/*.hpp")
file(GLOB INC_LATEX "src/latex/*.hpp")
file(GLOB INC_LATEX_TIKZ "src/latex/tikz/*.hpp")
file(GLOB INC_MATH "src/math/*.hpp")
file(GLOB INC_SFC "src/sfc/*.hpp")
file(GLOB INC_STRINGS "src/strings/*.hpp")
file(GLOB INC_TEST "test/*.hpp")


## Build application
add_library(sfcpp-core ${SRC_CPP} ${SRC_DATA} ${SRC_FILES} ${SRC_GEO} ${SRC_LATEX} ${SRC_LATEX_TIKZ} ${SRC_MATH} ${SRC_STRINGS} ${SRC_TIME})
include_directories("${PROJECT_SOURCE_DIR}/src")

option(BUILD_IMG ON)
option(BUILD_SFC ON)
option(BUILD_TEST ON)

if(BUILD_IMG)
    if(BUILD_IMG STREQUAL "ON")
        file(GLOB SRC_IMG "src/img/*.cpp")
        add_library(sfcpp-img ${SRC_IMG})
        install(TARGETS sfcpp-img DESTINATION lib)
    endif()
endif()

if(BUILD_SFC)
    if(BUILD_SFC STREQUAL "ON")
        file(GLOB SRC_SFC "src/sfc/*.cpp")
        add_library(sfcpp-sfc ${SRC_SFC})
        install(TARGETS sfcpp-sfc DESTINATION lib)
    endif()
endif()

if(BUILD_TEST)
    if(BUILD_TEST STREQUAL "ON")
        file(GLOB SRC_TEST "test/*.cpp")
        add_executable(sfcpp-test ${SRC_TEST})
        include_directories("${PROJECT_SOURCE_DIR}/src")
        target_link_libraries(sfcpp-test sfcpp-sfc sfcpp-img sfcpp-core opencv_imgproc opencv_core boost_system boost_filesystem boost_timer)
    endif()
endif()



install(TARGETS sfcpp-core DESTINATION lib)
install(FILES ${INC_CPP} DESTINATION include/sfcpp/cpp)
install(FILES ${INC_DATA} DESTINATION include/sfcpp/data)
install(FILES ${INC_FILES} DESTINATION include/sfcpp/files)
install(FILES ${INC_GEO} DESTINATION include/sfcpp/geo)
install(FILES ${INC_IMG} DESTINATION include/sfcpp/img)
install(FILES ${INC_LATEX} DESTINATION include/sfcpp/latex)
install(FILES ${INC_LATEX_TIKZ} DESTINATION include/sfcpp/latex/tikz)
install(FILES ${INC_MATH} DESTINATION include/sfcpp/math)
install(FILES ${INC_SFC} DESTINATION include/sfcpp/sfc)
install(FILES ${INC_STRINGS} DESTINATION include/sfcpp/strings)
install(FILES ${INC_TIME} DESTINATION include/sfcpp/time)





