set(CMAKE_CXX_STANDARD 17)

add_executable(test_query_time test_query_time.cpp)
target_link_libraries(test_query_time ${PROJECT_NAME})

add_executable(test_index_time test_index_time.cpp)
target_link_libraries(test_index_time ${PROJECT_NAME})

add_executable(test_query_quality test_query_quality.cpp)
target_link_libraries(test_query_quality ${PROJECT_NAME})

add_executable(test_split_graph test_split_graph.cpp)
target_link_libraries(test_split_graph ${PROJECT_NAME})

add_executable(test_index_without_join test_index_without_join.cpp)
target_link_libraries(test_index_without_join ${PROJECT_NAME})