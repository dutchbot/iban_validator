#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <regex>
#include "Validate.h"

void Validate::addSpecification(std::unique_ptr<Specification> specPtr)
{
	this->specifications[specPtr->countryCode] = std::move(specPtr);
}

/**
* Calculates the MOD 97 10 of the passed IBAN as specified in ISO7064.
*
* @param string iban
* @returns {bool}
*/
bool iso7064Mod97_10(std::string iBan) {
	std::rotate(iBan.begin(), iBan.begin() + 4, iBan.end());
	std::string numberstring;//will contain the letter substitutions
	for (const auto& c : iBan)
	{
		if (std::isdigit(c))
			numberstring += c;
		if (std::isupper(c))
			numberstring += std::to_string(static_cast<int>(c) - 55);
	}
	//implements a stepwise check for mod 97 in chunks of 9 at the first time
	// , then in chunks of seven prepended by the last mod 97 operation converted
	//to a string
	int segstart = 0;
	int step = 9;
	std::string prepended;
	long number = 0;
	while (segstart  < numberstring.length() - step) {
		number = std::stol(prepended + numberstring.substr(segstart, step));
		int remainder = number % 97;
		prepended = std::to_string(remainder);
		if (remainder < 10)
			prepended = "0" + prepended;
		segstart = segstart + step;
		step = 7;
	}
	number = std::stol(prepended + numberstring.substr(segstart));
	return (number % 97 == 1);
}

/**
* parse the bban structure used to configure each iban specification and returns a matching regular expression.
* a structure is composed of blocks of 3 characters (one letter and 2 digits). each block represents
* a logical group in the typical representation of the bban. for each group, the letter indicates which characters
* are allowed in this group and the following 2-digits number tells the length of the group.
*
* @param {string} structure the structure to parse
* @returns {regexp}
*/
std::regex parseStructure(std::string structure) {
	std::smatch match;

	std::string::const_iterator text_iter = structure.cbegin();
	std::ostringstream result;

	while (std::regex_search(text_iter, structure.cend(), match, std::regex("(.{3})")))
	{
		std::string format;
		char pattern = match[0].str()[0];
		int repeats = std::stoi((match[0].str().substr(1)));

		// parse each structure block (1-char + 2-digits)

		switch (pattern) {
		case 'A': format = "0-9A-ZA-Z"; break;
		case 'B': format = "0-9A-Z"; break;
		case 'C': format = "A-ZA-Z"; break;
		case 'F': format = "0-9"; break;
		case 'L': format = "A-Z"; break;
		case 'U': format = "A-Z"; break;
		case 'W': format = "0-9A-Z"; break;
		}

		result << "([" << format << "]{" << repeats << "})";

		text_iter = match[0].second;
	}

	std::string regex = "^" + result.str() + "$";
	return std::regex(regex.c_str());
}

void Validate::setSelectedSpecification(std::string countryCode)
{
	this->selectedSpec = std::move(this->specifications[countryCode]);
}

bool Validate::isValid(std::string arg)
{
	std::unique_ptr<Specification> spec;
	spec = (std::make_unique<Specification>("", 0, "", arg));
	if (!this->selectedSpec) {
		std::transform(spec->example.begin(), spec->example.end(), spec->example.begin(), toupper);
		spec->countryCode = spec->example.substr(0, 2);
		spec->length = spec->example.length();
		setSelectedSpecification(spec->countryCode);
	}
	if (!(this->selectedSpec == nullptr)) {
		std::string shortened = spec->example.substr(4, spec->example.length());
		return this->selectedSpec->length == spec->length
			&& this->selectedSpec->countryCode.compare(spec->countryCode) == 0
			&& std::regex_match(shortened, parseStructure(this->selectedSpec->structure))
			&& iso7064Mod97_10(spec->example);
	}
	else {
		return false;
	}
}

