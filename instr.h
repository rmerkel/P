/********************************************************************************************//**
 * @file instr.h
 *
 * Pascal-lite machine operation codes, machine instruction format, activation frame format, and
 * associated utilities used by both the compiler (PasComp) and the interpreter (Interp).
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#ifndef	INSTR_H
#define INSTR_H

#include <map>
#include <string>
#include <vector>

#include "datum.h"

/********************************************************************************************//**
 * Activation Frame layout
 *
 * Word offsets from the start of a activaction frame, as created by OpCode::Call. Note that
 * procedures and function have the same sized frame.
 ************************************************************************************************/
enum Frame {
	FrameBase		= 0,				///< Offset to the Activation Frame base (base(n))
	FrameOldFp		= 1,				///< Offset to the saved frame pointer register
	FrameRetAddr	= 2,	  			///< Offset to the return address
	FrameRetVal		= 3,				///< Offset to the function return value

	FrameSize							///< Number of entries in an activaction frame (4)
};

/********************************************************************************************//**
 * Machine operation codes
 ************************************************************************************************/
enum class OpCode : unsigned char {
	NEG,								///< Negation; TOS = -TOS
	ITOR,								///< Convert integer to real; TOS = Real(TOS)
	ITOR2,								///< Convert integer to real; TOS-1 = Real(TOS-1)
	ROUND,								///< Round real TOS to integer; TOS = Round(TOS)
	TRUNC,								///< Truncate real TOS to integer; TOS = TRUNC(TOS)
	ABS,								///< Absolute value; TOS = rabs(TOS)
	ATAN,								///< Arc tangent; TOS = atan(TOS)
	EXP,								///< Exponent; TOS = exp(TOS)
	LOG,								///< Natural logarithm; TOS = log(TOS)

	DUP,								///< Duplicat; push(TOS)
	ODD,								///< Is odd? TOS = is TOS an odd number?
	PRED,								///< Predecessor; limit check, TOS--
	SUCC,								///< Successor; limit check; TOS++

	SIN,								///< Sine; TOS = sin(TOS)
	SQR,								///< Square; TOS = TOS * TOS
	SQRT,								///< Square-root; TOS = sqrt(TOS)

	WRITE,								///< Write expr-list on standard output
	WRITELN,							///< Write expr-list,  newline on standard output
	NEW,								///< Allocate dynamic store
	DISPOSE,							///< Dispose of allocated dynamic store

	ADD,								///< Addition
	SUB,								///< Subtraction
	MUL,								///< Multiplication
	DIV,								///< Division
	REM,								///< Remainder

	LT,									///< Less than
	LTE,								///< Less then or equal
	EQU,								///< Is equal to
	GTE,								///< Greater than or equal
	GT,									///< Greater than
	NEQU,								///< Does not equal

	LOR,								///< Logical or
	LAND,								///< Logical and
	LNOT,								///< Logical not
	
	POP,								///< Pop; pop TOS into the bit bucket
	PUSH,								///< Push a constant integer value
	PUSHVAR,							///< Push variable address (base(level) + addr)
	EVAL,								///< Evaluate variable TOS = address, replace with value
	ASSIGN,								///< Assign; TOS-1 = variable address, TOS = value

	CALL,								///< Call a procedure, pushing a new acrivation Frame
	ENTER,								///< Allocate locals on the stack
	RET,								///< Return from procedure; unlink Frame
	RETF,								///< Return from function; push result
	JUMP,								///< Jump to a location
	JNEQ,								///< Condition = pop(); Jump if condition == false (0)

	LLIMIT,								///< Check array index; out-of-range error if TOS <  addr
	ULIMIT,								///< Check array index; out-of-range error if TOS >  addr

	HALT = 255							///< Halt the machine
};

/********************************************************************************************//**
 * OpCode Information
 *
 * An OpCodes name string, and the number of stack elements it accesses
 ************************************************************************************************/
class OpCodeInfo {
	/// An OpCode to OpCodeInfo mapping
	typedef std::map<OpCode, OpCodeInfo>	InfoMap;

	static const InfoMap	opInfoTbl;	///< A table of OpCode information

	std::string		_name;				///< The OpCodes name, e.g., "add"
	unsigned		_nElements;			///< Number of stack elements used , e.g, 2

public:
	OpCodeInfo() : _nElements{0} {}		///< Construct an empty element

	/// Construct a OpCodeInfo from it's components
	OpCodeInfo(const std::string& name, unsigned nelements)
		: _name{name}, _nElements{nelements} {}

	virtual ~OpCodeInfo() {}			///< Destructor

	/// Return the OpCode name string
	const std::string name() const		{	return _name;   	}

	/// Return the number of stack elements the OpCode uses
	unsigned nElements() const			{	return _nElements;  }

	/// Return information about an OpCode
	static const OpCodeInfo& info(OpCode op);	///< Return information about op
};

/********************************************************************************************//**
 * An Instruction
 ************************************************************************************************/
struct Instr {
	Datum			addr;				///< A data value or memory address
	int8_t			level;				///< Base level: 0..255
	OpCode			op;					///< Operation code

	/// Default constructor; results in pushConst 0, 0...
	Instr() : level{0}, op{OpCode::HALT} {}

	/// Construct an instruction from it's components...
	Instr(OpCode o, int8_t l = 0, Datum d = 0) : addr{d}, level{l}, op{o} {}
};

/********************************************************************************************//**
 * A vector of Instr's (instructions)
 ************************************************************************************************/
typedef std::vector<Instr>					InstrVector;

/********************************************************************************************//**
 * Disassemble an instruction...
 ************************************************************************************************/
unsigned disasm(std::ostream& out, unsigned loc, const Instr& instr, const std::string label = "");

#endif

