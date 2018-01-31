/********************************************************************************************//**
 * @file pinterp.h
 *
 * The Pascal interpreter.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ********************************************************************************************//**/

#ifndef	PINTERP_H
#define PINTERP_H

#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "instr.h"
#include "freestore.h"

/********************************************************************************************//**
 * A Machine for the P languange
 *
 * A interperter that started life as a straight C/C++ port of the PL/0 machine described in
 * Algorithms + Data Structures = Programs, 1st Edition, by Wirth.
 *
 * @section Memory-map
 *
 * Code and data each exist in their own namespaces. The data namespace is divided into three
 * fixed length segments; the the constants segment, followed by the evaluation/call stack
 * (originized by call activation frames [blocks]), followed by the heap (free store). The size of
 *  each are set at construction, and reset by operator()();
 *
 * Address range            | Region    | Notes
 * ------------------------ | --------- | ------------------------------
 * stackSze..heap.size()-1  | Heap      | Maintained by heap(stackSz, fstoreSz)
 * 0..stackSize-1   		| Stack     | Evaluation and call stack
 ********************************************************************************************//**/
class PInterp {
public:
	/// Interpeter results
	enum Result {
		success,							///< No errors
		divideByZero,						///< Divide by zero
		badFetch,							///< Attempt to fetch uninitialized code
		unknownInstr,						///< Attempt to execute an undefined instruction
		stackOverflow,						///< Attempt to access beyound the end of the statck
		stackUnderflow,						///< Attempt to access an empty stack
		freeStoreError,						///< Allocation or free error
		outOfRange,							///< Attempt to index object with out-of-range index
		halted								///< Machine has halted
	};

	/// Runtime exception
	class Error : public std::runtime_error {
		Result	rcode;						///< The result code
	public:
		/// Construct with results code and description
		explicit Error(Result code, const std::string& what) : runtime_error(what), rcode(code) {}
		/// Return my results code
		Result result() const {	return rcode;	}
	};

	PInterp(unsigned stackSz = 1024, unsigned fstoreSz = 3*1024);
	virtual ~PInterp() {}

	/// Load a applicaton and start the pl/0 machine running...
	Result operator()(const InstrVector& prog, bool v = false);
	void reset();							///< Reset the machine back to it's initial state.
	size_t cycles() const;					///< Return number of machine cycles run so far

protected:
	/// Return true if the specified memory range is valid
	bool rangeCheck(size_t begin, size_t end);

	unsigned base(unsigned lvl);			///< Find the activation base 'lvl' levels up the stack...

	Datum pop();							///< Pop a Datum from the top of stack...
	void pop(unsigned n);					///< Pop and discard n Datums from the top of stack...
	void push(Datum d);						///< Push a Datum onto the stack...

	void write1(unsigned index);			///< Write one expression on standard output
	bool write();							///< Write expressions on standard output

	/// Call a subroutine...
	void call(int8_t nlevel, unsigned addr);
	void ret();								///< Return from procedure...
	void retf();							///< Return from a function...

	void assign(unsigned n);				///< Assign N Datums...
	void eval(unsigned n);					///< Evaluate N Datums...
	void copy(unsigned n);					///< Copy N Datums...

	Result step();							///< Single step the machine...
	Result run();							///< Run the machine...

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
	unsigned		stackSize;				///< The size of the stack segment, in Datums.
	DatumVector		stack;					///< Data segment (stack + free-store), indexed by fp and sp
	FreeStore		heap;					///< Dynamic memory heap
	unsigned		pc;						///< Program counter register; index of *next* instruction in code[]
	unsigned		fp;						///< Frame pointer register; index of the current mark block/frame in stack[]
	unsigned		sp;						///< Top of stack register (stack[sp])
	Instr			ir;						///< *Current* instruction register (code[pc-1])

	EAddr			lastWrite;				///< Last write effective address (to stack[]), if valid
	bool			verbose;				///< Verbose output if true
	unsigned  		ncycles;				///< Number of machine cycles run since the last reset

	void dump();
};

std::ostream& operator<<(std::ostream& os, const PInterp::Result& result);

#endif
