run: all
	./checkElasticMatrix3D
	./checkCoordDerivativesCalc
	./checkFindOwnerTetrAdvancedImpl

all: checkElasticMatrix3D checkCoordDerivativesCalc checkFindOwnerTetrAdvancedImpl

checkElasticMatrix3D:
	g++ -o checkElasticMatrix3D checkElasticMatrix3D.cpp

checkCoordDerivativesCalc:
	g++ -o checkCoordDerivativesCalc checkCoordDerivativesCalc.cpp

checkFindOwnerTetrAdvancedImpl:
	g++ -o checkFindOwnerTetrAdvancedImpl -lgsl -lgslcblas checkFindOwnerTetrAdvancedImpl.cpp

clean:
	rm -f ./checkElasticMatrix3D ./checkCoordDerivativesCalc ./checkFindOwnerTetrAdvancedImpl
