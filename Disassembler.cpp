/* Alexander Ray
 * RedID: 823978916
 * Sawyer Thompson
 * RedID: 823687079
 * October 29th, 2021
 * CS 530-2
 * Professor Shen
 */

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <limits>

// Stores the mnemonic labels that correspond to the ops
std::string mnemonics[] = {
"ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP",
"COMPF", "COMPR", "DIV", "DIVF", "DIVR", "FIX",
"FLOAT", "HIO", "J", "JEQ", "JGT", "JLT",
"JSUB", "LDA", "LDB", "LDCH", "LDF", "LDL",
"LDS", "LDT", "LDX", "LPS", "MUL", "MULF",
"MULR", "NORM", "OR", "RD", "RMO", "RSUB",
"SHIFTL", "SHIFTR", "SIO", "SSK", "STA", "STB",
"STCH", "STF", "STI", "STL","STS", "STSW",
"STT", "STX", "SUB", "SUBF", "SUBR", "SVC",
"TD", "TIO", "TIX", "TIXR", "WD"};

// Stores the ops values that correspond to the mnemonic
std::string ops[] = {
"18", "58", "90", "40", "b4", "28",
"88", "a0", "24", "64", "9c", "c4",
"c0", "f4", "3c", "30", "34", "38",
"48", "00", "68", "50", "70", "08",
"6C", "74", "04", "d0", "20", "60",
"98", "c8", "44", "d8", "ac", "4c",
"a4", "a8", "f0", "ec", "0c", "78",
"54", "80", "d4", "14", "7c", "e8",
"84", "10", "1c", "5c", "94", "b0",
"e0", "f8", "2c", "b8", "dc"};

// Stores the format2
std::string f2[] = {
"A", "X", "L", "B", "S", "T",
"F", "PC", "SW"};

// Stores the location, base, and index across the class
int loc = 0;
int base = 0;
int indexX = 0;
//initializes end address
int programEndAddr;
//Stores the fileName
std::string fileName;
//Stores the mpas used throughout the program
std::map< int, std::string> format2;
std::map<std::string, std::string> opsMnemonics;
std::map<std::string, std::string> symbols;
std::map<std::string, std::pair<std::string,std::string> > literals;

// Gets the Opcodes from the first two bytes of object code
std::string getOps(std::string first2bytes) {
	std:: stringstream paddedhex;
	int decimal;
	decimal = stoi(first2bytes, 0, 16);
	// clears the ni bits by shifting back and forth.
	decimal>>=2;
	decimal<<=2;
	paddedhex << std::hex << std::setw(2);
	paddedhex << std::setfill('0') << decimal;

	return paddedhex.str();
}

/* Checks the NI bits of the object code to test
 * if it is simple, indirect or immediate
 */
std::string checkNI(std::string secondByte) {
	int value,n;
	value = stoi(secondByte, 0, 16);
	n = value>>1;
	if (n % 2 != 0 && value % 2 == 0) {
		return "@";
	}
	if (n % 2 == 0 && value % 2 != 0) {
		return "#";
	}
	return "";
}

/* Checks the X bit of the object code to test
 * if it is indexed
 */
std::string checkX(std::string thirdByte) {
	int value,x;
	value = stoi(thirdByte, 0, 16);
	x = value>>3;
	if (x % 2 != 0) {
		return ",X";
	}
	return "";
}

/* Checks the format 2 last two bytes to
 * see what register to return
 */
std::string checkLast2(std::string byte1, std::string byte2) {
	std::string rString,s1,s2;
	int value, value2;
	std::stringstream output;
	value = stoi(byte1, 0, 16);
	value2 = stoi(byte2, 0, 16);
	//checks the map to see what registers correspond to the object code
	s1 = format2[value];
	s2 = format2[value2];
	rString.append(s1 + ","+ s2);

	if (value != 0) {
		output <<std::left<< std::setw(12)<<rString;
		return output.str();
	}
	output <<std::left<< std::setw(12)<<s1;
	return output.str();
}

