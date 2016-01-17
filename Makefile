all : test

test :
	g++ -std=c++11 Awele.cpp main.cpp -o awele
