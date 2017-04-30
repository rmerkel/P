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

	// Data converstion
	
	{ OpCode::itor,		OpCodeInfo{ "itor",		1   		}	},
	{ OpCode::rtoi,		OpCodeInfo{ "rtoi",		1   		}	},

	// Unary operations

	{ OpCode::noti,		OpCodeInfo{ "not.i",	1			}	},
	{ OpCode::notr,		OpCodeInfo{	"not.r",	1			}	},
	{ OpCode::negi,		OpCodeInfo{ "neg.i",	1			}	},
	{ OpCode::negr,		OpCodeInfo{	"neg.r",	1   		}	},
	{ OpCode::comp,		OpCodeInfo{ "comp.u",	1			}	},

	// Binary operations

	{ OpCode::addi,		OpCodeInfo{ "add.i",	2   		}	},
	{ OpCode::addr,		OpCodeInfo{ "add.r",	2   		}	},
	{ OpCode::subi,		OpCodeInfo{ "sub.i",	2			}	},
	{ OpCode::subr,		OpCodeInfo{ "sub.r",	2   		}	},
	{ OpCode::muli,		OpCodeInfo{ "mul.i",	2			}	},
	{ OpCode::mulr,		OpCodeInfo{ "mul.r",	2   		}	},
	{ OpCode::divi,		OpCodeInfo{ "div.i",	2			}	},
	{ OpCode::divr,		OpCodeInfo{ "div.r",	2   		}	},
	{ OpCode::remi,		OpCodeInfo{ "remi",		2			}	},
	{ OpCode::remr,		OpCodeInfo{ "remr",		2   		}	},

	{ OpCode::bor,		OpCodeInfo{ "bor",		2			}	},
	{ OpCode::band,		OpCodeInfo{ "band",		2			}	},
	{ OpCode::bxor,		OpCodeInfo{ "bxor",		2			}	},
	{ OpCode::lshift,	OpCodeInfo{ "lshift",	2			}	},
	{ OpCode::rshift,	OpCodeInfo{ "rshift",	2			}	},

	{ OpCode::lti,		OpCodeInfo{ "lt.i",		2			}	},
	{ OpCode::ltr,		OpCodeInfo{ "lt.r",		2			}	},
	{ OpCode::ltei,		OpCodeInfo{ "lte.i",	2			}	},
	{ OpCode::lter,		OpCodeInfo{ "lte.r",	2			}	},
	{ OpCode::equi,		OpCodeInfo{ "equ.i",	2			}	},
	{ OpCode::equr,		OpCodeInfo{ "equ.r",	2			}	},
	{ OpCode::gtei,		OpCodeInfo{ "gte.i",	2			}	},
	{ OpCode::gter,		OpCodeInfo{ "gte.r",	2			}	},
	{ OpCode::gti,		OpCodeInfo{ "gt.i",		2			}	},
	{ OpCode::gtr,		OpCodeInfo{ "gt.r",		2			}	},
	{ OpCode::neqi,		OpCodeInfo{ "neq.i",	2			}	},
	{ OpCode::neqr,		OpCodeInfo{ "neq.r",	2			}	},

	{ OpCode::lori,		OpCodeInfo{ "lor.i",	2			}	},
	{ OpCode::lorr,		OpCodeInfo{ "lor.r",	2			}	},
	{ OpCode::landi,	OpCodeInfo{ "land.i",	2			}	},
	{ OpCode::landr,	OpCodeInfo{ "land.r",	2			}	},

	// Push/pop

	{ OpCode::pushi,	OpCodeInfo{ "push.i",	1			}	},
	{ OpCode::pushr,	OpCodeInfo{ "push.r",	1   		}	},
	{ OpCode::pushVar,	OpCodeInfo{ "pushVar",	1			}	},
	{ OpCode::eval,		OpCodeInfo{ "eval",		2			}	},
	{ OpCode::assign,	OpCodeInfo{ "assign",	2			}	},

	// Call/return/jump...

	{ OpCode::call,		OpCodeInfo{ "call",		0			}	},
	{ OpCode::enter,	OpCodeInfo{ "enter",	0			}	},	// Size isn't staticly know
	{ OpCode::ret,		OpCodeInfo{ "ret",		FrameSize	}	},
	{ OpCode::retf,		OpCodeInfo{ "retf",		FrameSize	}	}, 	// Pops frame, push return value
	{ OpCode::jump,		OpCodeInfo{ "jump",		0			}	},
	{ OpCode::jneq,		OpCodeInfo{ "jneq",		0			}	},

	{ OpCode::halt,		OpCodeInfo{ "halt",		0			}   }
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

	if (label.size())
		out << label << ": ";

	out << setw(5) << loc << ": " << OpCodeInfo::info(instr.op).name();
	switch(instr.op) {
	case OpCode::pushi:
	case OpCode::pushr:
	case OpCode::enter:
	case OpCode::jump:
	case OpCode::jneq:
		out << " " << instr.addr;
		break;

	case OpCode::pushVar:
		out << " "	<< level << ", " << instr.addr;
		break;

	case OpCode::call:
		out << " "	<< level << ", " << instr.addr;
		break;

	default:								// The rest don't use level, address or value
		break;
	}
	out << "\n";

	return loc+1;
}