// Check the Literals and returns 1 (format"1") if a literal
int checkLiteral(int loc) {
    std:: stringstream location;
    location << std::hex << std::setw(6) << std::uppercase;
    location << std::setfill('0') << loc;
    for (auto it = literals.begin(); it != literals.end(); it++)
    {
        if (location.str() == it->first) {
            return 1;
        }
    }
    return 0;
}

/* Grabs the displacement and checks if it is base, PC Relative
 * and returns the labels of the given object code for format3
 */
std::string Displacement(std::string imed,std::string XBPE, std::string last3) {
    int value,value2,x,b,p;
    std::string rString;
    value2 = stoi(XBPE, 0, 16);
    x = value2>>3;
    b = value2>>2;
    p = value2>>1;
    value = stoi(last3, 0, 16);
    std:: stringstream paddedhex;
    std:: stringstream outputpadded;
    //If immediate it returns the Hex value of the last 3 bytes of the object code
    if (imed == "#") {
        paddedhex << std::hex << value;
        outputpadded <<std::left<< std::setw(11) << paddedhex.str();
        return outputpadded.str();
    }
    //updates base
    if (b%2 == 1) {
        value +=base;
    }
    //updates the value based on the PC realative
    if (p%2 == 1) {
		if (value>=0x800) {	
			value = loc - ((0xFFF - value) + 1);
        }
		else {
        		value+=loc;
		}
    }
    //adds the index
    if (x%2 == 1) {
            value+=indexX;
    }
    paddedhex << std::hex << std::setw(6) << std::uppercase;
    paddedhex << std::setfill('0') << value;
    auto symbol = symbols.find(paddedhex.str());
    auto lit = literals.find(paddedhex.str());
	// returns the literal with the proper padding
    if (checkLiteral(value) > 0) {
		if ((lit->second).first == "      ") {
			outputpadded <<std::left<< std::setw(12) << (lit->second).second;
			return outputpadded.str();
        }
		outputpadded <<std::left<< std::setw(12) << (lit->second).first;
		return outputpadded.str();
    }
	// default padding for operand
    if((symbol) != symbols.end()){
    	outputpadded <<std::left<< std::setw(12) << (symbol->second);
    }
    return outputpadded.str();
}

/* Grabs the displacement and checks if it is base, PC Relative
 * and returns the labels of the given object code for format4
 * Uses the last 5 digits instead of 4 since it's format 4
 */
std::string Displacementf4(std::string imed,std::string XBPE, std::string last5) {
    int i,value,x;
    value = stoi(last5, 0, 16);
    x = value>>3;
    std:: stringstream paddedhex;
    std:: stringstream outputpadded;
    paddedhex << std::hex << std::setw(6) << std::uppercase;
    if (imed == "#") {
        paddedhex << std::setfill('0') << last5;
        outputpadded <<std::left<< std::setw(11) << symbols[paddedhex.str()];
        return outputpadded.str();
    }
    if (x%2 == 1) {
            value+=indexX;
    }
    paddedhex << std::setfill('0') << last5;
    // stores either literal or symbol
    auto symbol = symbols.find(paddedhex.str());
    auto lit = literals.find(paddedhex.str());
    if (checkLiteral(value) >0) {
		if ((lit->second).first == "      ") {
			outputpadded <<std::left<< std::setw(12) << (lit->second).second;
            return outputpadded.str();
        }
		outputpadded <<std::left<< std::setw(12) << (lit->second).first;
        return outputpadded.str();
    }
    outputpadded <<std::left<< std::setw(12) << (symbol->second);
    return outputpadded.str();
}

// Returns the size of the literal
int getLiteralSize(std::string &literal) {
	return literal.substr(literal.find_first_of("\'") + 1,literal.find_last_of("\'") - literal.find_first_of("\'") - 1 ).length() / 2;
}

// Checks if format 2, 3, or 4 using the first 2 bytes and the third byte
int checkFormat(std::string f2byt, std::string thirdbyte) {
	int value, value2, n;
	value = stoi(thirdbyte, 0, 16);
	value2 = stoi(f2byt, 0, 16);

	//shifts the bit over 1 to check if there is an n bit
	n = value2>>1;
	//checks if n and i bits are 00.
	if (value2 % 2 == 0 && n % 2 == 0 && f2byt!="00") {
		return 2;
	}
	//checks if e bit is 0
	 else if (value % 2 != 1) {
		return 3;
	}
	return 4;
}

