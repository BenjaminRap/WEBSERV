#include <iostream>

#include "SharedResource.hpp"

SharedResource<int*>	getIntWrappedPtr(int value)
{

	int*	test = new int;
	SharedResource<int*>	wrappedTest(test, freePointer);
	*(wrappedTest.value) = value;
	return (wrappedTest);
}

void	basicPrtTest()
{
	int*	test = new int;
	SharedResource<int*>	wrappedTest(test, freePointer);
	*(wrappedTest.value) = 5;
	std::cout << "basic ptr test : " << *(wrappedTest.value) << '\n';
}

void	returnedPtrTest()
{
	std::cout << "returned ptr test : " << *(getIntWrappedPtr(10).value) << '\n';
}

void	returnedAndCopyConstructor()
{
	SharedResource<int*>	wrappedTest(getIntWrappedPtr(15));

	std::cout << "returned and copy constructor : " << *(wrappedTest.value) << '\n';
}

void	returnedAndAssignmentOperator()
{
	SharedResource<int*>	wrappedTest = getIntWrappedPtr(20);

	std::cout << "returned and assignment operator : " << *(wrappedTest.value) << '\n';
}

void	multipleInstances()
{

	SharedResource<int*>	wrappedTest(getIntWrappedPtr(15));
	SharedResource<int*>	wrappedTest2(wrappedTest);
	{
		SharedResource<int*>	wrappedTest3(wrappedTest2);
	}
}

int	main(void)
{
	basicPrtTest();
	returnedPtrTest();
	returnedAndCopyConstructor();
	returnedAndAssignmentOperator();
	multipleInstances();
}
