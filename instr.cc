/********************************************************************************************//**
 * @file instr.cc
 *
 * P machine Opcodes and instruction format
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/


#include <iomanip>
#include <iostream>
#include <sstream>

#include "instr.h"

using namespace std;

/********************************************************************************************//**
 * class OpCodeInfo
 *
 * privite static
 ************************************************************************************************/

/********************************************************************************************//**
 * A OpCodeInfo table, indexed by OpCode's
 ************************************************************************************************/
const OpCodeInfo::InfoMap OpCodeInfo::opInfoTbl {
	// Unary operations

	{ OpCode::NEG,		OpCodeInfo{ "neg",		1			} },

	{ OpCode::ITOR,		OpCodeInfo{ "itor",		1			} },
	{ OpCode::ITOR2,	OpCodeInfo{ "itor2",	2			} },
	{ OpCode::ROUND,	OpCodeInfo{ "round",	1			} },
	{ OpCode::TRUNC,	OpCodeInfo{ "trunc",	1			} },
	{ OpCode::ABS,		OpCodeInfo{	"abs",		1			} },
	{ OpCode::ATAN,		OpCodeInfo{	"atan",		1			} },
	{ OpCode::EXP,		OpCodeInfo{ "exp",		1			} },
	{ OpCode::LOG,		OpCodeInfo{ "log",		1			} },

	{ OpCode::DUP,		OpCodeInfo{ "dup",		1			} },
	{ OpCode::ODD,		OpCodeInfo{ "Odd",		1			} },
	{ OpCode::PRED,		OpCodeInfo{ "pred",		1			} },
	{ OpCode::SUCC,		OpCodeInfo{ "succ",		1			} },

	{ OpCode::SIN,		OpCodeInfo{ "sin",		1			} },
	{ OpCode::SQR,		OpCodeInfo{ "sqr",		1			} },
	{ OpCode::SQRT,		OpCodeInfo{ "sqrt",		1			} },

	// Builtin procedures

	{ OpCode::GET,		OpCodeInfo{ "get",		2			} },
	{ OpCode::GETLN,	OpCodeInfo{ "get_line",	2			} },
	{ OpCode::PUT,		OpCodeInfo{ "put",		4			} },
	{ OpCode::PUTLN,	OpCodeInfo{ "put_line",	4			} },
	{ OpCode::PUTF,		OpCodeInfo{ "putf",		5			} },
	{ OpCode::PUTFLN,	OpCodeInfo{ "putfln",	5			} },
	{ OpCode::NEW,		OpCodeInfo{ "new",		1			} },
	{ OpCode::DISPOSE,	OpCodeInfo{ "dispose",	1			} },

	// Binary operations

	{ OpCode::ADD,		OpCodeInfo{ "add",		2   		} },
	{ OpCode::SUB,		OpCodeInfo{ "sub",		2			} },
	{ OpCode::MUL,		OpCodeInfo{ "mul",		2			} },
	{ OpCode::DIV,		OpCodeInfo{ "div",		2			} },
	{ OpCode::REM,		OpCodeInfo{ "rem",		2			} },

	{ OpCode::BNOT,		OpCodeInfo{ "bitand",	1			} },
	{ OpCode::BAND,		OpCodeInfo{ "bitand",	2			} },
	{ OpCode::BOR,		OpCodeInfo{ "bitor",	2			} },
	{ OpCode::BXOR,		OpCodeInfo{ "bitxor",	2			} },
	{ OpCode::SHIFTL,	OpCodeInfo{ "shiftl",	2			} },
	{ OpCode::SHIFTR,	OpCodeInfo{ "shiftr",	2			} },

	{ OpCode::LT,		OpCodeInfo{ "lt",		2			} },
	{ OpCode::LTE,		OpCodeInfo{ "lte",		2			} },
	{ OpCode::EQU,		OpCodeInfo{ "equ",		2			} },
	{ OpCode::GTE,		OpCodeInfo{ "gte",		2			} },
	{ OpCode::GT,		OpCodeInfo{ "gt",		2			} },
	{ OpCode::NEQ,		OpCodeInfo{ "neq",		2			} },

	{ OpCode::OR,		OpCodeInfo{ "or",		2			} },
	{ OpCode::AND,		OpCodeInfo{ "and",		2			} },
	{ OpCode::NOT,		OpCodeInfo{ "not",		2			} },

	// Push/pop

	{ OpCode::POP,		OpCodeInfo{ "pop",		1			} },
	{ OpCode::PUSH,		OpCodeInfo{ "push",		1			} },
	{ OpCode::PUSHVAR,	OpCodeInfo{ "pushvar",	1			} },
	{ OpCode::EVAL,		OpCodeInfo{ "eval",		1			} },
	{ OpCode::ASSIGN,	OpCodeInfo{ "assign",	2			} },
	{ OpCode::COPY,		OpCodeInfo{	"copy",		2			} },

	// Call/return/jump...

	{ OpCode::CALL,		OpCodeInfo{ "call",		2			} },
	{ OpCode::CALLI,	OpCodeInfo{ "calli",	0			} },

	{ OpCode::ENTER,	OpCodeInfo{ "enter",	0			} },	// Size isn't staticly know
	{ OpCode::RET,		OpCodeInfo{ "ret",		FrameSize	} },
	{ OpCode::RETF,		OpCodeInfo{ "retf",		FrameSize	} }, 	// Pops frame, push return value

	{ OpCode::JUMP,		OpCodeInfo{ "jump",		2			} },
	{ OpCode::JUMPI,	OpCodeInfo{ "jumpi",	0			} },
	{ OpCode::JNEQ,		OpCodeInfo{ "jneq",		1			} },
	{ OpCode::JNEQI,	OpCodeInfo{ "jneqi",	0			} },

	{ OpCode::LLIMIT,	OpCodeInfo{ "llimit",	1			} },
	{ OpCode::ULIMIT,	OpCodeInfo{ "ulimit",	1			} },

	{ OpCode::HALT,		OpCodeInfo{ "halt",		0			} }
};

