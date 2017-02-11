/** @file pl0.h
 *
 *	PL/0 Machine Description
 *
 *	Ported from p0com.p, from Algorithms + Data Structions = Programes.
 */

#ifndef	PL0_H
#define PL0_H

#include <cstdint>
#include <string>
#include <vector>

/// Describe the PL0 machine
namespace pl0 {
	/// OpCode - A machine operation code
	enum class OpCode : std::uint8_t {
		pushConst,						///< Push a constant literal on the stack
		pushVar,						///< Read a variable off of the stack, and push it
		Pop,							///< Write the value from tos on the stack, and pop
		Call,							///< Call a procedure, setting up a mark block (frame)
		Enter,							///< InCremenT? Allocate locals, skip over mark block (frame)
		Jump,							///< Jump to a location 
		Jne,							///< Jump if tos == 0, pop
   		Return,							///< Return (from procedure)
		Neg,							///< Unary negation 
   		Add,							///< Binary Addition
   		Sub,							///< " Subtraction
   		Mul,							///< " Multiplication
   		Div,							///< " Divison
		Odd,							///< Unary "is TOS odd?"
   		Equ,							///< Is equal?
   		Neq,							///< Isn't equal?
   		LT,								///< Less than?
   		LTE,							///< Less then or equal?
   		GT,								///< Greater than?
   		GTE								///< Greater than or equal?
	};

	std::string String(OpCode op);

	typedef std::int64_t	Word;		///< A machine data word or address

	/// A vector of Words
	typedef std::vector<Word>	WordVector;

	/// A machine Instruction
	struct Instr {
		struct {
			Word		addr;			///< Address or data value
			OpCode		op;				///< Operation code
			uint8_t		level;			///< level: 0..255, or encouded OpCode
		};

		/// Default construction; results in pushConst 0 0
		Instr() : addr{0}, op{OpCode::pushConst}, level{0}	{}

		/// Construct an instruction from it's components
		Instr(OpCode o, uint8_t l = 0, Word a = 0) : addr{a}, op{o}, level{l} {}
	};

	/// A vector of Instrucitons
	typedef std::vector<Instr>	InstrVector;

	Word disasm(const std::string& label, Word loc, const Instr& instr);
}

#endif
