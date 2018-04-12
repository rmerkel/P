/********************************************************************************************//**
 * @file interp.cc
 *
 * The P machine; a P language interpreter in C++
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include "interp.h"

using namespace std;
using namespace std::rel_ops;

// private:

/********************************************************************************************//**
 * Dump the current machine state
 ************************************************************************************************/
void PInterp::dump() {
	if (trace && lastWrite.valid())	// dump the last write...
		cout << "    "
			 << setw(5)	<< lastWrite << ": "
			 << setw(10) << stack[lastWrite]
			 << std::endl;
	lastWrite.invalidate();

	if (!trace) return;

	static vector<string> labels = {
		"(base)",
		"(saved fp)",
		"(raddr)",
		"(rvalue)"
	};
	auto it = labels.begin();

	// Dump the current  activation frame, followed by locals and temps...

	assert(sp >= fp);
	cout    << "fp: " 	<< setw(5)	<< fp << ": "
			<< right 	<< setw(10)	<< stack[fp];

	if (it != labels.end())
		cout << ' ' << *it++;
	cout << endl;

	for (auto bl = fp+1; bl < sp; ++bl) {
		cout
			<<	"    "	<< setw(5)	<< bl << ": "
			<< right	<< setw(10) << stack[bl];

		if (it != labels.end())
			cout << ' ' << *it++;
		cout << endl;
	}

	cout    << "sp: " 	<< setw(5) 	<< sp << ": "
			<< right	<< setw(10) << stack[sp];

	if (it != labels.end())
		cout << ' ' << *it++;
	cout << endl;

	disasm(cout, pc, code[pc], "pc");

	cout << endl;
}

/********************************************************************************************//**
 * Checks to see if the memory range described by [begin, end] is valid, i.e., either within the
 * data stack, or in the heap.
 *
 * @param	begin	Start of the memory range
 * @param 	end		One past the end of the memory range
 * @return	true if [begin,end) describes a valid memory range
 ************************************************************************************************/
bool PInterp::rangeCheck(size_t begin, size_t end) {
	assert (begin <= end);

	const size_t stackEnd = sp + 1;			// one past the top-of-stack
	const size_t heapBegin = heap.addr();
	const size_t heapEnd = heapBegin + heap.size();

	if (begin < stackEnd && end <= stackEnd)
		return true;						// Range is in the stack
	else if (begin >= heapBegin && begin < heapEnd && end >= heapBegin && end <= heapEnd)
		return true;						// Range is in the heap
	else
		return false;
}

// protected:

/********************************************************************************************//**
 * @param nlevel Number of levels down
 * @return The base, nlevel's down the stack
 ************************************************************************************************/
size_t PInterp::base(size_t nlevel) {
	auto b = fp;
	for (; nlevel > 0; --nlevel)
		b = stack[b].natural();

	return b;
}

/********************************************************************************************//**
 * @return the top-of-stack
 ************************************************************************************************/
Datum& PInterp::tos() 						{	return stack[sp];	}

/********************************************************************************************//**
 * @return the top-of-stack
 ************************************************************************************************/
const Datum& PInterp::tos() const			{	return stack[sp];	}

/********************************************************************************************//**
 * @throws	Result::stackUnderflow if the stack underflows
 * @return the top-of-stack
 ************************************************************************************************/
Datum PInterp::pop() {
	if (sp == 0)
		throw Result::stackUnderflow;
	else
		return stack[sp--];
}

/********************************************************************************************//**
 * @throws	Result::stackUnderflow if the stack underflows
 * @param n	number of datums to pop off the stack
 ************************************************************************************************/
void PInterp::pop(size_t n)	{
	if (sp < n)
		throw Result::stackUnderflow;
	else
		sp -= n;
}

/********************************************************************************************//**
 * Writes one expresson on the standard output stream. Index identifies a 4-tuple on the stack;
 * v:n:w:p the value to write, the number Datum's in the value, the width of the field to write
 * the value in, and the precision of the value. Note that if v isn't a Real, p is ignored.
 *
 * @note caller must consume the tuple-expression off of the stack.
 *
 * @param index	index into stack[] for the expr, width [, precision] tuple to print
 * @return false if width or precision is negative, true otherwise
 ************************************************************************************************/
Result PInterp::write1(unsigned index) {
	if (sp < 3)
		return Result::stackUnderflow;

	const Datum nValue = stack[index+1];
	const Datum width = stack[index+2];
	const Datum prec = stack[index+3];

	size_t n = 1;							// default value of nValue
	int w = 0;								// default value of width
	int p = 0;								// defualt value of prec

	Result r = Result::success;

	if (nValue.kind() != Datum::Integer) {
		cerr << "WRITE[LN] value count paramters is not an integer!" << endl;
		r =  Result::badDataType;

	} else if (nValue.integer() < 0) {
		cerr << "WRITE[LN] value count paramters is negative!" << endl;
		r =  Result::badDataType;

	} else
		n = nValue.integer();

	if (width.kind() == Datum::Integer)
		w = width.integer();
	
	else {
		cerr << "WRITE[LN] width parameter is not an integer!" << endl;
		r = Result::badDataType;
	}
	
	if (prec.kind() == Datum::Integer)
		p = prec.integer();
	
	else {
		cerr << "WRITE[LN] precision parameter is not an integer!" << endl;
		r =  Result::badDataType;
	}

	for (unsigned i = 0; i < n; ++i) {
		const Datum value = stack[index-n+1+i];

		switch(value.kind()) {
		case Datum::Boolean:	cout << boolalpha << value.boolean();					break;
		case Datum::Character:	cout << setw(w) << value.character();					break;
		case Datum::Integer:	cout << setw(w) << setprecision(p) << value.integer();	break;
		case Datum::Real:
			if (p == 0)
				cout << setw(w) << scientific << setprecision(6) << value.real();
			else
				cout << setw(w) << fixed << setprecision(p) << value.real();
			break;

		default:
			cerr << "unknown datum type: " << static_cast<unsigned>(value.kind()) << endl;
			r = Result::badDataType;
		}
	}

	return r;
}