// sets the format two map using the f2 array.
void setformat2(std::string format[]) {
	int i;
	for(i = 0; i<9; i++) {

		if (i>=7) {
			format2.insert(std::make_pair(i+1,f2[i]));
		}else{
			format2.insert(std::make_pair(i,f2[i]));
		}
	}
}

// sets the opsMnemonics map
void setInstructions(std::string ops[], std::string mnemonics[]) {
	int i;
	for (i = 0; i<59; i++) {
		opsMnemonics.insert(std::make_pair(ops[i],mnemonics[i]));
	}
}

// Returns the label of the symbol table
std::string getLabel () {
	std::stringstream curAddr;
	curAddr << std::hex << std::setw(6) << std::setfill('0') << std::uppercase << loc;
	std::string label = "      ";
	auto symbol = symbols.find(curAddr.str());
	
	if (symbol != symbols.end()) {
		label = symbol->second;
	}
	return label;
}

// Puts the RESB label and size into the output file
void putResb(std::string strAddr, std::ofstream &outputFile) {
	int istrAddr = std::stoi(strAddr, 0, 16);
	//loops through while the location is less that istrAddr
	while (loc < istrAddr) {
		std::string label = getLabel();
		std::stringstream location;
		int size = 10;
		location << std::setfill('0') << std::setw(6) << std::hex << std::uppercase << loc;
		//puts the label resb into the file
		outputFile << location.str().substr(2,4) << "    " << label << "  RESB    ";
		int nextAddr = istrAddr;
		//finds the symbol at the given location
		auto symIt = symbols.find(location.str());
		//checks if the symbol is not the last one in a given file
		if (++symIt != symbols.end()) {
			//ensures that the address of the table is less than the nextAddr
			if (std::stoi(symIt->first,0,16) < nextAddr) {
				nextAddr = std::stoi(symIt->first,0,16);
			}
		}
		//stores the size of the bytes which is the next address - the location
		size = nextAddr - loc;

		outputFile << std::dec << size << "\n";
		//updates location
		loc += size;
	}
}

// adds the base value to Base
int addBase(std::string label) {
    for (auto it = symbols.begin(); it != symbols.end(); it++) {
        if (it->second == label) {
            return stoi(it->first,0,16);
        }
    }
    return 0;
}

