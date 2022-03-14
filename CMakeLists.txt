cmake_minimum_required(VERSION 3.14)

project(FlightMap LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)

# Main group
file (GLOB MAIN_SRC "src/main/*.cpp" "src/main/*.h")
source_group("main" FILES ${MAIN_SRC})
set (ALL_SRC ${ALL_SRC} ${MAIN_SRC})

# Common group
file (GLOB COMMON_SRC "src/common/*.cpp" "src/common/*.h")
source_group("common" FILES ${COMMON_SRC})
set (ALL_SRC ${ALL_SRC} ${COMMON_SRC})

# Devices group
file (GLOB DEVICES_SRC "src/devices/*.cpp" "src/devices/*.h")
source_group("devices" FILES ${DEVICES_SRC})
set (ALL_SRC ${ALL_SRC} ${DEVICES_SRC})

# Messages group
file (GLOB MESSAGES_SRC "src/messages/*.cpp" "src/messages/*.h")
source_group("messages" FILES ${MESSAGES_SRC})
set (ALL_SRC ${ALL_SRC} ${MESSAGES_SRC})

# Data group
file (GLOB DATA_SRC "src/data/*.cpp" "src/data/*.h")
source_group("data" FILES ${DATA_SRC})
set (ALL_SRC ${ALL_SRC} ${DATA_SRC})

# Comms group
file (GLOB COMMS_SRC "src/comms/*.cpp" "src/comms/*.h")
source_group("comms" FILES ${COMMS_SRC})
set (ALL_SRC ${ALL_SRC} ${COMMS_SRC})

# Sources
include_directories("src")
file(GLOB SOURCES "src/*.h" "src/*.hpp" "src/*.cpp")
add_executable(${PROJECT_NAME} ${ALL_SRC} "res/qml.qrc")

# Json
if (WIN32)
	include_directories("libs/jsoncpp/include")
	target_link_directories(${PROJECT_NAME} PUBLIC "libs/jsoncpp/lib/windows")
	target_link_libraries(${PROJECT_NAME} -ljsoncpp)
else()
	include_directories("libs/jsoncpp/include")
	target_link_directories(${PROJECT_NAME} PUBLIC "libs/jsoncpp/lib/linux")
	target_link_libraries(${PROJECT_NAME} -l:libjsoncpp.a)
endif()

# Zip
add_subdirectory("libs/zlib/zip")
if (WIN32)
	target_link_directories(${PROJECT_NAME} PUBLIC "libs/zlib/lib/windows")
	target_link_libraries(${PROJECT_NAME} zip -l:libzlib.a)
else()
	target_link_directories(${PROJECT_NAME} PUBLIC "libs/zlib/lib/linux")
	target_link_libraries(${PROJECT_NAME} zip -l:libz.a)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)

if (WIN32)
	add_link_options(LINKER:-std=c++17 -lstdc++fs)
	target_link_libraries(${PROJECT_NAME} pthread ws2_32)
else()
	target_link_libraries(${PROJECT_NAME} pthread)
endif()
