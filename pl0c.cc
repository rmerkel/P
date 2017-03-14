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
	// class OpCode Info privite static

	/// A OpCodeInfo table, indexed by OpCode's
	const OpCodeInfo::InfoMap OpCodeInfo::opInfoTbl {

			// Unary operations

		{ OpCode::Not,		OpCodeInfo{ "not",		1			}	},
		{ OpCode::neg,		OpCodeInfo{ "neg",		1			}	},
		{ OpCode::comp,		OpCodeInfo{ "comp",		1			}	},

			// Binary operations

		{ OpCode::sub,		OpCodeInfo{ "sub",		2			}	},
		{ OpCode::mul,		OpCodeInfo{ "mul",		2			}	},
		{ OpCode::div,		OpCodeInfo{ "div",		2			}	},
		{ OpCode::rem,		OpCodeInfo{ "rem",		2			}	},

		{ OpCode::bor,		OpCodeInfo{ "bor",		2			}	},
		{ OpCode::band,		OpCodeInfo{ "band",		2			}	},
		{ OpCode::bxor,		OpCodeInfo{ "bxor",		2			}	},
		{ OpCode::lshift,	OpCodeInfo{ "lshift",	2			}	},
		{ OpCode::rshift,	OpCodeInfo{ "rshift",	2			}	},

		{ OpCode::lt,		OpCodeInfo{ "lt",		2			}	},
		{ OpCode::lte,		OpCodeInfo{ "lte",		2			}	},
		{ OpCode::equ,		OpCodeInfo{ "equ",		2			}	},
		{ OpCode::gte,		OpCodeInfo{ "gte",		2			}	},
		{ OpCode::gt,		OpCodeInfo{ "gt",		2			}	},
		{ OpCode::neq,		OpCodeInfo{ "neq",		2			}	},

		{ OpCode::lor,		OpCodeInfo{ "lor",		2			}	},
		{ OpCode::land,		OpCodeInfo{ "land",		2			}	},

			// Push/pop

		{ OpCode::pushConst,OpCodeInfo{ "pushConst",1			}	},
		{ OpCode::pushVar,	OpCodeInfo{ "pushVar",	1			}	},
		{ OpCode::eval,		OpCodeInfo{ "eval",		2			}	},
		{ OpCode::assign,	OpCodeInfo{ "assign",	2			}	},

			// Call/return/jump...

    	{ OpCode::call,		OpCodeInfo{ "call",		0			}	},
		{ OpCode::enter,	OpCodeInfo{ "enter",	0			}	},	// Size isn't staticly know
		{ OpCode::ret,		OpCodeInfo{ "ret",		FrameSize	}	},
		{ OpCode::reti,		OpCodeInfo{ "reti",		FrameSize	}	}, 	// Pops frame, pushed value
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
	Integer disasm(ostream& out, Integer loc, const Instr& instr, const string label) {
		const int level = instr.level;		// so we don't display level as a character
		if (label.size()) out << label << ": ";
		out << setw(5) << loc << ": " << OpCodeInfo::info(instr.op).name();

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