// public static

/********************************************************************************************//**
 * @note Returns { "nnknown", 0 } if op is an illegal opcode
 * @param op	The OpCode to look u
 * @return		OpCodeInfo for op
 ************************************************************************************************/
const OpCodeInfo& OpCodeInfo::info(OpCode op) {
	auto i = opInfoTbl.find(op);
	if ((opInfoTbl.end() != i))
		return i->second;

	else {
		static const OpCodeInfo unknown("unknown", 0);
		return unknown;
	}
}

/********************************************************************************************//**
 * @param	out		Where to write the results
 * @param	loc		Address of the instruction
 * @param	instr	The instruction to disassemble
 * @param	label	Display label
 * @return 	loc+1
 ************************************************************************************************/
unsigned disasm(ostream& out, unsigned loc, const Instr& instr, const string label) {
	out << fixed;

	const int level = instr.level;		// so we don't display level as a character

	if (label.size())
		out << label << ": ";

	out << setw(5) << loc << ": " << OpCodeInfo::info(instr.op).name();

	switch(instr.op) {
	case OpCode::ASSIGN:
	case OpCode::COPY:
	case OpCode::ENTER:
	case OpCode::EVAL:
	case OpCode::GET:
	case OpCode::LLIMIT:
	case OpCode::ULIMIT:
	case OpCode::POP:
	case OpCode::PRED:
	case OpCode::PUSH:
	case OpCode::SUCC:
	case OpCode::JUMPI:
	case OpCode::JNEQI:
	case OpCode::RET:
	case OpCode::RETF:
		out << " " << instr.value;
		break;

	case OpCode::PUSHVAR:
	case OpCode::CALLI:
		out << " "	<< level << ", " << instr.value;
		break;

	default:								// The rest don't use level, address or value
		break;
	}
	out << "\n";

	return loc+1;
}