/*trims string of blank space*/
std::string trimEnd(std::string in){
	std:: string out;
	int i;
	for(i = in.length()-1;i>=0;i--) {
		if(in[i] != ' '){
			return in.substr(0,i+1);
		}
	}
	return in;
}
// Puts the Instructions onto the output file
void putInstructions(std::string line, std::ofstream &outputFile) {
	//initializes the necessary variables
	std::string key,output;
	std::string lowerCase = line;
	int i, format,end;
	//ensures that no problems are ran into due to case sensitivity
	std::transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(), ::tolower);
	//grabs the number of object codes in the file that are used for creating the output file
	int numberObjectCodes = 2 * stoi(line.substr(7,2), 0, 16);
	//Loops through a line of object code
	for(i = 9; i<numberObjectCodes+9;) {
		//checks if literal makes it an honorary format 1
		if (checkLiteral(loc) == 1) {
			format = 1;
		}
		else {
		//checks if format 2, 3, or 4
		format = checkFormat(lowerCase.substr(i,2),lowerCase.substr(i+2,1));
		}
		//gets the mnemonics of the first 2 bits
		key = getOps(lowerCase.substr(i,2));
		std::stringstream ss;
		ss << std::left<<std::setw(8)<<std::setfill(' ')<<opsMnemonics[key];
		output = ss.str();
		std:: stringstream location;
        location << std::hex << std::setw(6) << std::uppercase;
        location << std::setfill('0') << loc;
        //adds specifications for literals
		if (format == 1) {
			auto it =literals.find(location.str());
			//adds LTORG
			if ((it->second).second.substr(0,1) == "=") {
				outputFile << "                LTORG\n";
			}
            outputFile << std::setfill('0') << std::setw(4) << std::hex << loc << "    ";
            outputFile << (it->second).first << "  ";
			//if statement for last line of code
			if ((it->second).second.substr(0,1) == "=") {
				outputFile << "*       ";
			}
			else {
			outputFile << "BYTE    ";
			}
			outputFile << (it->second).second << "  ";
            outputFile << line.substr(i,getLiteralSize((it->second).second) * 2) + "  \n";
            //updates location
			loc += getLiteralSize((it->second).second);
			//skips correct number of object code in loop
            i+= getLiteralSize((it->second).second) * 2;
            
        }
		else if (format == 2) {
			//adds location into file, and fills 4 bits long
			outputFile <<std::right<< std::setfill('0') << std::setw(4) << std::hex << std::uppercase << loc << "    ";
			outputFile << getLabel() << "  ";
			//updates location
			loc += 2;
			//outputs the Mnemonic at the given ops, key
			outputFile << output;
			//outputs the register associated with format 2
			outputFile <<checkLast2(lowerCase.substr(i+2,1),lowerCase.substr(i+3,1));
			// outputs object code
			outputFile << line.substr(i,4)<< '\n';
			i+= 4;
		}
		else if (format == 3) {
			outputFile << std::right<<std::setfill('0') << std::setw(4) << std::hex << std::uppercase << loc << "    ";
			outputFile << getLabel() << "  ";
			loc += 3;
			outputFile << output;
			//checks NI bits and outputs @ or # or
			outputFile << checkNI(lowerCase.substr(i+1,1));
			//Checks the displacement as well as the NI bits to see what label should be returned
			std::string operand = Displacement(checkNI(lowerCase.substr(i+1,1)),lowerCase.substr(i+2,1),line.substr(i+3,3));
			//removes a space at the end to account for the @ sign in front of the operand
			if (checkNI(lowerCase.substr(i+1,1)) == "@") {
				operand.erase(8,1);
			}
			//checks if object code is indexed
			if(checkX(lowerCase.substr(i+2,1)) == ",X"){
				std::string out = trimEnd(operand);
				out.append(",X");
				outputFile << std::left<<std::setfill(' ') << std::setw(12) << out;
			}else{
			outputFile << operand;
			}
			outputFile << line.substr(i,6)<< '\n';
			//checks if key opcode is associated with LDB to load the base
			if (opsMnemonics[key]=="LDB") {
            	outputFile <<"                "<<"BASE"<< "    "<< Displacement(checkNI(lowerCase.substr(i+1,1)),lowerCase.substr(i+2,1),line.substr(i+3,3))<<"\n";
				base = addBase(Displacement(checkNI(lowerCase.substr(i+1,1)),lowerCase.substr(i+2,1),line.substr(i+3,3)).substr(0,6));
            }
			//checks if key opcode is associated with LDB to update index
			if (opsMnemonics[key] == "LDX") {
				indexX = std::stoi(Displacement(checkNI(lowerCase.substr(i+1,1)),lowerCase.substr(i+2,1),line.substr(i+3,3)).substr(0,6),0,16);
			}
			i+= 6;
			

		}
		else{
			outputFile <<std::right<< std::setfill('0') << std::setw(4) << std::hex << std::uppercase << loc << "    ";
			outputFile << getLabel() << "  ";
			loc += 4;
			// erases a space at the end to account for the + sign
			output.erase(7,1);

			outputFile << "+" <<output;
			outputFile <<checkNI(lowerCase.substr(i+1,1));
			//Checks the displacement as well as the NI bits to see what label should be returned
			std::string operand = Displacementf4(checkNI(lowerCase.substr(i+1,1)),lowerCase.substr(i+2,1),line.substr(i+3,5));
			//checks if object code is indexed
			if(checkX(lowerCase.substr(i+2,1)) == ",X"){
				std::string out = trimEnd(operand);
				out.append(",X");
				outputFile << std::left<<std::setfill(' ') << std::setw(12) << out;
			}else{
			outputFile << operand;
			}
			outputFile <<line.substr(i,8)<< '\n';
			if (opsMnemonics[key]=="LDB") {
                outputFile <<"                "<<"BASE"<< "    "<< Displacementf4(checkNI(lowerCase.substr(i+1,1)),lowerCase.substr(i+2,1),line.substr(i+3,5))<<"\n";
				base = addBase(Displacementf4(checkNI(lowerCase.substr(i+1,1)),lowerCase.substr(i+2,1),line.substr(i+3,5)).substr(0,6));
			}
			if (opsMnemonics[key] == "LDX") {
				indexX = std::stoi(Displacement(checkNI(lowerCase.substr(i+1,1)),lowerCase.substr(i+2,1),line.substr(i+3,5)).substr(0,6),0,16);
			}
			i+=8;
		}

	}
}

