test: main.cpp
	g++ -o gcm_test -lgsl -lgslcblas -lm main.cpp

production: main.cpp
	g++ -O3 -o gcm_prod_test -lgsl -lgslcblas -lm main.cpp

clean:
	rm -f ./gcm_test ./gcm_prod_test
