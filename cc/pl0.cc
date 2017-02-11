/** @file pl0.h
 */

#include "pl0.h"

#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

namespace pl0 {
	// convert an OpCode into a string
	string String(OpCode op) {
		switch(op) {
    	case OpCode::pushConst:	return "pushConst";	break;
    	case OpCode::pushVar:	return "pushVar";	break;
    	case OpCode::Pop:		return "Pop";		break;
    	case OpCode::Call:		return "Call";		break;
    	case OpCode::Enter:		return "Enter";		break;
    	case OpCode::Jump:		return "Jump";		break;
    	case OpCode::Jne:		return "Jne";		break;
    	case OpCode::Return:	return "Return";	break;	
		case OpCode::Neg:		return "Neg";		break;
    	case OpCode::Add:		return "Add";		break;
    	case OpCode::Sub:		return "Sub";		break;
    	case OpCode::Mul:		return "Mul";		break;
    	case OpCode::Div:		return "Div";		break;
		case OpCode::Odd:		return "Odd";		break;
    	case OpCode::Equ:		return "Equ";		break;
    	case OpCode::Neq:		return "Neq";		break;
    	case OpCode::LT:		return "LT";		break;
    	case OpCode::LTE:		return "LTE";		break;
    	case OpCode::GT:		return "GT";		break;
    	case OpCode::GTE:		return "GTE";		break;
		default: {
				ostringstream oss;
				oss << "Unknown OpCode: " << static_cast<unsigned>(op) <<  "!" << ends;
				return oss.str();
			}
		}
	}

	/** Dissamble one instruction
	 *
	 * Dissamble the instruction instr, whose location is given as loc, and named lable.
	 *
	 * @param	label	
	 * @param	loc		
	 * @param	instr	
	 *
	 * @return loc+1
	 */
	Word disasm(const string& label, Word loc, const Instr& instr) {
		const unsigned level = instr.level;		// so we don't diplay the level as a charactor
		if (label.empty())
			cout << setw(10) << loc << ": " << String(instr.op);
		else
			cout << label << ": " << setw(5) << loc << ": " << String(instr.op);

    	switch(instr.op) {
    	case OpCode::pushConst:	cout << " "						<< instr.addr;	break;
    	case OpCode::pushVar:	cout << " "	<< level << ", "	<< instr.addr;	break;
    	case OpCode::Pop:		cout << " "	<< level << ", "	<< instr.addr;	break;
    	case OpCode::Call:		cout << " "	<< level << ", "	<< instr.addr;	break;
    	case OpCode::Enter:		cout << " "						<< instr.addr;	break;
    	case OpCode::Jump:		cout << " "						<< instr.addr;	break;
    	case OpCode::Jne:		cout << " "						<< instr.addr;	break;
		default:								// The rest don't use level or address
		break;
		}
		cout << "\n";

		return loc+1;
	}
}