Validate::Validate()
{
	addSpecification(std::move(std::make_unique<Specification>("AD", 24, "F04F04A12", "AD1200012030200359100100")));
	addSpecification(std::move(std::make_unique<Specification>("AE", 23, "F03F16", "AE070331234567890123456")));
	addSpecification(std::move(std::make_unique<Specification>("AL", 28, "F08A16", "AL47212110090000000235698741")));
	addSpecification(std::move(std::make_unique<Specification>("AT", 20, "F05F11", "AT611904300234573201")));
	addSpecification(std::move(std::make_unique<Specification>("AZ", 28, "U04A20", "AZ21NABZ00000000137010001944")));
	addSpecification(std::move(std::make_unique<Specification>("BA", 20, "F03F03F08F02", "BA391290079401028494")));
	addSpecification(std::move(std::make_unique<Specification>("BE", 16, "F03F07F02", "BE68539007547034")));
	addSpecification(std::move(std::make_unique<Specification>("BG", 22, "U04F04F02A08", "BG80BNBG96611020345678")));
	addSpecification(std::move(std::make_unique<Specification>("BH", 22, "U04A14", "BH67BMAG00001299123456")));
	addSpecification(std::move(std::make_unique<Specification>("BR", 29, "F08F05F10U01A01", "BR9700360305000010009795493P1")));
	addSpecification(std::move(std::make_unique<Specification>("CH", 21, "F05A12", "CH9300762011623852957")));
	addSpecification(std::move(std::make_unique<Specification>("CR", 21, "F03F14", "CR0515202001026284066")));
	addSpecification(std::move(std::make_unique<Specification>("CY", 28, "F03F05A16", "CY17002001280000001200527600")));
	addSpecification(std::move(std::make_unique<Specification>("CZ", 24, "F04F06F10", "CZ6508000000192000145399")));
	addSpecification(std::move(std::make_unique<Specification>("DE", 22, "F08F10", "DE89370400440532013000")));
	addSpecification(std::move(std::make_unique<Specification>("DK", 18, "F04F09F01", "DK5000400440116243")));
	addSpecification(std::move(std::make_unique<Specification>("DO", 28, "U04F20", "DO28BAGR00000001212453611324")));
	addSpecification(std::move(std::make_unique<Specification>("EE", 20, "F02F02F11F01", "EE382200221020145685")));
	addSpecification(std::move(std::make_unique<Specification>("ES", 24, "F04F04F01F01F10", "ES9121000418450200051332")));
	addSpecification(std::move(std::make_unique<Specification>("FI", 18, "F06F07F01", "FI2112345600000785")));
	addSpecification(std::move(std::make_unique<Specification>("FO", 18, "F04F09F01", "FO6264600001631634")));
	addSpecification(std::move(std::make_unique<Specification>("FR", 27, "F05F05A11F02", "FR1420041010050500013M02606")));
	addSpecification(std::move(std::make_unique<Specification>("GB", 22, "U04F06F08", "GB29NWBK60161331926819")));
	addSpecification(std::move(std::make_unique<Specification>("GE", 22, "U02F16", "GE29NB0000000101904917")));
	addSpecification(std::move(std::make_unique<Specification>("GI", 23, "U04A15", "GI75NWBK000000007099453")));
	addSpecification(std::move(std::make_unique<Specification>("GL", 18, "F04F09F01", "GL8964710001000206")));
	addSpecification(std::move(std::make_unique<Specification>("GR", 27, "F03F04A16", "GR1601101250000000012300695")));
	addSpecification(std::move(std::make_unique<Specification>("GT", 28, "A04A20", "GT82TRAJ01020000001210029690")));
	addSpecification(std::move(std::make_unique<Specification>("HR", 21, "F07F10", "HR1210010051863000160")));
	addSpecification(std::move(std::make_unique<Specification>("HU", 28, "F03F04F01F15F01", "HU42117730161111101800000000")));
	addSpecification(std::move(std::make_unique<Specification>("IE", 22, "U04F06F08", "IE29AIBK93115212345678")));
	addSpecification(std::move(std::make_unique<Specification>("IL", 23, "F03F03F13", "IL620108000000099999999")));
	addSpecification(std::move(std::make_unique<Specification>("IS", 26, "F04F02F06F10", "IS140159260076545510730339")));
	addSpecification(std::move(std::make_unique<Specification>("IT", 27, "U01F05F05A12", "IT60X0542811101000000123456")));
	addSpecification(std::move(std::make_unique<Specification>("KW", 30, "U04A22", "KW81CBKU0000000000001234560101")));
	addSpecification(std::move(std::make_unique<Specification>("KZ", 20, "F03A13", "KZ86125KZT5004100100")));
	addSpecification(std::move(std::make_unique<Specification>("LB", 28, "F04A20", "LB62099900000001001901229114")));
	addSpecification(std::move(std::make_unique<Specification>("LC", 32, "U04F24", "LC07HEMM000100010012001200013015")));
	addSpecification(std::move(std::make_unique<Specification>("LI", 21, "F05A12", "LI21088100002324013AA")));
	addSpecification(std::move(std::make_unique<Specification>("LT", 20, "F05F11", "LT121000011101001000")));
	addSpecification(std::move(std::make_unique<Specification>("LU", 20, "F03A13", "LU280019400644750000")));
	addSpecification(std::move(std::make_unique<Specification>("LV", 21, "U04A13", "LV80BANK0000435195001")));
	addSpecification(std::move(std::make_unique<Specification>("MC", 27, "F05F05A11F02", "MC5811222000010123456789030")));
	addSpecification(std::move(std::make_unique<Specification>("MD", 24, "U02A18", "MD24AG000225100013104168")));
	addSpecification(std::move(std::make_unique<Specification>("ME", 22, "F03F13F02", "ME25505000012345678951")));
	addSpecification(std::move(std::make_unique<Specification>("MK", 19, "F03A10F02", "MK07250120000058984")));
	addSpecification(std::move(std::make_unique<Specification>("MR", 27, "F05F05F11F02", "MR1300020001010000123456753")));
	addSpecification(std::move(std::make_unique<Specification>("MT", 31, "U04F05A18", "MT84MALT011000012345MTLCAST001S")));
	addSpecification(std::move(std::make_unique<Specification>("MU", 30, "U04F02F02F12F03U03", "MU17BOMM0101101030300200000MUR")));
	addSpecification(std::move(std::make_unique<Specification>("NL", 18, "U04F10", "NL91ABNA0417164300")));
	addSpecification(std::move(std::make_unique<Specification>("NO", 15, "F04F06F01", "NO9386011117947")));
	addSpecification(std::move(std::make_unique<Specification>("PK", 24, "U04A16", "PK36SCBL0000001123456702")));
	addSpecification(std::move(std::make_unique<Specification>("PL", 28, "F08F16", "PL61109010140000071219812874")));
	addSpecification(std::move(std::make_unique<Specification>("PS", 29, "U04A21", "PS92PALS000000000400123456702")));
	addSpecification(std::move(std::make_unique<Specification>("PT", 25, "F04F04F11F02", "PT50000201231234567890154")));
	addSpecification(std::move(std::make_unique<Specification>("RO", 24, "U04A16", "RO49AAAA1B31007593840000")));
	addSpecification(std::move(std::make_unique<Specification>("RS", 22, "F03F13F02", "RS35260005601001611379")));
	addSpecification(std::move(std::make_unique<Specification>("SA", 24, "F02A18", "SA0380000000608010167519")));
	addSpecification(std::move(std::make_unique<Specification>("SE", 24, "F03F16F01", "SE4550000000058398257466")));
	addSpecification(std::move(std::make_unique<Specification>("SI", 19, "F05F08F02", "SI56263300012039086")));
	addSpecification(std::move(std::make_unique<Specification>("SK", 24, "F04F06F10", "SK3112000000198742637541")));
	addSpecification(std::move(std::make_unique<Specification>("SM", 27, "U01F05F05A12", "SM86U0322509800000000270100")));
	addSpecification(std::move(std::make_unique<Specification>("ST", 25, "F08F11F02", "ST68000100010051845310112")));
	addSpecification(std::move(std::make_unique<Specification>("TL", 23, "F03F14F02", "TL380080012345678910157")));
	addSpecification(std::move(std::make_unique<Specification>("TN", 24, "F02F03F13F02", "TN5910006035183598478831")));
	addSpecification(std::move(std::make_unique<Specification>("TR", 26, "F05F01A16", "TR330006100519786457841326")));
	addSpecification(std::move(std::make_unique<Specification>("VG", 24, "U04F16", "VG96VPVG0000012345678901")));
	addSpecification(std::move(std::make_unique<Specification>("XK", 20, "F04F10F02", "XK051212012345678906")));

	// Angola
	addSpecification(std::move(std::make_unique<Specification>("AO", 25, "F21", "AO69123456789012345678901")));
	// Burkina
	addSpecification(std::move(std::make_unique<Specification>("BF", 27, "F23", "BF2312345678901234567890123")));
	// Burundi
	addSpecification(std::move(std::make_unique<Specification>("BI", 16, "F12", "BI41123456789012")));
	// Benin
	addSpecification(std::move(std::make_unique<Specification>("BJ", 28, "F24", "BJ39123456789012345678901234")));
	// Ivory
	addSpecification(std::move(std::make_unique<Specification>("CI", 28, "U01F23", "CI17A12345678901234567890123")));
	// Cameron
	addSpecification(std::move(std::make_unique<Specification>("CM", 27, "F23", "CM9012345678901234567890123")));
	// Cape Verde
	addSpecification(std::move(std::make_unique<Specification>("CV", 25, "F21", "CV30123456789012345678901")));
	// Algeria
	addSpecification(std::move(std::make_unique<Specification>("DZ", 24, "F20", "DZ8612345678901234567890")));
	// Iran
	addSpecification(std::move(std::make_unique<Specification>("IR", 26, "F22", "IR861234568790123456789012")));
	// Jordan
	addSpecification(std::move(std::make_unique<Specification>("JO", 30, "A04F22", "JO15AAAA1234567890123456789012")));
	// Madagascar
	addSpecification(std::move(std::make_unique<Specification>("MG", 27, "F23", "MG1812345678901234567890123")));
	// Mali
	addSpecification(std::move(std::make_unique<Specification>("ML", 28, "U01F23", "ML15A12345678901234567890123")));
	// Mozambique
	addSpecification(std::move(std::make_unique<Specification>("MZ", 25, "F21", "MZ25123456789012345678901")));
	// Quatar
	addSpecification(std::move(std::make_unique<Specification>("QA", 29, "U04A21", "QA30AAAA123456789012345678901")));
	// Senegal
	addSpecification(std::move(std::make_unique<Specification>("SN", 28, "U01F23", "SN52A12345678901234567890123")));
	// Ukraine
	addSpecification(std::move(std::make_unique<Specification>("UA", 29, "F25", "UA511234567890123456789012345")));
}

