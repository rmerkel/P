/** @file instr.cc
 *
 * Pascal-lite Opcodes and instruction format
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */


#include <iomanip>
#include <iostream>
#include <sstream>

#include "instr.h"

using namespace std;

// class OpCode Info privite static

/// A OpCodeInfo table, indexed by OpCode's
const OpCodeInfo::InfoMap OpCodeInfo::opInfoTbl {
	// Unary operations

	{ OpCode::NEG,		OpCodeInfo{ "neg",		1			}	},

	{ OpCode::ITOR,		OpCodeInfo{ "itor",		1			}	},
	{ OpCode::ITOR2,	OpCodeInfo{ "itor2",	1			}	},
	{ OpCode::RTOI,		OpCodeInfo{ "rtoi",		1			}	},
	{ OpCode::TRUNC,	OpCodeInfo{ "trunc",	1			}	},
	{ OpCode::ABS,		OpCodeInfo{	"abs",		1			}	},
	{ OpCode::ATAN,		OpCodeInfo{	"atan",		1			}	},
	{ OpCode::EXP,		OpCodeInfo{ "exp",		1			}	},
	{ OpCode::LOG,		OpCodeInfo{ "log",		1			}	},
	{ OpCode::ODD,		OpCodeInfo{ "Odd",		1			}	},
#if 0	// TBD
	{ OpCode::PRED,		OpCodeInfo{ "PROD",		1			}	},
#endif
	{ OpCode::SIN,		OpCodeInfo{ "Sin",		1			}	},
	{ OpCode::SQR,		OpCodeInfo{ "Sqr",		1			}	},
	{ OpCode::SQRT,		OpCodeInfo{ "Sqrt",		1			}	},
#if	0	// TBD
	{ OpCode::SUCC,		OpCodeInfo{ "Succ",		1			}	},
#endif

	// Builtin procedures

	{ OpCode::WRITE,	OpCodeInfo{ "Write",	1			}	},
	{ OpCode::WRITELN,	OpCodeInfo{ "Writeln",	1			}	},

	// Binary operations

	{ OpCode::ADD,		OpCodeInfo{ "add",		2   		}	},
	{ OpCode::SUB,		OpCodeInfo{ "sub",		2			}	},
	{ OpCode::MUL,		OpCodeInfo{ "mul",		2			}	},
	{ OpCode::DIV,		OpCodeInfo{ "div",		2			}	},
	{ OpCode::REM,		OpCodeInfo{ "rem",		2			}	},

	{ OpCode::LT,		OpCodeInfo{ "lt",		2			}	},
	{ OpCode::LTE,		OpCodeInfo{ "lte",		2			}	},
	{ OpCode::EQU,		OpCodeInfo{ "equ",		2			}	},
	{ OpCode::GTE,		OpCodeInfo{ "gte",		2			}	},
	{ OpCode::GT,		OpCodeInfo{ "gt",		2			}	},
	{ OpCode::NEQU,		OpCodeInfo{ "neq",		2			}	},

	{ OpCode::LOR,		OpCodeInfo{ "lor",		2			}	},
	{ OpCode::LAND,		OpCodeInfo{ "land",		2			}	},
	{ OpCode::LNOT,		OpCodeInfo{ "lnot",		2			}	},

	// Push/pop

	{ OpCode::PUSH,		OpCodeInfo{ "push",		1			}	},
	{ OpCode::PUSHVAR,	OpCodeInfo{ "pushvar",	1			}	},
	{ OpCode::EVAL,		OpCodeInfo{ "eval",		2			}	},
	{ OpCode::ASSIGN,	OpCodeInfo{ "assign",	2			}	},

	// Call/return/jump...

	{ OpCode::CALL,		OpCodeInfo{ "call",		0			}	},
	{ OpCode::ENTER,	OpCodeInfo{ "enter",	0			}	},	// Size isn't staticly know
	{ OpCode::RET,		OpCodeInfo{ "ret",		FrameSize	}	},
	{ OpCode::RETF,		OpCodeInfo{ "retf",		FrameSize	}	}, 	// Pops frame, push return value
	{ OpCode::JUMP,		OpCodeInfo{ "jump",		0			}	},
	{ OpCode::JNEQ,		OpCodeInfo{ "jneq",		0			}	},

	{ OpCode::HALT,		OpCodeInfo{ "halt",		0			}   }
};

// public static

/**
 * @note Returns { "nnknown", 0 } if op is an illegal opcode
 * @param op	The OpCode to look u
 * @return		OpCodeInfo for op
 */
const OpCodeInfo& OpCodeInfo::info(OpCode op) {
	auto i = opInfoTbl.find(op);
	if ((opInfoTbl.end() != i))
		return i->second;

	else {
		static const OpCodeInfo unknown("unknown", 0);
		return unknown;
	}
}

/**
 * @param	out		Where to write the results
 * @param	loc		Address of the instruction
 * @param	instr	The instruction to disassemble
 * @param	label	Display label
 * @return 	loc+1
 */
unsigned disasm(ostream& out, unsigned loc, const Instr& instr, const string label) {
	const int level = instr.level;		// so we don't display level as a character

	cout << fixed;							// Use fixed format for floating point values;

	if (label.size())
		out << label << ": ";

	out << setw(5) << loc << ": " << OpCodeInfo::info(instr.op).name();

	switch(instr.op) {
	case OpCode::PUSH:
	case OpCode::ENTER:
	case OpCode::JUMP:
	case OpCode::JNEQ:
		out << " " << instr.addr;
		break;

	case OpCode::PUSHVAR:
	case OpCode::CALL:
		out << " "	<< level << ", " << instr.addr;
		break;

	default:								// The rest don't use level, address or value
		break;
	}
	out << "\n";

	return loc+1;
}
