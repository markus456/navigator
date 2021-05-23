add_executable(navigator main.cc objects.cc world.cc events.cc graphics.cc)
target_link_libraries(navigator ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})
install(TARGETS navigator DESTINATION ${CMAKE_BINARY_DIR})
target_link_options(navigator PRIVATE /SUBSYSTEM:windows /ENTRY:mainCRTStartup)

add_subdirectory(test)