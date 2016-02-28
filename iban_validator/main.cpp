#include <iostream>
#include <memory>
#include "Specification.h"
#include "Validate.h"

int main(int argc, char *argv[])
{
	Validate val;
	std::string arg;
	if (argc == 2) {
		arg = argv[1];
	} else {
		arg = "_REPLACE_ME";
	}
	if (val.isValid(arg)) {
		std::cout << "Valid" << std::endl;
	} else {
		std::cout << "Not Valid" << std::endl;
	}
	return 0;
}