add_library(sim_lib
	Simulation.h
	Simulation.cpp
)

target_include_directories(sim_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sim_lib PUBLIC road_lib raylib)
