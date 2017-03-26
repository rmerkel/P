/** @file pl0c.h
 *
 * PL/0C machine definitions; operation codes, instruction and activation frame format, and
 * associated utilities used by both the compiler (pl0c::Comp) and the interpreter (pl0c::Interp).
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#ifndef	PL0C_H
#define PL0C_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "datum.h"

/// The PL/0C Namespace
namespace pl0c {
	/** Activation Frame layout
	 *
	 * Word offsets from the start of a activaction frame, as created by OpCode::call
	 */
	enum Frame {
		FrameBase		= 0,				///< Offset to the Activation Frame base (base(n))
		FrameOldFp		= 1,				///< Offset to the saved frame pointer register
		FrameRetAddr	= 2,	  			///< Offset to the return address
		FrameRetVal		= 3,				///< Offset to the function return value

		FrameSize							///< Number of entries in an activaction frame (4)
	};

	/// Operation codes; restricted to 256 operations, maximum
	enum class OpCode : std::uint8_t {
		noti, 								///< Unary integer not
		negi,								///< Unary integer negation
		comp,								///< Unary one's compliment

		addi,								///< Interger addition
		subi,								///< Interger subtraction
		muli,								///< Interger multiplication
		divi,								///< Interger division
		remi,								///< Integer remainder

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

		call,								///< Call a procedure, pushing a new acrivation Frame
		enter,								///< Allocate locals on the stack
		ret,								///< Return from procedure; unlink Frame
		reti,								///< Return from function; unlink Frame and push result
		jump,								///< Jump to a location
		jneq,								///< Condition = pop(); Jump if condition == false (0)

		halt = 255							///< Halt the machine
	};

	/** OpCode Information
	 *
	 * An OpCodes name string, and the number of stack elements it accesses
	 */
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

		/// Return the OpCode name string
		const std::string name() const		{	return _name;   	}

		/// Return the number of stack elements the OpCode uses
		unsigned nElements() const			{	return _nElements;  }

		/// Return information about an OpCode
		static const OpCodeInfo& info(OpCode op);	///< Return information about op
	};

	/// An Instruction
	struct Instr {
		Datum			addr;				///< A data value or memory address
		int8_t			level;				///< Base level: 0..255
		OpCode			op;					///< Operation code

		/// Default constructor; results in pushConst 0, 0...
		Instr() : addr{0}, level{0}, op{OpCode::pushConst}
			{}

		/// Construct an instruction from it's components...
		Instr(OpCode o, int8_t l = 0, Datum d = 0) : addr{d}, level{l}, op{o}
			{}
	};

	/// A vector of Instr's (instructions)
	typedef std::vector<Instr>					InstrVector;

	/// Disassemble an instruction...
	Unsigned disasm(std::ostream& out, Unsigned loc, const Instr& instr, const std::string label = "");
}

#endif
