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
 * Checks to see if any part of the specified memory range is invalid, i.e.,  between the top of
 * the stack and the start of the heap.
 *
 * @param	begin	Start of the memory range
 * @param 	end		One past the end of the memory range
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
 * @param lvl Number of levels down
 * @return The base, lvl's down the stack
 ************************************************************************************************/
size_t PInterp::base(size_t lvl) {
	auto b = fp;
	for (; lvl > 0; --lvl)
		b = stack[b].address();

	return b;
}

/********************************************************************************************//**
 * @return the top-of-stack
 ************************************************************************************************/
Datum PInterp::pop()	{
	if (sp == 0)
		throw Error(stackUnderflow, "stack underflow error");
	else
		return stack[sp--];
}

/********************************************************************************************//**
 * @param n	number of datums to pop off the stack
 ************************************************************************************************/
void PInterp::pop(size_t n)	{
	if (sp < n)
		throw Error(stackUnderflow, "stack underflow error");
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
PInterp::Result PInterp::write1(unsigned index) {
	if (sp < 3)
		return stackUnderflow;

	const Datum nValue = stack[index+1];
	const Datum width = stack[index+2];
	const Datum prec = stack[index+3];

	size_t n = 1;							// default value of nValue
	int w = 0;								// default value of width
	int p = 0;								// defualt value of prec

	Result r = Result::success;

	if (nValue.kind() == Datum::Address)
		n = nValue.address();

	else {
		cerr << "WRITE[LN] value count paramters is not an integer!" << endl;
		r =  Result::badDataType;
	}

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
		case Datum::Address:	cout << setw(w) << setprecision(p) << value.address();	break;
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
 * Convert the TOS interger value to real
 * @param	TOS	Interger value to convert
 * @return	badDataType if TOS isn't an Integer
 ************************************************************************************************/
PInterp::Result PInterp::ITOR(PInterp::DatumVecIter TOS) {
	if (TOS->kind() == Datum::Integer) {
		*TOS = TOS->integer() * 1.0;
		return Result::success;

	} else
		return Result::badDataType;
}

/********************************************************************************************//**
 * Convert the TOS-1 interger value to real, preserving the TOS value.
 * @param	TOS	Top-of-stack (unmodified).
 * @return	badDataType if TOS isn't an Integer
 ************************************************************************************************/
PInterp::Result PInterp::ITOR2(PInterp::DatumVecIter TOS) {
	return ITOR(--TOS);
}

/********************************************************************************************//**
 * Round the TOS real value to the nearest integer
 * @param	TOS	Real value to round
 * @return	badDataType if TOS isn't a Real
 ************************************************************************************************/
PInterp::Result PInterp::ROUND(PInterp::DatumVecIter TOS) {
	if (TOS->kind() == Datum::Real) {
		*TOS  = static_cast<int>(round(TOS->real()));
		return Result::success;

	} else
		return Result::badDataType;
}

/********************************************************************************************//**
 * Truncate the TOS value to a integer
 * @param	TOS	Real value to truncate
 * @return	badDataType if TOS isn't a real
 ************************************************************************************************/
PInterp::Result PInterp::TRUNC(PInterp::DatumVecIter TOS) {
	if (TOS->kind() == Datum::Real) {
		*TOS = static_cast<int>(TOS->real());
		return Result::success;

	} else
		return Result::badDataType;
 }

/********************************************************************************************//**
 * Replace the TOS value with its absolute value
 * @param	TOS	Signed numeric value to calculate absolute value of 
 * @return	badDataType if TOS isn't a numeric type
 ************************************************************************************************/
PInterp::Result PInterp::ABS(PInterp::DatumVecIter TOS) {
	Result r = Result::success;

	if (TOS->kind() == Datum::Integer)
		*TOS = abs(TOS->integer());
	else if (TOS->kind() == Datum::Real)
		*TOS = fabs(TOS->real());
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * Replace the TOS value with its arc tangent
 * @param	TOS	Numeric value to calculate arc tangent of
 * @return	badDagtaType if TOS isn't a numeric type
 ************************************************************************************************/
PInterp::Result PInterp::ATAN(PInterp::DatumVecIter TOS) {
	Result r = Result::success;

	if (TOS->kind() == Datum::Integer)
		*TOS = atan(TOS->integer());
	else if (TOS->kind() == Datum::Real)
		*TOS = atan(TOS->real());
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * Replace the TOS value with its base-e exponential
 * @param	TOS	Numeric value to calculate base-e exponential of
 * @return	badDataType if TOS isn't a numeric type.
 ************************************************************************************************/
PInterp::Result PInterp::EXP(PInterp::DatumVecIter TOS) {
	Result r = Result::success;

	if (TOS->kind() == Datum::Integer)
		*TOS = exp(TOS->integer());
	else if (TOS->kind() == Datum::Real)
		*TOS = exp(TOS->real());
	else
		return Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * @param	TOS	value to duplicate
 * @return	success
 ************************************************************************************************/
PInterp::Result PInterp::DUP(PInterp::DatumVecIter TOS) {
	push(*TOS);
	return Result::success;
}

/********************************************************************************************//**
 * Replace the TOS value with its natural logarithm
 * @param	TOS	Numeric value to caluculate natural logarithm of
 * @return	badDataType if TOS isn't a numeric type, dividyByZero if zero.
 ************************************************************************************************/
PInterp::Result PInterp::LOG(PInterp::DatumVecIter TOS) {
	Result r = Result::success;

	if (!TOS->numeric())
		r = Result::badDataType;

	else if (TOS->kind() == Datum::Real) {
		if (TOS->real() == 0.0) {
			cerr << "Attempt to take log(0.0) @ pc (" << prevPc << ")!\n";
			r = Result::divideByZero;

		} else 
			*TOS = log(TOS->real());

	} else if (TOS->kind() == Datum::Integer) {
		if (TOS->kind() == Datum::Integer && TOS->integer() == 0) {
			cerr << "Attempt to take log(0) @ pc (" << prevPc << ")!\n";
			r = Result::divideByZero;

		} else 
			*TOS = log(TOS->integer());
	}

	return r;
}

/********************************************************************************************//**
 * Replace the TOS integer value with true, if the value was an odd value, false otherwise.
 * @param	TOS	Integer value to test
 * @return	badDataType if TOS isn't an Integer.
 ************************************************************************************************/
PInterp::Result PInterp::ODD(PInterp::DatumVecIter TOS) {
	if (TOS->kind() == Datum::Integer) {
		*TOS = (TOS->integer() & 1) ? true : false;
		return Result::success;

	} else
		return Result::badDataType;
}

/********************************************************************************************//**
 * Replace the TOS integer value with its predicesor value
 * @param	TOS	Integer value to calculate predicesor of
 * @return	badDataType if TOS isn't a numeric type, outOfRange if the operation would exceed
 *			the types range. 
 ************************************************************************************************/
PInterp::Result PInterp::PRED(PInterp::DatumVecIter TOS) {
	Result r = Result::success;

	if (!TOS->numeric())
		r = Result::badDataType;

	else if (*TOS <= ir.value)
		r =  Result::outOfRange;

	--(*TOS);
	return r;
}

/********************************************************************************************//**
 * Replace the numeric TOS value with it's sine
 * @param	TOS	Value to calulate sine of
 * @return	badDataType if TOS isn't a numeric type.
 ************************************************************************************************/
PInterp::Result PInterp::SIN(PInterp::DatumVecIter TOS) {
	Result r = Result::success;

	if (TOS->kind() == Datum::Integer)
		*TOS = sin(TOS->integer());
	else if (TOS->kind() == Datum::Real)
		*TOS = sin(stack[sp].real());
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * Replace the numeric TOS value with it's square
 * @param	TOS	Value to square
 * @return	badDataType if TOS isn't a numeric type.
 ************************************************************************************************/
PInterp::Result PInterp::SQR(PInterp::DatumVecIter TOS) {
	Result r = Result::success;

	// note that Datum lacks a *= operator
	if (TOS->kind() == Datum::Integer)
		*TOS = TOS->integer() * TOS->integer();
	else if (TOS->kind() == Datum::Real)
		*TOS = TOS->real() * TOS->real();
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * Replace the numeric TOS value with its square-root.
 * @param	TOS	Value to calculate square root of
 * @return	badDataType if TOS isn't a numeric type.
 ************************************************************************************************/
PInterp::Result PInterp::SQRT(PInterp::DatumVecIter TOS) {
	Result r = Result::success;

	if (TOS->kind() == Datum::Integer)
		*TOS = sqrt(TOS->integer());
	else if (TOS->kind() == Datum::Real)
		*TOS = sqrt(TOS->real());
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * Replace the integer TOS valeu with it's successor value.
 * @param	TOS	Value to calculate successor of
 * @return	outOfRange if the operation would exceed the types range. 
 ************************************************************************************************/
PInterp::Result PInterp::SUCC(PInterp::DatumVecIter TOS) {
	Result r = Result::success;

	if (*TOS >= ir.value)
		r =  Result::outOfRange;
	++(*TOS);

	return r;
}

/********************************************************************************************//**
 * Writes zero or more elements on the standard output stream. Each element is described by a
 * 3-tuple in the form v:w:p for Value, field Width and Precision if value is a Real. TOS is the
 * number of 3-tuple elements preceeding it, with the last 3-tuple precieeding the count.
 *
 * @bug		Doesn't support arrays, including strings.
 *
 * @note	Maybe add an element count to each tuple ; v:w:p:n where n is the number of elements
 *			in v, i.e., 1 for scalers, or the number of elements in an array.
 *
 * @param	TOS	Number of 4-tuple elements to write
 * @return	BadDataType if TOS isn't an integer, stackUnderflow if the would underflow.
 ************************************************************************************************/
PInterp::Result PInterp::WRITE(PInterp::DatumVecIter TOS) {
	const	unsigned	tupleSz = 4;	// Each parameter is a 4-tuple
			Result		r = Result::success;
			size_t		nargs = 0;		// # of tuples to process...

	if (TOS->kind() != Datum::Integer) {
		cerr << "WRITE TOS is not an integer!" << endl;
		r =  Result::badDataType;

	} else {
		nargs = TOS->integer();
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
 * @param	TOS	Number of 3-tuple elements to write
 * @return	BadDataType if TOS isn't an integer, stackUnderflow if the would underflow.
 ************************************************************************************************/
PInterp::Result PInterp::WRITELN(PInterp::DatumVecIter TOS) {
	auto r = WRITE(TOS);
	cout << '\n';
	return r;
}

/********************************************************************************************//**
 * Replaces the TOS, which is the number of Datums to allocate on the heap, and if successful,
 * replaces the TOS with the address of the new block, or zero if there was insufficient space
 * in the heap.
 *
 * @param	TOS	Number of Datums to allocate off the heap.
 * @return	badDataType if TOS isn't an Integer.
 ************************************************************************************************/
PInterp::Result PInterp::NEW(PInterp::DatumVecIter TOS) {
	if (TOS->kind() != Datum::Integer) {
		cerr << "NEW TOS is not an integer!" << endl;
		return Result::badDataType;
	}

	push(heap.alloc(pop().address()));
	if (trace)
		heap.dump(cout);					// Dump the new heap state...

	return Result::success;
}

/********************************************************************************************//**
 * Return a block of data, allocated by NEW, back on the heap
 * @param	TOS	Address of a block returned by NEW
 * @return	badDataType if the TOS isn't an Integer.
 ************************************************************************************************/
PInterp::Result PInterp::DISPOSE(PInterp::DatumVecIter TOS) {
	if (TOS->kind() != Datum::Address) {
		cerr << "DISPOSE TOS is not an address!" << endl;
		return Result::badDataType;
	}

	const size_t addr = TOS->address(); pop();
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
 * @param	TOS	Value to negate
 * @return	badDataType if TOS isn't numeric.
 ************************************************************************************************/
PInterp::Result PInterp::NEG(PInterp::DatumVecIter TOS) {
	if (TOS->numeric()) {
		*TOS = -*TOS;
		return Result::success;

	} else
		return Result::badDataType;
}

/********************************************************************************************//**
 * Replace the top two numeric values on the stack with their sum
 * @return	stackUndeflow if the stack underflowed, badDataType if either operand isn't numeric.
 ************************************************************************************************/
PInterp::Result PInterp::ADD() {
	if (sp < 2)
		return Result::stackUnderflow;

	const auto rhand = pop();
	const auto lhand = pop();

	if (lhand.numeric() && rhand.numeric()) {
		push(lhand + rhand);
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
PInterp::Result PInterp::SUB() {
	if (sp < 2)
		return Result::stackUnderflow;

	const auto rhand = pop();
	const auto lhand = pop();

	if (lhand.numeric() && rhand.numeric()) {
		push(lhand - rhand);
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
PInterp::Result PInterp::MUL() {
	if (sp < 2)
		return Result::stackUnderflow;

	const auto rhand = pop();
	const auto lhand = pop();

	if (lhand.numeric() && rhand.numeric()) {
		push(lhand * rhand);
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
PInterp::Result PInterp::DIV() {
	Result r = Result::success;

	if (sp < 2)
		r =  Result::stackUnderflow;

	else {
		const auto rhand = pop();
		const auto lhand = pop();

		if (!lhand.numeric() || !rhand.numeric()) {
			cerr << "Attampt to divide with non-numbeic value\n";
			r =  Result::badDataType;

		} else if (rhand.kind() == Datum::Real && rhand.real() == 0.0) {
			cerr << "Attempt to divide by zero @ pc (" << prevPc << ")!\n";
			r= Result::divideByZero;

		} else if (rhand.kind() == Datum::Integer && rhand.integer() == 0) {
			cerr << "Attempt to divide by zero @ pc (" << prevPc << ")!\n";
			r = Result::divideByZero;

		} else
			push(lhand / rhand);
	}

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
PInterp::Result PInterp::REM() {
	Result r = Result::success;

	if (sp < 2)
		r =  Result::stackUnderflow;

	else {
		const auto rhand = pop();
		const auto lhand = pop();

		if (lhand.kind() != Datum::Integer || rhand.kind() != Datum::Integer) {
			cerr << "Attampt to calulate reminder with non-integer value\n";
			r =  Result::badDataType;

		} else if (rhand.integer() == 0) {
			cerr << "Attempt to divide by zero @ pc (" << prevPc << ")!\n";
			r = Result::divideByZero;

		} else
			push(lhand % rhand);
	}

	if (r != Result::success)
		push(0);

	return r;
}

/********************************************************************************************//**
 * Replace the top two values with true if the left hand value is less than the right, false 
 * otherwise.
 *
 * @note TOS is the right hand value, TOS is the left hand value.
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
PInterp::Result PInterp::LT() {
	Result r = Result::success;

	if (sp < 2)
		r =  Result::stackUnderflow;

	else {
		const auto rhand = pop();
		const auto lhand = pop();

		if (!rhand.numeric() || !lhand.numeric()) {
			cerr << "Non-numeric binary value\n";
			push(false);
			r = Result::badDataType;

		} else
			push(lhand < rhand ? true : false);
	}
	
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
PInterp::Result PInterp::LTE() {
	Result r = Result::success;

	if (sp < 2)
		r =  Result::stackUnderflow;

	else {
		const auto rhand = pop();
		const auto lhand = pop();

		if (!rhand.numeric() || !lhand.numeric()) {
			cerr << "Non-numeric binary value\n";
			push(false);
			r = Result::badDataType;

		} else
			push(lhand <= rhand ? true : false);
	}
	
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
PInterp::Result PInterp::EQU() {
	Result r = Result::success;

	if (sp < 2)
		r =  Result::stackUnderflow;

	else {
		const auto rhand = pop();
		const auto lhand = pop();

		if (!rhand.numeric() || !lhand.numeric()) {
			cerr << "Non-numeric binary value\n";
			push(false);
			r = Result::badDataType;

		} else
			push(Datum(lhand == rhand ? true : false));
	}
	
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
PInterp::Result PInterp::GTE() {
	Result r = Result::success;

	if (sp < 2)
		r =  Result::stackUnderflow;

	else {
		const auto rhand = pop();
		const auto lhand = pop();

		if (!rhand.numeric() || !lhand.numeric()) {
			cerr << "Non-numeric binary value\n";
			push(false);
			r = Result::badDataType;

		} else
			push(lhand >= rhand ? true : false);
	}
	
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
PInterp::Result PInterp::GT() {
	Result r = Result::success;

	if (sp < 2)
		r =  Result::stackUnderflow;

	else {
		const auto rhand = pop();
		const auto lhand = pop();

		if (!rhand.numeric() || !lhand.numeric()) {
			cerr << "Non-numeric binary value\n";
			push(false);
			r = Result::badDataType;

		} else
			push(Datum(lhand > rhand ? true : false));
	}
	
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
PInterp::Result PInterp::NEQ() {
	Result r = Result::success;

	if (sp < 2)
		r =  Result::stackUnderflow;

	else {
		const auto rhand = pop();
		const auto lhand = pop();

		if (!rhand.numeric() || !lhand.numeric()) {
			cerr << "Non-numeric binary value\n";
			push(false);
			r = Result::badDataType;

		} else
			push(Datum(lhand != rhand ? true : false));
	}
	
	return r;
}

/********************************************************************************************//**
 * Replace the top two Boolean values on the stack with their Boolean OR.
 *
 * @note TOS is the right hand value, TOS-1 is the left hand value.
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
PInterp::Result PInterp::OR() {
	Result r = Result::success;

	if (sp < 2)
		r =  Result::stackUnderflow;

	else {
		const auto rhand = pop();
		const auto lhand = pop();

		if (lhand.kind() != Datum::Boolean || rhand.kind() != Datum::Boolean) {
			cerr << "Non-boolean binary value\n";
			push(false);
			r = Result::badDataType;

		} else
			push(Datum(lhand || rhand ? true : false));
	}
	
	return r;
}

/********************************************************************************************//**
 * Replace the top two Boolean values on the stack with their Boolean AND.
 *
 * @note TOS is the right hand value, TOS-1 is the left hand value.
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
PInterp::Result PInterp::AND() {
	Result r = Result::success;

	if (sp < 2)
		r =  Result::stackUnderflow;

	else {
		const auto rhand = pop();
		const auto lhand = pop();

		if (lhand.kind() != Datum::Boolean || rhand.kind() != Datum::Boolean) {
			cerr << "Non-boolean binary value\n";
			push(false);
			r = Result::badDataType;

		} else
			push(Datum(lhand && rhand ? true : false));
	}
	
	return r;
}

/********************************************************************************************//**
 * Replace the Boolean value on the TOS with it's Boolean NOT.
 *
 * @note TOS value to "not"
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
PInterp::Result PInterp::NOT(DatumVecIter TOS) {
	Result r = Result::success;
	if (TOS->kind() != Datum::Boolean)
		r = Result::badDataType;
	else
		*TOS = !TOS->boolean();

	return r;
}

/********************************************************************************************//**
 * Pops ir.value items off of the stack
 *
 * @return	stackUnderflow if the stack underflowed.
 ************************************************************************************************/
PInterp::Result PInterp::POP() {
	if (ir.value < Datum(0))
		return Result::stackUnderflow;

	const size_t n = ir.value.address();		// Accually, just an unsigned value
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
PInterp::Result PInterp::PUSH() {
	push(ir.value);
	return Result::success;
}

/********************************************************************************************//**
 * Push a variable reference (base(ir.level) + ir.value, onto the stack.
 *
 * @return	success
 ************************************************************************************************/
PInterp::Result PInterp::PUSHVAR() {
	push(base(ir.level) + ir.value.integer());
	return Result::success;
}

/********************************************************************************************//**
 * Evaluates ir.value Datums, whose starting address is on the stop of the stack. Replaces the
 * address with the values.
 *
 * @return	stackUnderflow if the stack underflowed, 
 ************************************************************************************************/
PInterp::Result PInterp::EVAL() {
	const	size_t	n = ir.value.address();	// acually, an unsigned integer
			Result	r = Result::success;

	if (sp < n) {
		cerr << "Stack underflow evaluating " << n << " Datums!\n";
		r = Result::stackUnderflow;
	}

	size_t dst = pop().address();
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
PInterp::Result PInterp::ASSIGN() {
	const	size_t	n = ir.value.address();	// Accually, just unsigned
			Result	r = Result::success;

	if (sp < n)
		r = Result::stackUnderflow;

	size_t dst = stack[sp - ir.value.address()].address();
	if (!rangeCheck(dst, dst + n))
		r = Result::stackUnderflow;

	lastWrite = dst + n - 1;

	size_t src = sp - n + 1;
	for (size_t i = 0; i < n; ++i)
		stack[dst++] = stack[src++];

	pop(n+1);					// 'pop' the stack, including dest addr

	return r;
}

/********************************************************************************************//**
 * Copy N Datums, where N is ir.value, and the source starting address is TOS, and the destination 
 * starting address is TOS-1. The source and destination addresses are consumed.
 * @return	stackUnderflow if the stack underflowed.
 ************************************************************************************************/
PInterp::Result PInterp::COPY() {
	const	size_t	n = ir.value.address();	// Accually, just unsinged value
			Result	r = Result::success;

	size_t src  = pop().address();
	if (!rangeCheck(src, src + n))
		r = Result::stackUnderflow;

	size_t dst = pop().address();
	if (!rangeCheck(dst, dst + n))
		r = Result::stackUnderflow;

	lastWrite = dst + n;
	for (size_t i = 0; i < n; ++i)
		stack[dst++] = stack[src++];

	return r;
}

/********************************************************************************************//**
 * Call a subroutine whose level is ir.level and whose entry point is ir.value.
 * @return	success.
 ************************************************************************************************/
PInterp::Result PInterp::CALL() {
#if 1
	const	size_t	addr = pop().address();
	const	int8_t	nlevel = pop().integer();
#else
	const	int8_t	nlevel = ir.level;
	const	size_t	addr = ir.value.address();
#endif
	const	size_t	oldFp = fp;			// Save a copy before we modify it

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
 * Unlinks the stack frame, setting the return address as the next instruciton.
 * @return	success.
 ************************************************************************************************/
PInterp::Result PInterp::RET() {
	sp = fp - 1; 					// "pop" the activaction frame
	pc = stack[fp + FrameRetAddr].address();
	fp = stack[fp + FrameOldFp].address();
	sp -= ir.value.address();		// Pop n parameters, if any...

	return Result::success;
}

/********************************************************************************************//**
 * Unlink the stack frame, set the return address, and then push the function result
 * @return	success.
 ************************************************************************************************/
PInterp::Result PInterp::RETF() {
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
PInterp::Result PInterp::ENTER() {
	sp += ir.value.integer();

	return Result::success;
}

/********************************************************************************************//**
 * Jumps to TOS
 * @return	success.
 ************************************************************************************************/
PInterp::Result PInterp::JUMP(PInterp::DatumVecIter TOS) {
	pc = TOS->address();
	pop();

	return Result::success;
}

/********************************************************************************************//**
 * Jump to TOS if TOS-1 == false.
 * @return	badDataType if TOS isn't a Boolean.
 ************************************************************************************************/
PInterp::Result PInterp::JNEQ() {
	const uint32_t addr = pop().address();
	const Datum value = pop();

	if (value.kind() != Datum::Boolean)
		return Result::badDataType;

	if (value.boolean() == false)
		pc = addr;

	return Result::success;
}

/********************************************************************************************//**
 * @note	The TOS is not consumed.
 * @param	TOS	Value to test
 * @return	BadDataType if TOS isn't an Integer or a Boolean, outOfRange if the check failed.
 ************************************************************************************************/
PInterp::Result PInterp::LLIMIT(PInterp::DatumVecIter TOS) {
	if (!TOS->ordinal()) 
		return Result::badDataType;

	else if (TOS->kind() == Datum::Boolean) {
		const Datum i(TOS->boolean() ? 1 : 0);
		return i < ir.value ? Result::outOfRange : Result::success;

	} else if (TOS->kind() == Datum::Character) {
		const Datum i(static_cast<size_t>(TOS->character()));
		return i < ir.value ? Result::outOfRange : Result::success;

	} else 
		return *TOS < ir.value ? Result::outOfRange : Result::success;
}

/********************************************************************************************//**
 * @note	The TOS is not consumed.
 * @param	TOS	Value to test
 * @return	BadDataType if TOS isn't an Integer or a Boolean, outOfRange if the check failed.
 ************************************************************************************************/
PInterp::Result PInterp::ULIMIT(PInterp::DatumVecIter TOS) {
	if (!TOS->ordinal()) 
		return Result::badDataType;

	else if (TOS->kind() == Datum::Boolean) {
		const Datum i(TOS->boolean() ? 1 : 0);
		return i > ir.value ? Result::outOfRange : Result::success;

	} else if (TOS->kind() == Datum::Character) {
		const Datum i(static_cast<size_t>(TOS->character()));
		return i > ir.value ? Result::outOfRange : Result::success;

	} else
		return *TOS > ir.value ? Result::outOfRange : Result::success;
}

/********************************************************************************************//**
 * @return	halted
 ************************************************************************************************/
PInterp::Result PInterp::HALT() {
	return Result::halted;
}

/********************************************************************************************//**
 * @return Result::success or...
 ************************************************************************************************/
PInterp::Result PInterp::step() {
	prevPc = pc++;
	ir = code[prevPc];					// Fetch next instruction...
	++ncycles;

	auto info = OpCodeInfo::info(ir.op);
	if (sp < info.nElements()) {
		cerr << "Out of bounds stack access @ pc (" << prevPc << "), sp == " << sp << "!\n";
		return Result::stackUnderflow;
	}
	assert(sp < stack.size());

	Result r = Result::success;
	DatumVecIter TOS = stack.begin() + sp;

	switch(ir.op) {
	case OpCode::ITOR:		r = ITOR(TOS);		break;	
	case OpCode::ITOR2:		r = ITOR2(TOS);		break;
	case OpCode::ROUND:		r = ROUND(TOS);		break;
	case OpCode::TRUNC:		r = TRUNC(TOS);		break;
	case OpCode::ABS:		r = ABS(TOS);		break;
	case OpCode::ATAN:		r = ATAN(TOS);		break;
	case OpCode::EXP: 		r = EXP(TOS);		break;
	case OpCode::LOG: 		r = LOG(TOS);		break;
	case OpCode::DUP:		r = DUP(TOS);		break;
	case OpCode::ODD:		r = ODD(TOS);		break;
	case OpCode::PRED:		r = PRED(TOS);		break;
	case OpCode::SIN:		r = SIN(TOS);		break;
	case OpCode::SQR:		r = SQR(TOS);		break;
	case OpCode::SQRT:		r = SQRT(TOS);		break;
	case OpCode::SUCC:		r = SUCC(TOS);		break;
	case OpCode::WRITE:		r = WRITE(TOS);		break;
	case OpCode::WRITELN:	r = WRITELN(TOS);	break;
	case OpCode::NEW:		r = NEW(TOS);		break;
	case OpCode::DISPOSE:	r = DISPOSE(TOS);	break;
	case OpCode::NEG:		r = NEG(TOS);		break;
	case OpCode::ADD:		r = ADD();			break;
	case OpCode::SUB:		r = SUB();			break;
	case OpCode::MUL:		r = MUL();			break;
	case OpCode::DIV:		r = DIV();			break;
	case OpCode::REM:		r = REM();			break;
	case OpCode::LT:		r = LT();			break;
	case OpCode::LTE:   	r = LTE();			break;
	case OpCode::EQU:   	r = EQU();			break;
	case OpCode::GTE:  		r = GTE();			break;
	case OpCode::GT:		r = GT();			break;
	case OpCode::NEQ: 		r = NEQ();			break;
	case OpCode::OR:  		r = OR();			break;
	case OpCode::AND: 		r = AND();			break;
	case OpCode::NOT:		r = NOT(TOS);		break;
	case OpCode::POP:		r = POP();			break;
	case OpCode::PUSH: 		r = PUSH();			break;
	case OpCode::PUSHVAR:	r = PUSHVAR();		break;
	case OpCode::EVAL:		r = EVAL();			break;
	case OpCode::ASSIGN:	r = ASSIGN();		break;
	case OpCode::COPY:		r = COPY();			break;
	case OpCode::CALL: 		r = CALL();			break;
	case OpCode::RET:   	r = RET();  		break;
	case OpCode::RETF: 		r = RETF();			break;
	case OpCode::ENTER:		r = ENTER();		break;
	case OpCode::JUMP:		r = JUMP(TOS);		break;
	case OpCode::JNEQ:		r = JNEQ();			break;
	case OpCode::LLIMIT:	r = LLIMIT(TOS);	break;
	case OpCode::ULIMIT: 	r = ULIMIT(TOS);	break;
	case OpCode::HALT:		r = HALT();			break;

	default:
		cerr 	<< "Unknown op-code: " << OpCodeInfo::info(ir.op).name()
				<< " found at pc (" << prevPc << ")!\n" << endl;
		return Result::unknownInstr;
	};

	return r;
}

/********************************************************************************************//**
 *  @return	Result::success, or ...
 ************************************************************************************************/
PInterp::Result PInterp::run() {
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

	} catch (Error& exp) {
		cerr << "runtime error @pc " << prevPc << ", sp: " << sp << ": " << exp.what() << endl;
		return exp.result();

	} catch (Datum::IllegalOp& exp) {
		cerr << exp.what() << " @pc " << prevPc << ", sp: " << sp << endl;
		return Result::badDataType;

	} catch (Datum::DivideByZero& exp) {
		cerr << exp.what() << " @pc " << prevPc << ", sp: " << sp << endl;
		return Result::divideByZero;
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
PInterp::Result PInterp::operator()(const InstrVector& prog, bool trce) {
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

// operators

/********************************************************************************************//**
 * Puts Result value on os
 *
 * @param	os		Stream to write value to 
 * @param	result	The value to write 
 * @return	os 
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const PInterp::Result& result) {
	switch (result) {
	case PInterp::success:			os << "success";				break;
	case PInterp::divideByZero:		os << "divide-by-zero";			break;
	case PInterp::badFetch:			os << "bad fetch";				break;
	case PInterp::badDataType:		os << "bad data type";			break;
	case PInterp::unknownInstr:		os << "unknown instruction";	break;
	case PInterp::stackOverflow:	os << "stack overflow";			break;
	case PInterp::stackUnderflow:	os << "stack underflow";		break;
	case PInterp::freeStoreError:	os << "free-store error";		break;
	case PInterp::outOfRange:		os << "out-of-range";			break;
	case PInterp::halted:			os << "halted";					break;
	default:
		return os << "undefined result!";
		assert(false);
	}

	return os;
}

