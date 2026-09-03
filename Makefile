default:
	g++ src/*.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Llib -Iinclude -lraylib -o main


main:
	g++ src/main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Llib -Iinclude -lraylib -o main

roads: 
	g++ src/RoadNetwork.h src/RoadNetwork.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Llib -Iinclude -lraylib -o main

