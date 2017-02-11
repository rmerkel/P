/** @file pl0int.h
 *
 *	PL/0 interpreter in C++
 *
 *	Ported from p0com.p, from Algorithms + Data Structions = Programes. Changes include
 *	- index the stack (stack[0..maxstack-1]); thus the initial values for the t and b registers are
 *    -1 and 0.
 *  - replaced single letter variables, e.g., p is now pc.
 *
 *	Currently loads and runs a sample/test program, dumping machine state before each instruction
 *	fetch.
 */

#ifndef	PL0INT_H
#define PL0INT_H

#include <cstdint>
#include <vector>

/// A pl/0 machine...
class pl0 {
public:
	/// OpCode - A machine operation code
	enum OpCode : uint8_t {
		Const,									///< Push a constant literal on the stack
		Push,									///< Read a variable off of the stack, and push it
		Pop,									///< Write the value from tos on the stack, and pop
		Call,									///< Call a procedure, setting up a mark block (frame)
		Enter,									///< InCremenT? Allocate locals, skip over mark block (frame)
		Jump,									///< Jump to a location 
		JNE,									///< Jump if tos == 0, pop
    	Ret,									///< Return (from procedure)
		Neg,									///< Unary negation 
    	Add,									///< Addition
    	Sub,									///< Subtraction
    	Mul,									///< Multiplication
    	Div,									///< Divison
		Odd,									///< Unary "is TOS odd?"
    	Equ,									///< Is equal?
    	Neq,									///< Isn't equal?
    	LT,										///< Less than?
    	LTE,									///< Less then or equal?
    	GT,										///< Greater than?
    	GTE										///< Greater than or equal?
	};

	typedef int64_t		Word;					///< A machine data word or address

	/// A machine Instruction
	struct Instr {
		struct {
			Word		addr;					///< Address or data value
			OpCode		op;						///< Operation code
			uint8_t		level;					///< level: 0..255, or encouded OpCode
		};

		/// Default construction; results in Const 0 0
		Instr() : addr{0}, op{Const}, level{0}	{}

		/// Construct an instruction from it's components
		Instr(OpCode _op, uint8_t _level = 0, Word _addr = 0) : addr{_addr}, op{_op}, level{_level} {}
	};

	pl0(bool v = false, size_t stacksz = 512);
	size_t operator()(const Instr* begin, const Instr* end);
	void reset();

private:
	/// A vector of Instrucitons
	typedef std::vector<Instr>	InstrVector;

	/// A vector of Words
	typedef std::vector<Word>	WordVector;

	/// A Effective Address that maybe invalidated
	class EAddr {
		size_t	eaddr;							///< The effective address
		bool	val;							///< Is eaddr valid?

	public:
		EAddr() : eaddr{0}, val{false} {}
		operator size_t() const			{	return eaddr;	}
		EAddr& operator=(size_t n) {
			eaddr = n;
			val = true;
			return *this;
		}
		bool valid() const				{	return val;		}
		void invalidate() 				{	val = false;	}
	};

	InstrVector	code;							///< Code segment, indexed by pc
	WordVector	stack;							///< Data segment (the stack), index by bp and sp
	size_t		pc;								///< Program counter register; index of *next* instruction in code[]
	int			bp;								///< Base pointer register; index of the current mark block/frame in stack[]
	int			sp;								///< Top of stack register (stack[sp])
	Instr		ir;								///< *Current* instruction register (code[pc-1])

	EAddr		lastWrite;						///< Last write effective address (to stack[]), if valid
	bool		verbose;						///< Verbose output if true

	void disasm();
	void dump();

protected:
	uint16_t base(uint16_t lvl);
	size_t run();
};

#endif