/************************************************************************************************
 * Instructions...
 ************************************************************************************************/

/********************************************************************************************//**
 * @note	indexed by OpCode
 ************************************************************************************************/
PInterp::InstrPtr PInterp::instrTbl[] = {
	&PInterp::NEG,
	&PInterp::ITOR,
	&PInterp::ITOR2,
	&PInterp::ROUND,
	&PInterp::TRUNC,
	&PInterp::ABS,
	&PInterp::ATAN,
	&PInterp::EXP,
	&PInterp::LOG,
	&PInterp::DUP,
	&PInterp::ODD,
	&PInterp::PRED,
	&PInterp::SUCC,
	&PInterp::SIN,
	&PInterp::SQR,
	&PInterp::SQRT,
	&PInterp::WRITE,
	&PInterp::WRITELN,
	&PInterp::NEW,
	&PInterp::DISPOSE,
	&PInterp::ADD,
	&PInterp::SUB,
	&PInterp::MUL,
	&PInterp::DIV,
	&PInterp::REM,
	&PInterp::BNOT,
	&PInterp::BAND,
	&PInterp::BOR,
	&PInterp::BXOR,
	&PInterp::SHIFTL,
	&PInterp::SHIFTR,
	&PInterp::LT,
	&PInterp::LTE,
	&PInterp::EQU,
	&PInterp::GTE,
	&PInterp::GT,
	&PInterp::NEQ,
	&PInterp::OR,
	&PInterp::AND,
	&PInterp::NOT,
	&PInterp::POP,
	&PInterp::PUSH,
	&PInterp::PUSHVAR,
	&PInterp::EVAL,
	&PInterp::ASSIGN,
	&PInterp::COPY,
	&PInterp::CALL,
	&PInterp::CALLI,
	&PInterp::ENTER,
	&PInterp::RET,
	&PInterp::RETF,
	&PInterp::JUMP,
	&PInterp::JUMPI,
	&PInterp::JNEQ,
	&PInterp::JNEQI,
	&PInterp::LLIMIT,
	&PInterp::ULIMIT,
	&PInterp::HALT
};

/********************************************************************************************//**
 * Replace the Boolean value on the TOS with it's Boolean NOT.
 *
 * @note TOS value to "not"
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
Result PInterp::NOT() {
	Datum& TOS = tos();
	Result r = Result::success;

	if (TOS.kind() != Datum::Boolean)
		r = Result::badDataType;
	else
		TOS = !TOS.boolean();

	return r;
}

/********************************************************************************************//**
 * Convert the TOS interger value to real
 * @return	badDataType if TOS isn't an Integer
 ************************************************************************************************/
Result PInterp::ITOR() {
	Datum& TOS = tos();

	if (TOS.kind() == Datum::Integer) {
		TOS = TOS.integer() * 1.0;
		return Result::success;

	} else
		return Result::badDataType;
}

/********************************************************************************************//**
 * Convert the TOS-1 interger value to real, preserving the TOS value.
 * @return	badDataType if TOS isn't an Integer
 ************************************************************************************************/
Result PInterp::ITOR2() {
	Datum savedTOS = pop();
	Result r = ITOR();
	push(savedTOS);
	return r;
}

/********************************************************************************************//**
 * Round the TOS real value to the nearest integer
 * @return	badDataType if TOS isn't a Real
 ************************************************************************************************/
Result PInterp::ROUND() {
	Datum& TOS = tos();

	if (TOS.kind() == Datum::Real) {
		TOS = static_cast<int>(round(TOS.real()));
		return Result::success;

	} else
		return Result::badDataType;
}

/********************************************************************************************//**
 * Truncate the TOS value to a integer
 * @return	badDataType if TOS isn't a real
 ************************************************************************************************/
Result PInterp::TRUNC() {
	Datum& TOS = tos();

	if (TOS.kind() == Datum::Real) {
		TOS = static_cast<int>(TOS.real());
		return Result::success;

	} else
		return Result::badDataType;
 }

/********************************************************************************************//**
 * Replace the TOS value with its absolute value
 * @return	badDataType if TOS isn't a numeric type
 ************************************************************************************************/
