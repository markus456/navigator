add_executable(navigator main.cc objects.cc world.cc)
target_link_libraries(navigator ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})
install(TARGETS navigator DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(test)