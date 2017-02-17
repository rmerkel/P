/** @file pl0.h
 *
 * PL/0 Machine utilities
 */

#include "pl0.h"

#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

namespace pl0 {
	string String(OpCode op) {
		switch(op) {
    	case OpCode::pushConst:	return "pushConst";	break;
    	case OpCode::pushVar:	return "pushVar";	break;
    	case OpCode::pop:		return "pop";		break;
    	case OpCode::call:		return "call";		break;
    	case OpCode::enter:		return "enter";		break;
    	case OpCode::jump:		return "jump";		break;
    	case OpCode::jneq:		return "jneq";		break;
    	case OpCode::ret:		return "return";	break;	
		case OpCode::neg:		return "neg";		break;
    	case OpCode::add:		return "add";		break;
    	case OpCode::sub:		return "sub";		break;
    	case OpCode::mul:		return "mul";		break;
    	case OpCode::div:		return "div";		break;
		case OpCode::odd:		return "odd";		break;
    	case OpCode::equ:		return "equ";		break;
    	case OpCode::neq:		return "neq";		break;
    	case OpCode::lt:		return "lt";		break;
    	case OpCode::lte:		return "lte";		break;
    	case OpCode::gt:		return "gt";		break;
    	case OpCode::gte:		return "gte";		break;
		default: {
				ostringstream oss;
				oss << "Unknown OpCode: " << static_cast<unsigned>(op) <<  "!" << ends;
				return oss.str();
			}
		}
	}

	/** Disassemble an instruction
	 *
	 * @param	loc		Address of the instruction
	 * @param	instr	The instruction to disassemble
	 * @param	label	Display label
	 * @return loc+1
	 */
	Word disasm(Word loc, const Instr& instr, const string label) {
		const unsigned level = instr.level;		// so we don't diplay the level as a charactor
		if (label.empty())
			cout << setw(10) << loc << ": " << String(instr.op);
		else
			cout << label << ": " << setw(5) << loc << ": " << String(instr.op);

    	switch(instr.op) {
    	case OpCode::pushConst:	cout << " "                     << instr.addr;	break;
    	case OpCode::pushVar:	cout << " "	<< level << ", "	<< instr.addr;	break;
    	case OpCode::pop:		cout << " "	<< level << ", "	<< instr.addr;	break;
    	case OpCode::call:		cout << " "	<< level << ", "	<< instr.addr;	break;
    	case OpCode::enter:		cout << " "						<< instr.addr;	break;
    	case OpCode::jump:		cout << " "						<< instr.addr;	break;
    	case OpCode::jneq:		cout << " "						<< instr.addr;	break;
		default:								// The rest don't use level or address
		break;
		}
		cout << "\n";

		return loc+1;
	}
}
