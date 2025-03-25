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

void	functionTakingIntSharedPtr(SharedResource<int *> sharedIntPtr)
{
	(void)sharedIntPtr;
}

void	multipleInstances()
{

	SharedResource<int*>	wrappedTest(getIntWrappedPtr(25));
	SharedResource<int*>	wrappedTest2(wrappedTest);
	{
		SharedResource<int*>	wrappedTest3(wrappedTest2);
		functionTakingIntSharedPtr(wrappedTest2);
	}
	functionTakingIntSharedPtr(wrappedTest);
	functionTakingIntSharedPtr(wrappedTest2);
	std::cout << "multiple instances : " << *(wrappedTest2.value) << std::endl;
}

void	testDifferentResources()
{
	SharedResource<int*>	first(getIntWrappedPtr(30));
	std::cout << "test different resources : " << *(first.value);
	{
		SharedResource<int*>	second(getIntWrappedPtr(30));
		std::cout << " + " << *(second.value) << std::endl;
	}
}

void	testChangingResource()
{
	SharedResource<int*>	first(getIntWrappedPtr(30));
	SharedResource<int*>	second(getIntWrappedPtr(30));
	first = second;
	std::cout << "test changing resource : " << *(first.value) << std::endl;
}

int	main(void)
{
	basicPrtTest();
	returnedPtrTest();
	returnedAndCopyConstructor();
	returnedAndAssignmentOperator();
	multipleInstances();
	testDifferentResources();
	testChangingResource();
}
