set(CMAKE_CXX_STANDARD 17)

add_executable(main main.cpp)
target_link_libraries(main ${PROJECT_NAME})


add_executable(test_small_graph test_small_graph.cpp)
target_link_libraries(test_small_graph ${PROJECT_NAME})

add_executable(test_dynamic_graph test_dynamic_graph.cpp)
target_link_libraries(test_dynamic_graph ${PROJECT_NAME})