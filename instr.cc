/** @file instr.cc
 *
 * PL/0C Opcodes and instruction format
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

	{ OpCode::NOT,		OpCodeInfo{ "not",		1			}	},
	{ OpCode::NEG,		OpCodeInfo{ "neg",		1			}	},
	{ OpCode::COMP,		OpCodeInfo{ "comp",		1			}	},
	{ OpCode::RTOI,		OpCodeInfo{ "rtoi",		1			}	},

	// Binary operations

	{ OpCode::ADD,		OpCodeInfo{ "add",		2   		}	},
	{ OpCode::SUB,		OpCodeInfo{ "sub",		2			}	},
	{ OpCode::MUL,		OpCodeInfo{ "mul",		2			}	},
	{ OpCode::DIV,		OpCodeInfo{ "div",		2			}	},
	{ OpCode::REM,		OpCodeInfo{ "rem",		2			}	},

	{ OpCode::OR,		OpCodeInfo{ "or",		2			}	},
	{ OpCode::AND,		OpCodeInfo{ "and",		2			}	},
	{ OpCode::XOR,		OpCodeInfo{ "xor",		2			}	},

	{ OpCode::SHFTL,	OpCodeInfo{ "lshift",	2			}	},
	{ OpCode::SHFTR,	OpCodeInfo{ "rshift",	2			}	},

	{ OpCode::LT,		OpCodeInfo{ "lt",		2			}	},
	{ OpCode::LTE,		OpCodeInfo{ "lte",		2			}	},
	{ OpCode::EQU,		OpCodeInfo{ "equ",		2			}	},
	{ OpCode::GTE,		OpCodeInfo{ "gte",		2			}	},
	{ OpCode::GT,		OpCodeInfo{ "gt",		2			}	},
	{ OpCode::NEQU,		OpCodeInfo{ "nequ",		2			}	},

	{ OpCode::LOR,		OpCodeInfo{ "lor",		2			}	},
	{ OpCode::LAND,		OpCodeInfo{ "land",		2			}	},

	// Push/pop

	{ OpCode::PUSH,		OpCodeInfo{ "push",		1			}	},
	{ OpCode::PUSHVAR,	OpCodeInfo{ "pushVar",	1			}	},
	{ OpCode::EVAL,		OpCodeInfo{ "eval",		2			}	},
	{ OpCode::ASSIGN,	OpCodeInfo{ "assign",	2			}	},

	// Call/return/jump...

	{ OpCode::CALL,		OpCodeInfo{ "call",		0			}	},
	{ OpCode::ENTER,	OpCodeInfo{ "enter",	0			}	},	// Size isn't staticly know
	{ OpCode::RET,		OpCodeInfo{ "ret",		FrameSize	}	},
	{ OpCode::RETF,		OpCodeInfo{ "retf",		FrameSize	}	}, 	// Pops frame, push return value
	{ OpCode::JUMP,		OpCodeInfo{ "jump",		0			}	},
	{ OpCode::JFAIL,		OpCodeInfo{ "jneq",		0			}	},

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
Datum::Unsigned disasm(ostream& out, Datum::Unsigned loc, const Instr& instr, const string label) {
	const int level = instr.level;		// so we don't display level as a character

	cout << fixed;							// Use fixed format for floating point values;

	if (label.size())
		out << label << ": ";

	out << setw(5) << loc << ": " << OpCodeInfo::info(instr.op).name();

	switch(instr.op) {
	case OpCode::PUSH:
	case OpCode::ENTER:
	case OpCode::JUMP:
	case OpCode::JFAIL:
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

