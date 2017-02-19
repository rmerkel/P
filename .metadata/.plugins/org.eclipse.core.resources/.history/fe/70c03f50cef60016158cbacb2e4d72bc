/** @file pl0c.h
 *  
 *  A description of the PL/0C Machine operation codes, instruction format and utilities used by
 *  both the compiler (PL0Comp) and the interpreter (PL0Interp).
 *
 *	Ported from p0com.p, the compiler/interpreter from Algorithms + Data Structures = Programs.
 */

#ifndef	PL0_H
#define PL0_H

#include <cstdint>
#include <string>
#include <vector>

/// The PL0 machine operation codes and instruction format.
namespace pl0c {
	/// Operation codes
	enum class OpCode : std::uint8_t {

		// Unary operations

		odd,									///< is top-of-stack odd?
		neg,									///< Negate the top-of-stack
												 
		// Binary operations

		add,									///< Addition
		sub,									///< Subtraction
		mul,									///< Multiplication
		div,									///< Division
		equ,									///< Is equal?
		neq,									///< Is not equal?
		lt,										///< Less than?
		lte,									///< Less then or equal?
		gt,										///< Greater than?
		gte,									///< Greater than or equal?
		
		// Push/pop

		pushConst,								///< Push a constant literal on the stack
		pushVar,								///< Read and push a variable on the stack
		pop,									///< Pop and write a variable off of the stack
												///
		call,									///< Call a procedure, setting up a activation block (frame)
		ret,									///< Return (from procedure)
		enter,									///< Allocate locals on the stack
		jump,									///< Jump to a location
		jneq									///< Jump if top-of-stack == false (0), pop
	};

	std::string toString(OpCode op);			///< Return the name of the OpCode as a string

	typedef std::int32_t		Word;			///< A data word or address
	typedef std::vector<Word>	WordVector;		///< A vector of Words

	/// A Instruction
	struct Instr {
		struct {
			Word		addr;					///< Address or data value
			OpCode		op;						///< Operation code
			int8_t		level;					///< level: 0..255
		};

		/// Default constructor; results in pushConst 0, 0
		Instr() : addr{0}, op{OpCode::pushConst}, level{0}	{}

		/// Construct an instruction from it's components
		Instr(OpCode o, int8_t l = 0, Word a = 0) : addr{a}, op{o}, level{l} {}
	};

	typedef std::vector<Instr>	InstrVector;	///< A vector of Instr's

	Word disasm(Word loc, const Instr& instr, const std::string label = "");
}

#endif
