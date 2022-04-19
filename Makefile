.DEFAULT_GOAL := all

all:
	g++ -o encoder encoder.cpp -O3
	g++ -o decoder decoder.cpp -O3

encoder: encoder.cpp
	g++ -o encoder encoder.cpp -O3
decoder: decoder.cpp
	g++ -o decoder decoder.cpp -O3