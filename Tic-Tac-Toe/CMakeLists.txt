add_executable(Game
        main.cpp
        Tic-Tac-Toe.cpp
)

find_package(Threads REQUIRED)
target_link_libraries(Game PRIVATE Threads::Threads)
target_include_directories(Game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})