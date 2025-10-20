cmake_minimum_required(VERSION 3.5)

set(CMAKE_BUILD_TYPE Debug)

project(server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)	
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(FileManager STATIC IMPORTED) 
set_target_properties(FileManager PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/libFileManager.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/"
)

project(server LANGUAGES CXX)
add_executable(server filemanager.h clientManager.h clientManager.cpp utils.h utils.cpp server.cpp)
target_link_libraries(server pthread  FileManager stdc++fs)


#Replace "client.cpp" with "main_fm.cpp"
project(client LANGUAGES CXX)
add_executable(client filemanager.h distributedFileManager.cpp clientManager.h
  clientManager.cpp utils.h utils.cpp main_fm.cpp)
target_link_libraries(client pthread stdc++fs)


project (fileManager)
add_executable (fileManager main_fm.cpp filemanager.h)
target_link_libraries(fileManager pthread FileManager stdc++fs)