Result PInterp::ABS() {
	Datum& TOS = tos();
	Result r = Result::success;

	if (TOS.kind() == Datum::Integer)
		TOS = abs(TOS.integer());
	else if (TOS.kind() == Datum::Real)
		TOS = fabs(TOS.real());
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * Replace the TOS value with its arc tangent
 * @return	badDagtaType if TOS isn't a numeric type
 ************************************************************************************************/
Result PInterp::ATAN() {
	Datum& TOS = tos();
	Result r = Result::success;

	if (TOS.kind() == Datum::Integer)
		TOS = atan(TOS.integer());
	else if (TOS.kind() == Datum::Real)
		TOS = atan(TOS.real());
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * Replace the TOS value with its base-e exponential
 * @return	badDataType if TOS isn't a numeric type.
 ************************************************************************************************/
Result PInterp::EXP() {
	Datum& TOS = tos();
	Result r = Result::success;

	if (TOS.kind() == Datum::Integer)
		TOS = exp(TOS.integer());
	else if (TOS.kind() == Datum::Real)
		TOS = exp(TOS.real());
	else
		return Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * Replace the TOS value with its natural logarithm
 * @return	badDataType if TOS isn't a numeric type, dividyByZero if zero.
 ************************************************************************************************/
Result PInterp::LOG() {
	Datum& TOS = tos();
	Result r = Result::success;

	if (!TOS.numeric())
		r = Result::badDataType;

	else if (TOS.kind() == Datum::Real) {
		if (TOS.real() == 0.0) {
			cerr << "Attempt to take log(0.0) @ pc (" << prevPc << ")!\n";
			r = Result::divideByZero;

		} else 
			TOS = log(TOS.real());

	} else if (TOS.kind() == Datum::Integer) {
		if (TOS.kind() == Datum::Integer && TOS.integer() == 0) {
			cerr << "Attempt to take log(0) @ pc (" << prevPc << ")!\n";
			r = Result::divideByZero;

		} else 
			TOS = log(TOS.integer());
	}

	return r;
}

/********************************************************************************************//**
 * @return	success
 ************************************************************************************************/
Result PInterp::DUP() {
	Datum& TOS = tos();
	push(TOS);
	return Result::success;
}

/********************************************************************************************//**
 * Replace the TOS integer value with true, if the value was an odd value, false otherwise.
 * @return	badDataType if TOS isn't an Integer.
 ************************************************************************************************/
Result PInterp::ODD() {
	Datum& TOS = tos();

	if (TOS.kind() == Datum::Integer) {
		TOS = (TOS.integer() & 1) ? true : false;
		return Result::success;

	} else
		return Result::badDataType;
}

/********************************************************************************************//**
 * Replace the TOS integer value with its predicesor value
 * @return	badDataType if TOS isn't a numeric type, outOfRange if the operation would exceed
 *			the types range. 
 ************************************************************************************************/
Result PInterp::PRED() {
	Datum& TOS = tos();
	Result r = Result::success;

	if (!TOS.numeric())
		r = Result::badDataType;

	else if (TOS <= ir.value)
		r =  Result::outOfRange;
	--TOS;

	return r;
}

/********************************************************************************************//**
 * Replace the numeric TOS value with it's sine
 * @return	badDataType if TOS isn't a numeric type.
 ************************************************************************************************/
Result PInterp::SIN() {
	Datum& TOS = tos();
	Result r = Result::success;

	if (TOS.kind() == Datum::Integer)
		TOS = sin(TOS.integer());
	else if (TOS.kind() == Datum::Real)
		TOS = sin(stack[sp].real());
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * Replace the numeric TOS value with it's square
 * @return	badDataType if TOS isn't a numeric type.
 ************************************************************************************************/
Result PInterp::SQR() {
	Datum& TOS = tos();
	Result r = Result::success;

	// note that Datum lacks a *= operator
	if (TOS.kind() == Datum::Integer)
		TOS = TOS.integer() * TOS.integer();
	else if (TOS.kind() == Datum::Real)
		TOS = TOS.real() * TOS.real();
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * Replace the numeric TOS value with its square-root.
 * @return	badDataType if TOS isn't a numeric type.
 ************************************************************************************************/
Result PInterp::SQRT() {
	Datum& TOS = tos();
	Result r = Result::success;

	if (TOS.kind() == Datum::Integer)
		TOS = sqrt(TOS.integer());
	else if (TOS.kind() == Datum::Real)
		TOS = sqrt(TOS.real());
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * Replace the integer TOS valeu with it's successor value.
 * @return	outOfRange if the operation would exceed the types range. 
 ************************************************************************************************/
Result PInterp::SUCC() {
	Datum& TOS = tos();
	Result r = Result::success;

	if (TOS >= ir.value)
		r =  Result::outOfRange;
	++TOS;

	return r;
}

/********************************************************************************************//**
 * Writes zero or more elements on the standard output stream. Each element is described by a
 * 3-tuple in the form v:w:p for Value, field Width and Precision if value is a Real. TOS is the
 * number of 3-tuple elements preceeding it, with the last 3-tuple precieeding the count.
 *
 * @note	Maybe add an element count to each tuple ; v:w:p:n where n is the number of elements
 *			in v, i.e., 1 for scalers, or the number of elements in an array.
 *
 * @return	BadDataType if TOS isn't an integer, stackUnderflow if the would underflow.
 ************************************************************************************************/
Result PInterp::WRITE() {
	const	unsigned	tupleSz = 4;	// Each parameter is a 4-tuple
			Result		r = Result::success;
			size_t		nargs = 0;		// # of tuples to process...
			Datum& 		TOS = tos();

	if (TOS.kind() != Datum::Integer) {
		cerr << "WRITE TOS is not an integer!" << endl;
		r =  Result::badDataType;

	} else {
		nargs = TOS.integer();
		if (nargs > sp) {
			cerr << "Stack underflow @ pc " << pc << endl;
			r = Result::stackUnderflow; 

		} else if (nargs > 0) {
			// Write each value in each 3-tuble on standard output, seperated by spaces
			for (unsigned i = 0; i < nargs-1 && r == Result::success; ++i) {
				r = write1(sp - (nargs * tupleSz) + (i * tupleSz));
				cout << ' ';
			}
			r = write1(sp - tupleSz);
		}
	}

	pop((nargs * tupleSz) + 1);			// Consume nargs plus nargs * tupleSz tubles

	return r;
}

/********************************************************************************************//**
 * Evaluates WRITE() and then writes a newline on the stardard output stream.
 * @return	BadDataType if TOS isn't an integer, stackUnderflow if the would underflow.
 ************************************************************************************************/
Result PInterp::WRITELN() {
	auto r = WRITE();
	cout << '\n';
	return r;
}

/********************************************************************************************//**
 * Replaces the TOS, which is the number of Datums to allocate on the heap, and if successful,
 * replaces the TOS with the address of the new block, or zero if there was insufficient space
 * in the heap.
 *
 * @return	badDataType if TOS isn't an Integer.
 ************************************************************************************************/
Result PInterp::NEW() {
	Datum& TOS = tos();

	if (TOS.kind() != Datum::Integer) {
		cerr << "NEW TOS is not an integer!" << endl;
		return Result::badDataType;
	}

	push(heap.alloc(pop().natural()));
	if (trace)
		heap.dump(cout);					// Dump the new heap state...

	return Result::success;
}

/********************************************************************************************//**
 * Return a block of data, allocated by NEW, back on the heap
 * @return	badDataType if the TOS isn't an Integer.
 ************************************************************************************************/
Result PInterp::DISPOSE() {
	Datum& TOS = tos();

	if (TOS.kind() != Datum::Integer) {
		cerr << "DISPOSE TOS is not an integer!" << endl;
		return Result::badDataType;

	} else if (TOS.integer() < 0) {
		cerr << "DISPOSE TOS is negative!" << endl;
		return Result::badDataType;
	}

	const size_t addr = TOS.natural(); pop();
	if (!heap.free(addr)) {
		cerr << "Dispose of " << addr << " failed!\n";
		return Result::freeStoreError;
	}

	if (trace)
		heap.dump(cout);					// Dump the new heap state...

	return Result::success;
}

/********************************************************************************************//**
 * Replace the numberic TOS value with it's negative
 * @return	badDataType if TOS isn't numeric.
 ************************************************************************************************/
Result PInterp::NEG() {
	Datum& TOS = tos();

	if (TOS.numeric()) {
		TOS = -TOS;
		return Result::success;

	} else
		return Result::badDataType;
}

/********************************************************************************************//**
 * Replace the top two numeric values on the stack with their sum
 * @return	stackUndeflow if the stack underflowed, badDataType if either operand isn't numeric.
 ************************************************************************************************/
Result PInterp::ADD() {
	const auto rhs = pop();
	const auto lhs = pop();

	if (lhs.numeric() && rhs.numeric()) {
		push(lhs + rhs);
		return Result::success;

	} else {
		push(0);
		return Result::badDataType;
	}
}

/********************************************************************************************//**
 * Replace the top two numeric values on the stack with their difference
 * @note TOS is the right-hand value, TOS-1 is the left-hand value.
 * @return	stackUndeflow if the stack underflowed, badDataType if either operand isn't numeric.
 ************************************************************************************************/
Result PInterp::SUB() {
	const auto rhs = pop();
	const auto lhs = pop();

	if (lhs.numeric() && rhs.numeric()) {
		push(lhs - rhs);
		return Result::success;

	} else {
		push(0);
		return Result::badDataType;
	}
}

/********************************************************************************************//**
 * Replace the top two numeric values on the stack with their product
 * @return	stackUndeflow if the stack underflowed, badDataType if either operand isn't numeric.
 ************************************************************************************************/
Result PInterp::MUL() {
	const auto rhs = pop();
	const auto lhs = pop();

	if (lhs.numeric() && rhs.numeric()) {
		push(lhs * rhs);
		return Result::success;

	} else {
		push(0);
		return Result::badDataType;
	}
}

/********************************************************************************************//**
 * Replace the top two numeric values on the stack with their quotient
 * @note TOS is the dividend, TOS-1 is the divisor.
 * @return	dividyByZero if divisor is zero, stackUndeflow if the stack underflowed, badDataType
 * 			if either operand isn't numeric.
 ************************************************************************************************/
Result PInterp::DIV() {
	const	Datum	rhs = pop();
	const	Datum	lhs = pop();
			Result	r	= Result::success;

	if (!lhs.numeric() || !rhs.numeric()) {
		cerr << "Attampt to divide with non-numbeic value\n";
		r =  Result::badDataType;

	} else if (rhs.kind() == Datum::Real && rhs.real() == 0.0) {
		cerr << "Attempt to divide by zero @ pc (" << prevPc << ")!\n";
		r= Result::divideByZero;

	} else if (rhs.kind() == Datum::Integer && rhs.integer() == 0) {
		cerr << "Attempt to divide by zero @ pc (" << prevPc << ")!\n";
		r = Result::divideByZero;

	} else
		push(lhs / rhs);

	if (r != Result::success)
		push(0);

	return r;
}

/********************************************************************************************//**
 * Replaces the top two integer values on the stack with thier reminder
 * @note TOS is the dividend, TOS-1 is the divisor.
 * @return	dividyByZero if divisor is zero, stackUndeflow if the stack underflowed, badDataType
 * 			if either operand isn't numeric.
 ************************************************************************************************/
Result PInterp::REM() {
	const	Datum	rhs = pop();
	const	Datum	lhs = pop();
			Result	r	= Result::success;

	if (lhs.kind() != Datum::Integer || rhs.kind() != Datum::Integer) {
		cerr << "Attampt to calulate reminder with non-integer value\n";
		r =  Result::badDataType;

	} else if (rhs.integer() == 0) {
		cerr << "Attempt to divide by zero @ pc (" << prevPc << ")!\n";
		r = Result::divideByZero;

	} else
		push(lhs % rhs);

	if (r != Result::success)
		push(0);

	return r;
}

/********************************************************************************************//**
 * Replace the top-of-stac with it's bitwise not
 * @return	stackUndeflow if the stack underflowed, badDataType if either operand isn't numeric.
 ************************************************************************************************/
Result PInterp::BNOT() {
	const auto rhs = pop();

	if (rhs.numeric()) {
		push(~rhs);
		return Result::success;

	} else {
		push(0);
		return Result::badDataType;
	}
}

/********************************************************************************************//**
 * Replace the top two values on the stack with their bitwise and
 * @return	stackUndeflow if the stack underflowed, badDataType if either operand isn't numeric.
 ************************************************************************************************/
Result PInterp::BAND() {
	const auto rhs = pop();
	const auto lhs = pop();

	if (lhs.numeric() && rhs.numeric()) {
		push(lhs & rhs);
		return Result::success;

	} else {
		push(0);
		return Result::badDataType;
	}
}

/********************************************************************************************//**
 * Replace the top two values on the stack with their bitwise or
 * @return	stackUndeflow if the stack underflowed, badDataType if either operand isn't numeric.
 ************************************************************************************************/
Result PInterp::BOR() {
	const auto rhs = pop();
	const auto lhs = pop();

	if (lhs.numeric() && rhs.numeric()) {
		push(lhs | rhs);
		return Result::success;

	} else {
		push(0);
		return Result::badDataType;
	}
}

/********************************************************************************************//**
 * Replace the top two values on the stack with their bitwise exclusive or
 * @return	stackUndeflow if the stack underflowed, badDataType if either operand isn't numeric.
 ************************************************************************************************/
Result PInterp::BXOR() {
	const auto rhs = pop();
	const auto lhs = pop();

	if (lhs.numeric() && rhs.numeric()) {
		push(lhs ^ rhs);
		return Result::success;

	} else {
		push(0);
		return Result::badDataType;
	}
}

/********************************************************************************************//**
 * Shift TOS left by TOS-1 bits, and replace both with the result
 * Replace the top two values on the stack with their bitwise exclusive or
 * @return	stackUndeflow if the stack underflowed, badDataType if either operand isn't numeric.
 ************************************************************************************************/
Result PInterp::SHIFTL() {
	const auto rhs = pop();
	const auto lhs = pop();

	if (lhs.numeric() && rhs.numeric()) {
		push(lhs << rhs);
		return Result::success;

	} else {
		push(0);
		return Result::badDataType;
	}
}
/********************************************************************************************//**
 * Shift TOS right by TOS-1 bits, and replace both with the result
 * Replace the top two values on the stack with their bitwise exclusive or
 * @return	stackUndeflow if the stack underflowed, badDataType if either operand isn't numeric.
 ************************************************************************************************/
Result PInterp::SHIFTR() {
	const auto rhs = pop();
	const auto lhs = pop();

	if (lhs.numeric() && rhs.numeric()) {
		push(lhs >> rhs);
		return Result::success;

	} else {
		push(0);
		return Result::badDataType;
	}
}

/********************************************************************************************//**
 * Replace the top two values with true if the left hand value is less than the right, false 
 * otherwise.
 *
 * @note TOS is the right hand value, TOS is the left hand value.
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
Result PInterp::LT() {
	const	Datum	rhs = pop();
	const	Datum	lhs = pop();
			Result	r	= Result::success;

	if (!rhs.numeric() || !lhs.numeric()) {
		cerr << "Non-numeric binary value\n";
		push(false);
		r = Result::badDataType;

	} else
		push(lhs < rhs ? true : false);
	
	return r;
}

/********************************************************************************************//**
 * Replace the top two values with true if the left hand value is less than, or equal to, the
 * right, false otherwise.
 *
 * @note TOS is the right hand value, TOS is the left hand value.
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
Result PInterp::LTE() {
	const	Datum	rhs = pop();
	const	Datum	lhs = pop();
			Result	r	= Result::success;

	if (!rhs.numeric() || !lhs.numeric()) {
		cerr << "Non-numeric binary value\n";
		push(false);
		r = Result::badDataType;

	} else
		push(lhs <= rhs ? true : false);
	
	return r;
}

/********************************************************************************************//**
 * Replace the top two values with true if the left hand value is equal to the right, false 
 * otherwise.
 *
 * @note TOS is the right hand value, TOS is the left hand value.
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
Result PInterp::EQU() {
	const	Datum	rhs = pop();
	const	Datum	lhs = pop();
			Result	r	= Result::success;

	if (!rhs.numeric() || !lhs.numeric()) {
		cerr << "Non-numeric binary value\n";
		push(false);
		r = Result::badDataType;

	} else
		push(Datum(lhs == rhs ? true : false));
	
	return r;
}

/********************************************************************************************//**
 * Replace the top two values with true if the left hand value is greater than, or equal to, the
 * right, false otherwise.
 *
 * @note TOS is the right hand value, TOS is the left hand value.
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
Result PInterp::GTE() {
	const	Datum	rhs = pop();
	const	Datum	lhs = pop();
			Result	r	= Result::success;

	if (!rhs.numeric() || !lhs.numeric()) {
		cerr << "Non-numeric binary value\n";
		push(false);
		r = Result::badDataType;

	} else
		push(lhs >= rhs ? true : false);
	
	return r;
}

/********************************************************************************************//**
 * Replace the top two values with true if the left hand value is greater than the right, false
 * otherwise.
 *
 * @note TOS is the right hand value, TOS is the left hand value.
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
Result PInterp::GT() {
	const	Datum	rhs = pop();
	const	Datum	lhs = pop();
			Result	r	= Result::success;

	if (!rhs.numeric() || !lhs.numeric()) {
		cerr << "Non-numeric binary value\n";
		push(false);
		r = Result::badDataType;

	} else
		push(Datum(lhs > rhs ? true : false));
	
	return r;
}

/********************************************************************************************//**
 * Replace the top two values with true if the left hand value does not equal the right, false
 * otherwise.
 *
 * @note TOS is the right hand value, TOS is the left hand value.
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
Result PInterp::NEQ() {
	const	Datum	rhs = pop();
	const	Datum	lhs = pop();
			Result	r	= Result::success;

	if (!rhs.numeric() || !lhs.numeric()) {
		cerr << "Non-numeric binary value\n";
		push(false);
		r = Result::badDataType;

	} else
		push(Datum(lhs != rhs ? true : false));
	
	return r;
}

/********************************************************************************************//**
 * Replace the top two Boolean values on the stack with their Boolean OR.
 *
 * @note TOS is the right hand value, TOS-1 is the left hand value.
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
Result PInterp::OR() {
	const	Datum	rhs = pop();
	const	Datum	lhs = pop();
			Result	r	= Result::success;

	if (lhs.kind() != Datum::Boolean || rhs.kind() != Datum::Boolean) {
		cerr << "Non-boolean binary value\n";
		push(false);
		r = Result::badDataType;

	} else
		push(Datum(lhs || rhs ? true : false));
	
	return r;
}

/********************************************************************************************//**
 * Replace the top two Boolean values on the stack with their Boolean AND.
 *
 * @note TOS is the right hand value, TOS-1 is the left hand value.
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
Result PInterp::AND() {
	const	Datum	rhs = pop();
	const	Datum	lhs = pop();
			Result	r	= Result::success;

	if (lhs.kind() != Datum::Boolean || rhs.kind() != Datum::Boolean) {
		cerr << "Non-boolean binary value\n";
		push(false);
		r = Result::badDataType;

	} else
		push(Datum(lhs && rhs ? true : false));
	
	return r;
}

/********************************************************************************************//**
 * Pops ir.value items off of the stack
 *
 * @return	stackUnderflow if the stack underflowed.
 ************************************************************************************************/
Result PInterp::POP() {
	if (ir.value < Datum(0))
		return Result::stackUnderflow;

	const size_t n = ir.value.natural();		// Accually, just an unsigned value
	if (sp < n)
		return Result::stackUnderflow;

	pop(n);

	return Result::success;
}

/********************************************************************************************//**
 * Push a constant value (ir.value) onto the stack.
 *
 * @return	success
 ************************************************************************************************/
Result PInterp::PUSH() {
	push(ir.value);
	return Result::success;
}

/********************************************************************************************//**
 * Push a variable reference (base(ir.level) + ir.value, onto the stack.
 *
 * @return	success
 ************************************************************************************************/
Result PInterp::PUSHVAR() {
	push(base(ir.level) + ir.value.integer());
	return Result::success;
}

/********************************************************************************************//**
 * Evaluates ir.value Datums, whose starting address is on the stop of the stack. Replaces the
 * address with the values.
 *
 * @return	stackUnderflow if the stack underflowed, 
 ************************************************************************************************/
Result PInterp::EVAL() {
	const	size_t	n = ir.value.natural();	// acually, an unsigned integer
			Result	r = Result::success;

	if (sp < n) {
		cerr << "Stack underflow evaluating " << n << " Datums!\n";
		r = Result::stackUnderflow;
	}

	size_t dst = pop().natural();
	if (!rangeCheck(dst, dst + n)) {
		cerr << "Stack underflow evaluating " << n << " Datums!\n";
		r = Result::stackUnderflow;
	}

	for (size_t i = 0; i < n; ++i)
		push(stack[dst++]);

	return r;
}

/********************************************************************************************//**
 * Copies N (ir.value) Datam values from the stack to the starting address that follows the
 * values. The values and destination address are consumed.
 *
 * Stack layout before;
 *
 * stack  | contents
 * ------ | -------------------------------
 * sp-N   | dst - start destination address
 * ------ | -------------------------------
 * sp-N-1 | value 1
 * ------ | -------------------------------
 * sp-N-2 | value 2
 * ------ | -------------------------------
 * ... 	  | ...
 * ------ | -------------------------------
 * sp+1   | value N-1
 * ------ | -------------------------------
 * sp     | value N
 *
 * Copies value 1..N to stack[dest, dest+N), then pops N Datums off of the stack.
 *
 * @return	stackUnderflow if the stack underflowed, 
 ************************************************************************************************/
Result PInterp::ASSIGN() {
	const	size_t	n = ir.value.natural();	// the # of values to assign

	if (sp < n)
		return Result::stackUnderflow;

	const size_t dst = stack[sp - n].natural();
	if (!rangeCheck(dst, dst + n))
		return Result::stackUnderflow;

	lastWrite = dst + n - 1;

	Datum* lhs = &stack[dst];
	Datum* rhs = &stack[sp - n + 1];
	for (size_t i = 0; i < n; i++)
		*lhs++ = *rhs++;
	pop(n+1);								// 'pop' the stack, including the dest addr

	return Result::success;
}

/********************************************************************************************//**
 * Copy N Datums, where N is ir.value, and the source starting address is TOS, and the destination 
 * starting address is TOS-1. The source and destination addresses are consumed.
 * @return	stackUnderflow if the stack underflowed.
 ************************************************************************************************/
Result PInterp::COPY() {
	const	size_t	n = ir.value.natural();	// Accually, just unsinged value
			Result	r = Result::success;

	size_t src  = pop().natural();
	if (!rangeCheck(src, src + n))
		r = Result::stackUnderflow;

	size_t dst = pop().natural();
	if (!rangeCheck(dst, dst + n))
		r = Result::stackUnderflow;

	lastWrite = dst + n;
	for (size_t i = 0; i < n; ++i)
		stack[dst++] = stack[src++];

	return r;
}

/********************************************************************************************//**
 * Call a subroutine whose level is TOS-1 and whose entry point is TOS
 * @return	success.
 ************************************************************************************************/
Result PInterp::CALL() {
	const	size_t	addr	= pop().natural();
	const	int8_t	nlevel	= pop().integer();
	const	size_t	oldFp	= fp;	// Save a copy before we modify it

	// Push a new activation frame block on the stack:

	push(base(nlevel));				//	FrameBase

	fp = sp;						// 	fp points to the start of the new frame

	push(oldFp);					//	FrameOldFp
	push(pc);						//	FrameRetAddr
	push(0ul);						//	FrameRetVal

	pc = addr;

	return Result::success;
}

/********************************************************************************************//**
 * Call a subroutine whose level is ir.level and whose entry point is ir.value.
 * @return	success.
 ************************************************************************************************/
Result PInterp::CALLI() {
	const	size_t	oldFp	= fp;	// Save a copy before we modify it

	// Push a new activation frame block on the stack:

	push(base(ir.level));			//	Framebase

	fp = sp;						// 	fp points to the start of the new frame

	push(oldFp);					//	FrameOldFp
	push(pc);						//	FrameRetAddr
	push(0ul);						//	FrameRetVal

	pc = ir.value.natural();

	return Result::success;
}

/********************************************************************************************//**
 * Unlinks the stack frame, setting the return address as the next instruciton.
 * @return	success.
 ************************************************************************************************/
Result PInterp::RET() {
	sp = fp - 1; 					// "pop" the activaction frame
	pc = stack[fp + FrameRetAddr].natural();
	fp = stack[fp + FrameOldFp].natural();
	sp -= ir.value.natural();		// Pop n parameters, if any...

	return Result::success;
}

/********************************************************************************************//**
 * Unlink the stack frame, set the return address, and then push the function result
 * @return	success.
 ************************************************************************************************/
Result PInterp::RETF() {
	// Save the function result, unlink the stack frame, return the result
	auto temp = stack[fp + FrameRetVal];
	RET();
	push(temp);

	return Result::success;
}

/********************************************************************************************//**
 * Allocates ir.value Datums for local variables on the stack.
 * @return	success.
 ************************************************************************************************/
Result PInterp::ENTER() {
	sp += ir.value.integer();

	return Result::success;
}

/********************************************************************************************//**
 * Jumps to TOS
 * @return	success.
 ************************************************************************************************/
Result PInterp::JUMP() {
	pc = pop().natural();
	return Result::success;
}

/********************************************************************************************//**
 * Jumps to ir.value
 * @return	success.
 ************************************************************************************************/
Result PInterp::JUMPI() {
	pc = ir.value.natural();
	return Result::success;
}

/********************************************************************************************//**
 * Jump to TOS if TOS-1 == false.
 * @return	badDataType if TOS isn't a Boolean.
 ************************************************************************************************/
Result PInterp::JNEQ() {
	const uint32_t addr = pop().natural();
	const Datum value = pop();

	if (value.kind() != Datum::Boolean)
		return Result::badDataType;

	if (value.boolean() == false)
		pc = addr;

	return Result::success;
}

/********************************************************************************************//**
 * Jump to TOS if TOS == false.
 * @return	badDataType if TOS isn't a Boolean.
 ************************************************************************************************/
Result PInterp::JNEQI() {
	const Datum value = pop();

	if (value.kind() != Datum::Boolean)
		return Result::badDataType;

	if (value.boolean() == false)
		pc = ir.value.natural();

	return Result::success;
}

/********************************************************************************************//**
 * @note	The TOS is not consumed.
 * @return	BadDataType if TOS isn't an Integer or a Boolean, outOfRange if the check failed.
 ************************************************************************************************/
Result PInterp::LLIMIT() {
	Datum& TOS = tos();
	if (!TOS.ordinal()) 
		return Result::badDataType;

	else if (TOS.kind() == Datum::Boolean) {
		const Datum i(TOS.boolean() ? 1 : 0);
		return i < ir.value ? Result::outOfRange : Result::success;

	} else if (TOS.kind() == Datum::Character) {
		const Datum i(static_cast<size_t>(TOS.character()));
		return i < ir.value ? Result::outOfRange : Result::success;

	} else 
		return TOS < ir.value ? Result::outOfRange : Result::success;
}

/********************************************************************************************//**
 * @note	The TOS is not consumed.
 * @return	BadDataType if TOS isn't an Integer or a Boolean, outOfRange if the check failed.
 ************************************************************************************************/
Result PInterp::ULIMIT() {
	Datum& TOS = tos();
	if (!TOS.ordinal()) 
		return Result::badDataType;

	else if (TOS.kind() == Datum::Boolean) {
		const Datum i(TOS.boolean() ? 1 : 0);
		return i > ir.value ? Result::outOfRange : Result::success;

	} else if (TOS.kind() == Datum::Character) {
		const Datum i(static_cast<size_t>(TOS.character()));
		return i > ir.value ? Result::outOfRange : Result::success;

	} else
		return TOS > ir.value ? Result::outOfRange : Result::success;
}

/********************************************************************************************//**
 * @return	halted
 ************************************************************************************************/
Result PInterp::HALT() {
	return Result::halted;
}

/********************************************************************************************//**
 * @return Result::success or...
 ************************************************************************************************/
Result PInterp::step() {
	Result r = Result::success;

	prevPc = pc++;							// Fetch the next instruction...
	ir = code[prevPc];
	++ncycles;

	if (sp < OpCodeInfo::info(ir.op).nElements()) {
		cerr << "Out of bounds stack access @ pc (" << prevPc << "), sp == " << sp << "!\n";
		r = Result::stackUnderflow;

	} else if (ir.op > OpCode::HALT) {
		cerr	<< "Unknown op-code: " << hex << "0x" << ordinal(ir.op)
				<< " found at pc (" << prevPc << ")!\n";
		r = Result::unknownInstr;

	} else
		r = (*this.*instrTbl[ordinal(ir.op)]) ();

	return r;
}

/********************************************************************************************//**
 *  @return	Result::success, or ...
 ************************************************************************************************/
Result PInterp::run() {
	if (trace) {
		cout << "Reg  Addr Value/Instr\n"
			 << "---------------------\n";
		heap.dump(cout);					// Dump the initial heap state...
	}

	Result status = Result::success;
	try {
		do {
			if (pc >= code.size()) {
				cerr << "pc (" << pc << ") is out of range: [0.." << code.size() << ")!\n";
				status = Result::badFetch;

			} else {
				dump();							// Dump state and disasm the next instruction
				status = step();
			}

		} while (Result::success == status);

	} catch (Result result) {
		cerr << result << " @pc " << prevPc << ", sp: " << sp << endl;
		status = result;
	}

	if (status != Result::success && status != Result::halted)
		cerr << "runtime error @pc " << prevPc << ", sp: " << sp << ": " << status << endl;

	return status;
}

//public

/********************************************************************************************//**
 * Initialize the machine into a reset state with trace == false. 
 *
 * @param stackSz	Size of the evaluation & call stack, in Datums.
 * @param fstoreSz	Size of the free store, in Datums.
 ************************************************************************************************/
PInterp::PInterp(unsigned stackSz, unsigned fstoreSz)
	:	stackSize{stackSz},
		stack(stackSize + fstoreSz, Datum(-1)),
		heap(stackSz, fstoreSz),
		trace(false),
		ncycles(0)
{
	reset();
}

/********************************************************************************************//**
 *	@param	prog	The program to run
 *	@param 	trce	True for trace/debugging messages
 * 
 *  @return	The number of machine cycles run
 ************************************************************************************************/
Result PInterp::operator()(const InstrVector& prog, bool trce) {
	trace = trce;
	code = prog;

	reset();

	auto result = run();
	if (Result::halted == result)
		result = Result::success;			// halted is normal!

	return result;
}

/********************************************************************************************//**
 ************************************************************************************************/
void PInterp::reset() {
	prevPc = pc = 0;

	fp = 0;											// Setup the initial activacation frame
	for (sp = fp; sp < FrameSize; ++sp)
		stack[sp] = 0;
	sp = fp + FrameSize - 1;

	ncycles = 0;
}

/********************************************************************************************//**
 * @return number of machien cycles run so far
 ************************************************************************************************/
size_t PInterp::cycles() const {
	return ncycles;
}

