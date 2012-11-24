set(revolution_SRCS
    Camera.cpp
    Drawable.cpp
    Screen.cpp
    Shader.cpp
    World.cpp
    Main.cpp)

add_executable(main ${revolution_SRCS})
target_link_libraries(main ${SDL_LIBRARY})
