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
	 * @section bugs	Bugs
	 * 
	 *  - The stack length is fixed at construction and can't grow.
	 *  - There is no out of range checking for the stack or code; references out-of-bounds leads to
	 *    disaster.
	 *  - There are no checks for divide by zero.
	 *  - There are no input/output instructions.
	 *  - There is no interactive mode for debugging; just automatic single stepping (verbose == true)
	 */
	class Interp {
	public:
		Interp(std::size_t stacksz = 512);
		virtual ~Interp() {}

		/// Load a applicaton and start the pl/0 machine running...
		std::size_t operator()(const pl0c::InstrVector& program, bool v = false);
		void reset();							///< Reset the machine back to it's initial state.

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
		pl0c::IntVector	stack;				///< Data segment (the stack), index by bp and sp
		std::size_t			pc;					///< Program counter register; index of *next* instruction in code[]
		int					bp;					///< Base pointer register; index of the current mark block/frame in stack[]
		int					sp;					///< Top of stack register (stack[sp])
		pl0c::Instr			ir;					///< *Current* instruction register (code[pc-1])

		EAddr				lastWrite;			///< Last write effective address (to stack[]), if valid
		bool				verbose;			///< Verbose output if true

		void dump();

	protected:
		uint16_t base(uint16_t lvl);			///< Find the base 'lvl' levels (frames) up the stack...
		void ret();								///< Return from subroutine...
		std::size_t run();						///< Run the machine...
	};
}

#endif
