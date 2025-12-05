cmake_minimum_required(VERSION 4.0)
project(the_grand_prix)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(the_grand_prix
        src/CarList/Car/Car.cpp
        src/CarList/CarList.cpp
        src/Game/Game.cpp
        src/Leaderboards/Leaderboards.cpp
        src/Menu/Menu.cpp
        src/Physics/Physics.cpp
        src/Physics/Engine/Engine.cpp
        src/Session/Session.cpp
        src/Setup/Setup.cpp
        src/TrackList/TrackList.cpp
        src/TrackList/Track/Track.cpp
        src/utils/functions.cpp
        src/main.cpp)

target_compile_features(the_grand_prix PRIVATE cxx_std_17)
target_link_libraries(the_grand_prix PRIVATE sfml-graphics)

add_custom_command(TARGET the_grand_prix POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:the_grand_prix> $<TARGET_FILE_DIR:the_grand_prix>
        COMMAND_EXPAND_LISTS
)