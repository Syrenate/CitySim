all:
	g++ src/roads/*.cpp -lGL -lm -lpthread -ldl -lrt -lX11 -Llib -Iinclude -lraylib -o main


roads: 
	g++ src/roads/*.cpp -lGL -lm -lpthread -ldl -lrt -lX11 -Llib -Iinclude -lraylib -o roads

sim: 
	g++ src/sim/*.cpp -lGL -lm -lpthread -ldl -lrt -lX11 -Llib -Iinclude -lraylib -o sim
# roads:
# 	g++ src/roads/*.cpp -lGL -lm -lpthread -ldl -lrt -lX11 -Llib -Iinclude -lraylib
#
#
# main:
# 	g++ src/main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Llib -Iinclude -lraylib -o main
#
# roads: 
# 	g++ src/RoadNetwork.h src/RoadNetwork.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Llib -Iinclude -lraylib -o main

