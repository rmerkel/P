/** @file instr.cc
 *
 * Pascal-Lite Opcodes and instruction format
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

	{ OpCode::Neg,		OpCodeInfo{ "neg",		1			}	},

	{ OpCode::ITOR,		OpCodeInfo{ "itor",		1			}	},
	{ OpCode::ITOR2,	OpCodeInfo{ "itor2",	1			}	},
	{ OpCode::RTOI,		OpCodeInfo{ "rtoi",		1			}	},

	// Binary operations

	{ OpCode::Add,		OpCodeInfo{ "add",		2   		}	},
	{ OpCode::Sub,		OpCodeInfo{ "sub",		2			}	},
	{ OpCode::Mul,		OpCodeInfo{ "mul",		2			}	},
	{ OpCode::Div,		OpCodeInfo{ "div",		2			}	},
	{ OpCode::Rem,		OpCodeInfo{ "rem",		2			}	},

	{ OpCode::LT,		OpCodeInfo{ "lt",		2			}	},
	{ OpCode::LTE,		OpCodeInfo{ "lte",		2			}	},
	{ OpCode::EQU,		OpCodeInfo{ "equ",		2			}	},
	{ OpCode::GTE,		OpCodeInfo{ "gte",		2			}	},
	{ OpCode::GT,		OpCodeInfo{ "gt",		2			}	},
	{ OpCode::NEQU,		OpCodeInfo{ "neq",		2			}	},

	{ OpCode::LOR,		OpCodeInfo{ "lor",		2			}	},
	{ OpCode::LAND,		OpCodeInfo{ "land",		2			}	},

	// Push/pop

	{ OpCode::Push,		OpCodeInfo{ "push",		1			}	},
	{ OpCode::PushVar,	OpCodeInfo{ "pushvar",	1			}	},
	{ OpCode::Eval,		OpCodeInfo{ "eval",		2			}	},
	{ OpCode::Assign,	OpCodeInfo{ "assign",	2			}	},

	// Call/return/jump...

	{ OpCode::Call,		OpCodeInfo{ "call",		0			}	},
	{ OpCode::Enter,	OpCodeInfo{ "enter",	0			}	},	// Size isn't staticly know
	{ OpCode::Ret,		OpCodeInfo{ "ret",		FrameSize	}	},
	{ OpCode::Retf,		OpCodeInfo{ "retf",		FrameSize	}	}, 	// Pops frame, push return value
	{ OpCode::Jump,		OpCodeInfo{ "jump",		0			}	},
	{ OpCode::JNEQ,		OpCodeInfo{ "jneq",		0			}	},

	{ OpCode::Halt,		OpCodeInfo{ "halt",		0			}   }
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
	case OpCode::Push:
	case OpCode::Enter:
	case OpCode::Jump:
	case OpCode::JNEQ:
		out << " " << instr.addr;
		break;

	case OpCode::PushVar:
	case OpCode::Call:
		out << " "	<< level << ", " << instr.addr;
		break;

	default:								// The rest don't use level, address or value
		break;
	}
	out << "\n";

	return loc+1;
}
