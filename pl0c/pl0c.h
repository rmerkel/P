/** @file pl0c.h
 *  
 *  Definitions of the PL/0C machine operation codes, instruction format and utilities used by both
 *  the compiler (pl0c::Comp) and the interpreter (pl0c::Interp).
 * 
 *  @section	instructions	PL/0C Machine instructions
 * 
 *  Orignally ported from p0com.p, the compiler/interpreter from Algorithms + Data Structures =
 *  Programs, 1st Edition, by Wirth, modified to use one OpCode per operation, i.e., removed the
 *  "OPR" instruction, and then adding more "C like" instructions, e.g., bit and logical OR...
 *  Finally, converted to use the push variable (address), eval/assign type address from HOC, as
 *  described in The UNIX Programming Environment, by Kernighan and Pike.
 */ 

#ifndef	PL0C_H
#define PL0C_H

#include <cstdint>
#include <string>
#include <vector>

/// The PL/0C Namespace
namespace pl0c {
	/// Activation frame layout as created by OpCode::call
	enum Frame {
		base,								///< Frame base; base(n)
		oldfp,								///< Saved frame pointer register
		rAddr,								///< Return address
		rValue,								///< Function return value, defaults to zero.

		size								///< Number of elements in the frame
	};

	/// Operation codes; restricted to 256 operations, max
	enum class OpCode : std::uint8_t {
		Not, 								///< Unary not
		neg,								///< Unary negation
		comp,								///< Unary one's compliment

		add,								///< Addition
		sub,								///< Subtraction
		mul,								///< Multiplication
		div,								///< Division
		rem,								///< Remainder

		bor,								///< Bitwise inclusive or
		band,								///< Bitwise and
		bxor,								///< Bitwise eXclusive or

		lshift,								///< Left shift
		rshift,								///< Right shift
										
		lt,									///< Less than
		lte,								///< Less then or equal
		equ,								///< Equality
		gte,								///< Greater than or equal
		gt,									///< Greater than
		neq,								///< Inequality

		lor,								///< Logical or
		land,								///< logica and

		pushConst,							///< Push a constant value
		pushVar,							///< Push variable address (base(level) + addr)
		eval,								///< Evaluate variable TOS = address, replace with value
		assign,								///< Assign; TOS = variable address, TOS-1 = value

		call,								///< Call a procedure, create Frame
		enter,								///< Allocate locals on the stack
		ret,								///< Return from procedure; unlink Frame
		reti,								///< Return from function; unlink Frame and push result
		jump,								///< Jump to a location
		jneq								///< Condition = pop(); Jump if condition == false (0)
	};

	std::string toString(OpCode op);		///< Return the OpCode's name...

	typedef std::int32_t		Integer;	///< A data word or address
	typedef std::uint32_t		Unsigned;	///< Data word as an unsigned integer
	typedef std::vector<Integer> IntVector;	///< A vector of Integers...

	/// An Instruction
	struct Instr {
		struct {
			Integer		addr;				///< Address or data value
			OpCode		op;					///< Operation code
			int8_t		level;				///< level: 0..255
		};

		/// Default constructor; results in pushConst 0, 0...
		Instr() : addr{0}, op{OpCode::pushConst}, level{0}	{}

		/// Construct an instruction from it's components...
		Instr(OpCode o, int8_t l = 0, Integer a = 0) : addr{a}, op{o}, level{l} {}
	};

	/// A vector of Instr's
	typedef std::vector<Instr>	InstrVector;

	/// Disassemble an instruction...
	Integer disasm(std::ostream& out, Integer loc, const Instr& instr, const std::string label = "");
}

#endif
