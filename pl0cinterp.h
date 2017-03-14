/** @file pl0cinterp.h
 *
 * The PL/0C interpreter.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 */

#ifndef	PL0INT_H
#define PL0INT_H

#include <cstdint>
#include <vector>

#include "pl0c.h"

namespace pl0c {
	/** A PL/0C Machine
	 *
	 * A interperter that started life as a straight C/C++ port of the PL/0 machine described in Algorithms + Data Structures =
	 * Programs, 1st Edition, by Wirth, and then modified to provide "C like" instructions.
	 *

	 */
	class Interp {
	public:
		/// Interpeter results
		enum class Result {
			success,							///< No errors
			divideByZero,						///< Divide by zero
			badFetch,							///< Attempt to fetch uninitialized code
			unknownInstr,						///< Attempt to execute an undefined instruction
			stackOverflow,						///< Attempt to access beyound the end of the statck
			stackUnderflow,						///< Attempt to access an empty stack
			halted								///< Machine has halted
		};

		static std::string toString(Result r);	///< Return the results name

		Interp();
		virtual ~Interp() {}

		/// Load a applicaton and start the pl/0 machine running...
		Result operator()(const pl0c::InstrVector& program, bool v = false);
		void reset();							///< Reset the machine back to it's initial state.
		size_t cycles() const;					///< Return number of machine cycles run so far


	private:
		/// A Effective Address that maybe invalidated
		class EAddr {
			std::size_t	eaddr;					///< The effective address
			bool		val;					///< Is eaddr valid?

		public:
			EAddr() : eaddr{0}, val{false} {}	///< Construct an invalid address
			/// Return the effective address
			operator std::size_t() const		{	return eaddr;	}
			EAddr& operator=(std::size_t n) {	///< Set a valid address
				eaddr = n;
				val = true;
				return *this;
			}
			///< Return "is valid?"
			bool valid() const					{	return val;		}

			///< Invalidate the address
			void invalidate() 					{	val = false;	}
		};

		pl0c::InstrVector	code;				///< Code segment, indexed by pc
		pl0c::IntVector		stack;				///< Data segment (the stack), index by fp and sp
		pl0c::Unsigned		pc;					///< Program counter register; index of *next* instruction in code[]
		pl0c::Integer		fp;					///< Frame pointer register; index of the current mark block/frame in stack[]
		pl0c::Integer		sp;					///< Top of stack register (stack[sp])
		pl0c::Instr			ir;					///< *Current* instruction register (code[pc-1])

		EAddr				lastWrite;			///< Last write effective address (to stack[]), if valid
		bool				verbose;			///< Verbose output if true
		size_t 				ncycles;			///< Number of machine cycles run since the last reset

		void dump();

	protected:
		uint16_t base(uint16_t lvl);			///< Find the base 'lvl' levels (frames) up the stack...
		void mkStackSpace(size_t n);			///< Make room for at least sp+n entries on the stack

		Integer pop();							///< Pop an Integer from the top of stack
		Unsigned popUnsigned();					///< Pop an Unsigned from the top of stack
		void push(Integer i);					///< Push an Integer onto the stack
		void push(Unsigned u);					///< Push an Unsigned onto the stack

		void call(int8_t nlevel, Integer addr); ///< Call a subroutine...
		void ret();								///< Return from procedure...
		void reti();							///< Return from a function...

		Result step();							///< Single step the machine...
		Result run();							///< Run the machine...
	};
}

#endif
