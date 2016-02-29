#include <iostream>
#include <memory>
#include "Specification.h"
#include "Validate.h"

void usage(char *progname) {
	printf("%s [IBAN]\n\n"
		" IBAN\tValidate IBAN account\n"
		, progname);
}

int main(int argc, char *argv[]) {
	Validate val;
	if (argc < 2) {
		usage(argv[0]);
		return 1;
	}

	if (val.isValid(argv[1])) {
		std::cout << "IBAN is valid" << std::endl;
	} else {
		std::cout << "IBAN is invalid" << std::endl;
	}

	return 0;
}
