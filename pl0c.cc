/** @file pl0c.cc
 *
 * PL/0C utilities 
 *  
 * @author Randy Merkel, Slowly but Surly Software. 
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved. 
 */

#include "pl0c.h"

#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

namespace pl0c {
	/**
	 * @param op The OpCode, whose name we'll return 
	 * @return 	op's name. 
	 */
	string toString(OpCode op) {
		switch(op) {
		case OpCode::Not:		return "not";		break;
		case OpCode::neg:		return "neg";		break;
		case OpCode::comp:		return "comp";		break;

		case OpCode::add:		return "add";		break;
		case OpCode::sub:		return "sub";		break;
		case OpCode::mul:		return "mul";		break;
		case OpCode::div:		return "div";		break;
		case OpCode::rem:		return "rem";		break;

		case OpCode::bor:		return "bor";		break;
		case OpCode::band:		return "band";		break;
		case OpCode::bxor:		return "bxor";		break;

		case OpCode::lshift:	return "lshift";	break;
		case OpCode::rshift:	return "rshift";	break;

		case OpCode::lt:		return "lt";		break;
		case OpCode::lte:		return "lte";		break;
		case OpCode::equ:		return "equ";		break;
		case OpCode::gte:		return "gte";		break;
		case OpCode::gt:		return "gt";		break;
		case OpCode::neq:		return "neq";		break;

		case OpCode::lor:		return "lor";		break;
		case OpCode::land:		return "land";		break;

		case OpCode::pushConst:	return "pushConst";	break;
		case OpCode::pushVar:	return "pushVar";	break;
		case OpCode::eval:		return "eval";		break;
		case OpCode::assign:	return "assign";	break;

    	case OpCode::call:		return "call";		break;
		case OpCode::enter:		return "enter";		break;
		case OpCode::ret:		return "ret";		break;
		case OpCode::reti:		return "reti";		break;
    	case OpCode::jump:		return "jump";		break;
		case OpCode::jneq:		return "jneq";		break;

		default: {
				ostringstream oss;
				oss << "Unknown OpCode: " << static_cast<unsigned>(op) <<  "!" << ends;
				return oss.str();
			}
		}
	}

	/** 
	 * @param	out		Where to write the results 
	 * @param	loc		Address of the instruction
	 * @param	instr	The instruction to disassemble
	 * @param	label	Display label
	 * @return loc+1
	 */
	Integer disasm(ostream& out, Integer loc, const Instr& instr, const string label) {
		const int level = instr.level;		// so we don't display level as a character
		if (label.size()) out << label << ": ";
		out << setw(5) << loc << ": " << toString(instr.op);

    	switch(instr.op) {
    	case OpCode::pushConst:
    	case OpCode::enter:
    	case OpCode::jump:
    	case OpCode::jneq:
    		out << " " << instr.addr;
    		break;

    	case OpCode::pushVar:
    	case OpCode::call:
    		out << " "	<< level << ", " << instr.addr;
    		break;

		default:								// The rest don't use level or address
			break;
		}
		out << "\n";

		return loc+1;
	}
}
