set(revolution_SRCS
    Screen.cpp
    Shader.cpp
    World.cpp
    Main.cpp)

add_executable(main ${revolution_SRCS})
target_link_libraries(main ${SDL_LIBRARY})
