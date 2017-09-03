/********************************************************************************************//**
 * @file interp.h
 *
 * The PL/0C interpreter.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ********************************************************************************************//**/

#ifndef	INTERP_H
#define INTERP_H

#include <cstdint>
#include <vector>

#include "instr.h"

/********************************************************************************************//**
 * A PL/0C Machine
 *
 * A interperter that started life as a straight C/C++ port of the PL/0 machine described in
 * Algorithms + Data Structures = Programs, 1st Edition, by Wirth.
 ********************************************************************************************//**/
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
	Result operator()(const InstrVector& program, bool v = false);
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

	InstrVector		code;					///< Code segment, indexed by pc
	DatumVector		stack;					///< Data segment (stack), indexed by fp and sp
	Unsigned		pc;						///< Program counter register; index of *next* instruction in code[]
	Unsigned		fp;						///< Frame pointer register; index of the current mark block/frame in stack[]
	Unsigned		sp;						///< Top of stack register (stack[sp])
	Instr			ir;						///< *Current* instruction register (code[pc-1])

	EAddr			lastWrite;				///< Last write effective address (to stack[]), if valid
	bool			verbose;				///< Verbose output if true
	unsigned  		ncycles;				///< Number of machine cycles run since the last reset

	void dump();

protected:
	///< Find the activation base 'lvl' levels up the stack...
	Unsigned base(Unsigned lvl);

	void mkStackSpace(Unsigned n);	///< Make room for more stack entries...

	Datum pop();							///< Pop a Datum from the top of stack...
	void push(Datum d);						///< Push a Datum onto the stack...

	/// Call a subroutine...
	void call(int8_t nlevel, Unsigned addr);
	void ret();								///< Return from procedure...
	void retf();							///< Return from a function...

	Result step();							///< Single step the machine...
	Result run();							///< Run the machine...
};

#endif
