add_library(road_lib 	
	MathUtils.h
	VectorMath.cpp

	RoadNetwork.h
	RoadDesign.cpp
	RoadNetwork.cpp
	RoadRendering.cpp
	JunctionDesign.cpp
)
target_include_directories(road_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(road_lib PUBLIC raylib)
