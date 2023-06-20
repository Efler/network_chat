cmake_minimum_required(VERSION 3.25)
project(chat_client)

set(CMAKE_CXX_STANDARD 17)

add_executable(chat_client client.cpp client.hpp)
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc")

# --- SFML Independence from project dlls ---
set(SFML_STATIC_LIBRARIES TRUE)

# --- SFML ---

set(SFML_DIR "D:\\JetBrains\\CLion 2022.2.3\\CLionProjects\\libs\\SFML\\GCC 7.3.0 MinGW (SEH) - 64-bit\\SFML-2.5.1\\lib\\cmake\\SFML")

# find_package(SFML 2.5.1 COMPONENTS network audio graphics window system REQUIRED)
find_package(SFML 2.5.1 COMPONENTS network REQUIRED)

# target_link_libraries(${PROJECT_NAME} sfml-network sfml-audio sfml-graphics sfml-window sfml-system)
target_link_libraries(chat_client sfml-network)