// Runs the header
void runHeader (std::string &line, std::ofstream &outputFile) {
    std::string name = line.substr (1,6);
    const std::string strAddr = line.substr (7,6);
    int istrAddr = std::stoi(strAddr);
    //stores the fileName
	fileName = name;
	//stores the starting string
	programEndAddr = istrAddr + std::stoi(line.substr(13,6),0,16);
	//Outputs the first line of code
    outputFile << strAddr.substr (0,4) + "    ";
	outputFile << name + "  ";
	outputFile << "START   ";
	outputFile << istrAddr << std::endl;

}
// Runs text object codes
void runText (std::string &line, std::ofstream &outputFile) {
	std::string strAddr = line.substr (1, 6);
	int istrAddr = std::stoi(strAddr);
	int length = std::stoi(line.substr(7,2));
	putResb(strAddr, outputFile);
	putInstructions(line, outputFile);
}

// Runs end object code
void runEnd (std::string &line, std::ofstream &outputFile) {
	std::stringstream addr;
	addr << std::setfill('0') << std::setw(6) << std::hex << std::uppercase << programEndAddr;
	//checks for a RESB directive between the last Text record and the End Record
	putResb(addr.str(), outputFile);
	const std::string strAddr = line.substr (1,6);
		outputFile << "                END     ";
		outputFile << fileName << "\n";
}

// loads the symbol file onto the symbol map
void loadSymbols(std::fstream &symFile) {
	std::string line;
	//breaks down the symbol file
	while (std::getline(symFile, line)) {
		// Skips through extra lines and stops at the literal table
		if (line.empty() || line.substr(0,1) == "\r" || line.substr(0,1) == "\n") continue;
		if (line.substr(0,6) == "Symbol") continue;
		if (line.substr(0,1) == "-") continue;
		if (line.substr(0,4) == "Name") break;
		
		symbols.insert(std::pair<std::string,std::string>(line.substr(8,6), line.substr(0,6)));
	}
	
	
}

// loads the literals onto the literal map
void loadLiterals(std::fstream &symFile) {
	std::string line;
	while (std::getline(symFile, line)) {
		if (line.substr(0,1) != "-") {
			literals.insert(std::pair<std::string, std::pair<std::string, std::string> >
				(line.substr(line.length() - 6, 6), std::pair<std::string, std::string>(line.substr(0,6), line.substr(8,10))));
		}
	}
}


int main(int argc, char *argv[])
{
	//sets format maps
	setformat2(f2);
	setInstructions(ops,mnemonics);
	//makes sure correct number of arguments are passed
	if (argc != 3)
	{
		std::cout << "Improper arguments!" << std::endl;
		std::cout << "Usage: ./dissem test.obj test.sym" << std::endl;
		return 1;
	}
	std::ofstream outputFile ("out.lst");
	std::fstream objFile(argv[1]);
	std::string line;
	std::fstream symFile(argv[2]);
	//loads symbols and literals into the maps
	loadSymbols(symFile);
	loadLiterals(symFile);
	//checks if objFile is correct
	if (objFile.is_open() && symFile.is_open()) {
		while (std::getline(objFile, line))
		{
			if (line[0] == 'H') {
				runHeader(line,outputFile);
			}
			else if (line[0] == 'T') {
				runText(line,outputFile);
			}
			else if (line[0] == 'E') {
				runEnd(line,outputFile);
			}
		}
	} 
	else {
		std::cout << "Missing a File" << std::endl;
		return 1;
	}
	//closes files
	objFile.close();
	symFile.close();
}
