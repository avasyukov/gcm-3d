test: main.cpp
	g++ -o test_build -lm main.cpp

clean:
	rm -f ./test_build
