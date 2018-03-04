/********************************************************************************************//**
 * @file interp.h
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
		badDataType,						///< Wrong data type for operation
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
	Result operator()(const InstrVector& prog, bool t = false);
	void reset();							///< Reset the machine back to it's initial state.
	size_t cycles() const;					///< Return number of machine cycles run so far

protected:
	/// A DatumVector iterator
	typedef	DatumVector::iterator DatumVecIter;

	/// Return true if the specified memory range is valid
	bool rangeCheck(size_t begin, size_t end);

	size_t base(size_t lvl);				///< Find the activation base 'lvl' levels up the stack...
	Datum& tos();							///< Return the top-of-stack
	const Datum& tos() const;				///< Return the top-of-stack
	Datum pop();							///< Pop a Datum from the top-of-stack...
	void pop(size_t n);						///< Pop and discard n Datums from the top of stack...

	/// Push value onto the stack
	template<class T> void push(const T& value);
	Result write1(unsigned index);			///< Write one expression on standard output...

	Result ITOR();							///< Convert to real
	Result ITOR2();							///< Convert to real
	Result ROUND();							///< Convert to integer by rounding 
	Result TRUNC();							///< Convert to interger by truncation
	Result ABS();							///< Absolute value
	Result ATAN();							///< Arc tangent
	Result EXP();							///< Base-e exponential 
	Result DUP(); 							///< Duplicate
	Result LOG(); 							///< Natural logarithm
	Result ODD();							///< Is an odd number?
	Result PRED();							///< Predicesor
	Result SIN();							///< Sine
	Result SQR();							///< Square
	Result SQRT();							///< Square-root
	Result SUCC();							///< Successor
	Result WRITE();							///< Write on standard output
	Result WRITELN();						///< Write on standard output
	Result NEW();							///< Allocate space
	Result DISPOSE();						///< Free space
	Result NEG();							///< Negative
	Result ADD();							///< Replace the top two values on the stack with their sum
	Result SUB();							///< Replace the top two vlaues on the stack with their difference
	Result MUL();							///< Replace the top two values on the stack with their product
	Result DIV();							///< Replace the top two values on the stack with their quotient
	Result REM();							///< Reminder
	Result LT();							///< Less than?
	Result LTE();							///< Less than, or equal?
	Result EQU();							///< Equal?
	Result GTE();							///< Greater than, or equal?
	Result GT();							///< Greater than?
	Result NEQ();							///< Not equal?
	Result OR();							///< Logical or
	Result AND();							///< Logical and
	Result NOT();							///< Logical not
	Result POP();							///< Pop datum(s) off the stack
	Result PUSH();							///< Push constant on the stack
	Result PUSHVAR();						///< Push variable offset on the stack
	Result EVAL();							///< Evaluate Datum(s)...
	Result ASSIGN();						///< Assign N Datums...
	Result COPY();							///< Copy N Datums...
	Result CALL(); 							///< Call a subroutine...
	Result CALLI();							///< Call a subroutine
	Result RET();							///< Return from procedure...
	Result RETF();							///< Return from a function...
	Result ENTER();							///< Enter sub-routine, allocate space for locals
	Result JUMP();							///< Jump
	Result JUMPI();							///< Jump
	Result JNEQ();							///< Jump if condition is false
	Result JNEQI();							///< Jump if condition is false
	Result LLIMIT();						///< Check lower limit
	Result ULIMIT();						///< Check upper limit
	Result HALT();							///< Stop the machine

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

	InstrVector	code;						///< Code segment, indexed by pc
	unsigned	stackSize;					///< The size of the stack segment, in Datums.
	DatumVector	stack;						///< Data segment (stack + free-store), indexed by fp and sp
	FreeStore	heap;						///< Dynamic memory heap
	size_t		pc;							///< Program counter register; index of *next* instruction in code[]
	size_t		prevPc;						///< Previous PC register; index of the *current* instruction in code
	size_t		fp;							///< Frame pointer register; index of the current mark block/frame in stack[]
	size_t		sp;							///< Top of stack register (stack[sp])
	Instr		ir;							///< *Current* instruction register (code[pc-1])
	EAddr		lastWrite;					///< Last write effective address (to stack[]), if valid
	bool		trace;						///< Trace run if true
	unsigned  	ncycles;					///< Number of machine cycles run since the last reset

	void dump();
};

/********************************************************************************************//**
 * @param value	Datum to push on to the stack
 ************************************************************************************************/
template <class T> void PInterp::push(const T& value) {
	if (sp >= heap.addr())
		throw Error(stackOverflow, "stack overflow error");
	else
		stack[++sp] = Datum(value);
}

/********************************************************************************************//**
 * PInterp::Result stream put operator
 ************************************************************************************************/
std::ostream& operator<<(std::ostream& os, const PInterp::Result& result);

#endif
