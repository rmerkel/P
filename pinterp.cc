/********************************************************************************************//**
 * @file pinterp.cc
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

#include "pinterp.h"

using namespace std;

// private:

/********************************************************************************************//**
 * Dump the current machine state
 ************************************************************************************************/
void PInterp::dump() {
	if (lastWrite.valid())				// dump the last write...
		cout << "    "
			 << setw(5)	<< lastWrite << ": "
			 << setw(10) << stack[lastWrite]
			 << std::endl;
	lastWrite.invalidate();

	if (!verbose) return;

	// Dump the current  activation frame...
	assert(sp >= fp);
	cout    << "fp: " 	<< setw(5)	<< fp << ": "
			<< right 	<< setw(10)	<< stack[fp]
			<< endl;

	for (auto bl = fp+1; bl < sp; ++bl)
		cout
			<<	"    "	<< setw(5)	<< bl << ": "
			<< right	<< setw(10) << stack[bl]
			<< endl;

	cout    << "sp: " 	<< setw(5) 	<< sp << ": "
			<< right	<< setw(10) << stack[sp]
			<< endl;

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
unsigned PInterp::base(unsigned lvl) {
	auto b = fp;
	for (; lvl > 0; --lvl)
		b = stack[b].natural();

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
void PInterp::pop(unsigned n)	{
	if (sp < n)
		throw Error(stackUnderflow, "stack underflow error");
	else
		sp -= n;
}

/********************************************************************************************//**
 * @param d	Datum to push on to the stack
 ************************************************************************************************/
void PInterp::push(Datum d) {
	if (sp >= heap.addr())
		throw Error(stackOverflow, "stack overflow error");
	else
		stack[++sp] = d;
}

/********************************************************************************************//**
 * expression, width, precision are on the stack.
 * @note caller must consume the expression, widht and preceision.
 * @param index	index into stack[] for the expr, width [, precision] tuple to print
 * @return false if width or precision is negative, true otherwise
 ************************************************************************************************/
PInterp::Result PInterp::write1(unsigned index) {
	if (sp < 3)
		return stackUnderflow;

	const Datum value = stack[index];
	const Datum width = stack[index+1];
	const Datum prec = stack[index+2];

	unsigned w = width.natural();
	unsigned p = prec.natural();

	Result r = Result::success;
	if (width.kind() != Datum::Integer) {
		cerr << "WRITE[LN] width parameter is not an integer!" << endl;
		w = 0;
		r = Result::badDataType;

	} else if (prec.kind() != Datum::Integer) {
		cerr << "WRITE[LN] precision parameter is not an integer!" << endl;
		p = 0;
		r =  Result::badDataType;
	}

	switch(value.kind()) {
	case Datum::Boolean:	cout << boolalpha << value;						break;
	case Datum::Character:	cout << setw(w) << value;						break;
	case Datum::Integer:	cout << setw(w) << setprecision(p) << value;	break;
	case Datum::Real:
		if (p == 0)
			cout << setw(w) << scientific << setprecision(6) << value;
		else
			cout << setw(w) << fixed << setprecision(p) << value;
		break;

	default:
		cerr << "unknown datum type: " << static_cast<unsigned>(value.kind()) << endl;
		r = Result::badDataType;
	}

	return r;
}

/************************************************************************************************
 * Instructions...
 ************************************************************************************************/

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDataType if TOS isn't an integer
 ************************************************************************************************/
PInterp::Result PInterp::ITOR(PInterp::DatumVecIter tos) {
	if (tos->kind() == Datum::Integer) {
		*tos = tos->integer() * 1.0;
		return Result::success;

	} else
		return Result::badDataType;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDataType if TOS isn't a Real
 ************************************************************************************************/
PInterp::Result PInterp::ROUND(PInterp::DatumVecIter tos) {
	if (tos->kind() == Datum::Real) {
		*tos  = static_cast<int>(round(tos->real()));
		return Result::success;

	} else
		return Result::badDataType;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDataType if TOS isn't a real
 ************************************************************************************************/
PInterp::Result PInterp::TRUNC(PInterp::DatumVecIter tos) {
	if (tos->kind() == Datum::Real) {
		*tos = static_cast<int>(tos->real());
		return Result::success;

	} else
		return Result::badDataType;
 }

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDataType if TOS isn't a numeric type
 ************************************************************************************************/
PInterp::Result PInterp::ABS(PInterp::DatumVecIter tos) {
	Result r = Result::success;

	if (tos->kind() == Datum::Integer)
		*tos = abs(tos->integer());
	else if (tos->kind() == Datum::Real)
		*tos = fabs(tos->real());
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDagtaType if TOS isn't a numeric type
 ************************************************************************************************/
PInterp::Result PInterp::ATAN(PInterp::DatumVecIter tos) {
	Result r = Result::success;

	if (tos->kind() == Datum::Integer)
		*tos = atan(tos->integer());
	else if (tos->kind() == Datum::Real)
		*tos = atan(tos->real());
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDataType if TOS isn't a numeric type.
 ************************************************************************************************/
PInterp::Result PInterp::EXP(PInterp::DatumVecIter tos) {
	Result r = Result::success;

	if (tos->kind() == Datum::Integer)
		*tos = exp(tos->integer());
	else if (tos->kind() == Datum::Real)
		*tos = exp(tos->real());
	else
		return Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDataType if TOS isn't a numeric type, dividyByZero if zero.
 ************************************************************************************************/
PInterp::Result PInterp::LOG(PInterp::DatumVecIter tos) {
	Result r = Result::success;

	if (!tos->numeric())
		r = Result::badDataType;

	else if (tos->kind() == Datum::Real) {
		if (tos->real() == 0.0) {
			cerr << "Attempt to take log(0.0) @ pc (" << prevPc << ")!\n";
			r = Result::divideByZero;

		} else 
			*tos = log(tos->real());

	} else if (tos->kind() == Datum::Integer) {
		if (tos->kind() == Datum::Integer && tos->integer() == 0) {
			cerr << "Attempt to take log(0) @ pc (" << prevPc << ")!\n";
			r = Result::divideByZero;

		} else 
			*tos = log(tos->integer());
	}

	return r;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDataType if TOS isn't an Integer.
 ************************************************************************************************/
PInterp::Result PInterp::ODD(PInterp::DatumVecIter tos) {
	if (tos->kind() == Datum::Integer) {
		*tos = (tos->natural() & 1) ? true : false;
		return Result::success;

	} else
		return Result::badDataType;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	outOfRange if the operation would exceed the types range. 
 ************************************************************************************************/
PInterp::Result PInterp::PRED(PInterp::DatumVecIter tos) {
	Result r = Result::success;

	if (*tos <= ir.addr.integer())
		r =  Result::outOfRange;
	*tos = tos->integer() - 1;

	return r;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDataType if TOS isn't a numeric type.
 ************************************************************************************************/
PInterp::Result PInterp::SIN(PInterp::DatumVecIter tos) {
	Result r = Result::success;

	if (tos->kind() == Datum::Integer)
		*tos = sin(tos->integer());
	else if (tos->kind() == Datum::Real)
		*tos = sin(stack[sp].real());
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDataType if TOS isn't a numeric type.
 ************************************************************************************************/
PInterp::Result PInterp::SQR(PInterp::DatumVecIter tos) {
	Result r = Result::success;

	// note that Datum lacks a *= operator
	if (tos->kind() == Datum::Integer)
		*tos = tos->integer() * tos->integer();
	else if (tos->kind() == Datum::Real)
		*tos = tos->real() * tos->real();
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDataType if TOS isn't a numeric type.
 ************************************************************************************************/
PInterp::Result PInterp::SQRT(PInterp::DatumVecIter tos) {
	Result r = Result::success;

	if (tos->kind() == Datum::Integer)
		*tos = sqrt(tos->integer());
	else if (tos->kind() == Datum::Real)
		*tos = sqrt(tos->real());
	else
		r = Result::badDataType;

	return r;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	outOfRange if the operation would exceed the types range. 
 ************************************************************************************************/
PInterp::Result PInterp::SUCC(PInterp::DatumVecIter tos) {
	Result r = Result::success;

	if (*tos >= ir.addr.integer())
		r =  Result::outOfRange;
	*tos = tos->integer() + 1;

	return r;
}

/********************************************************************************************//**
 * TOS contains the number of elements, followed by that many expr, width, precision tuples.
 * @param	tos	Top-of-Stack
 * @return	BadDataType if TOS isn't an integer, stackUnderflow if the would underflow.
 ************************************************************************************************/
PInterp::Result PInterp::WRITE(PInterp::DatumVecIter tos) {
	const	unsigned	tupleSz = 3;	// Each parameter is a 3-tuple
			Result		r = Result::success;
			unsigned	nargs = 0;		// # of tuples to process...

	if (tos->kind() != Datum::Integer) {
		cerr << "WRITE TOS is not an integer!" << endl;
		r =  Result::badDataType;

	} else {
		nargs = tos->natural();
		if (nargs > sp) {
			cerr << "Stack underflow @ pc " << pc << endl;
			r = Result::stackUnderflow; 

		} else if (nargs > 0) {
			// Write each expressoin tuple on standard output, seperated by spaces
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
 * @param	tos	Top-of-Stack
 * @return	BadDataType if TOS isn't an integer, stackUnderflow if the would underflow.
 ************************************************************************************************/
PInterp::Result PInterp::WRITELN(PInterp::DatumVecIter tos) {
	auto r = WRITE(tos);
	cout << '\n';
	return r;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDataType if TOS isn't an Integer.
 ************************************************************************************************/
PInterp::Result PInterp::NEW(PInterp::DatumVecIter tos) {
	if (tos->kind() != Datum::Integer) {
		cerr << "WRITE TOS is not an integer!" << endl;
		return Result::badDataType;
	}

	push(heap.alloc(pop().natural()));
	if (verbose)
		heap.dump(cout);					// Dump the new heap state...

	return Result::success;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDataType if the TOS isn't an Integer.
 ************************************************************************************************/
PInterp::Result PInterp::DISPOSE(PInterp::DatumVecIter tos) {
	if (tos->kind() != Datum::Integer) {
		cerr << "WRITE TOS is not an integer!" << endl;
		return Result::badDataType;
	}

	const auto addr = tos->natural(); pop();
	if (!heap.free(addr)) {
		cerr << "Dispose of " << addr << " failed!\n";
		return Result::freeStoreError;
	}

	if (verbose)
		heap.dump(cout);					// Dump the new heap state...

	return Result::success;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDataType if TOS isn't numeric.
 ************************************************************************************************/
PInterp::Result PInterp::NEG(PInterp::DatumVecIter tos) {
	if (tos->numeric()) {
		*tos = -*tos;
		return Result::success;

	} else
		return Result::badDataType;
}

/********************************************************************************************//**
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
			push(lhand == rhand ? true : false);
	}
	
	return r;
}

/********************************************************************************************//**
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
			push(lhand > rhand ? true : false);
	}
	
	return r;
}

/********************************************************************************************//**
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
PInterp::Result PInterp::NEQU() {
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
			push(lhand != rhand ? true : false);
	}
	
	return r;
}

/********************************************************************************************//**
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
PInterp::Result PInterp::LOR() {
	Result r = Result::success;

	if (sp < 2)
		r =  Result::stackUnderflow;

	else {
		const auto rhand = pop();
		const auto lhand = pop();

		push(lhand || rhand ? true : false);
	}
	
	return r;
}

/********************************************************************************************//**
 * @return	stackUnderflow if the stack underflowed, badDataType if either operation arn't
 *			numeric.
 ************************************************************************************************/
PInterp::Result PInterp::LAND() {
	Result r = Result::success;

	if (sp < 2)
		r =  Result::stackUnderflow;

	else {
		const auto rhand = pop();
		const auto lhand = pop();

		push(lhand && rhand ? true : false);
	}
	
	return r;
}

/********************************************************************************************//**
 * @return	stackUnderflow if the stack underflowed.
 ************************************************************************************************/
PInterp::Result PInterp::POP() {
	if (sp < ir.addr.natural())
		return Result::stackUnderflow;

	for (unsigned i = 0; i < ir.addr.natural(); ++i)
			pop();

	return Result::success;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	badDataType if TOS isn't a Boolean.
 ************************************************************************************************/
PInterp::Result PInterp::JNEQ(PInterp::DatumVecIter tos) {
	if (tos->kind() != Datum::Boolean)
		return Result::badDataType;

	if (tos->boolean() == false)
		pc = ir.addr.natural();

	pop();

	return Result::success;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	BadDataType if TOS isn't an Integer or a Boolean, outOfRange if the check failed.
 ************************************************************************************************/
PInterp::Result PInterp::LLIMIT(PInterp::DatumVecIter tos) {
	if (!tos->ordinal()) 
		return Result::badDataType;
	else if (*tos < ir.addr.integer())
		return Result::outOfRange;
	else
		return Result::success;
}

/********************************************************************************************//**
 * @param	tos	Top-of-Stack
 * @return	BadDataType if TOS isn't an Integer or a Boolean, outOfRange if the check failed.
 ************************************************************************************************/
PInterp::Result PInterp::ULIMIT(PInterp::DatumVecIter tos) {
	if (!tos->ordinal()) 
		return Result::badDataType;
	else if (*tos > ir.addr.integer())
		return Result::outOfRange;
	else
		return Result::success;
}

/********************************************************************************************//**
 * @param 	nlevel	Set the subroutines frame base nlevel's down
 * @param 	addr 	The address of the subroutine.
 ************************************************************************************************/
void PInterp::CALL(int8_t nlevel, unsigned addr) {
	const auto oldFp = fp;			// Save a copy before we modify it

	// Push a new activation frame block on the stack:

	push(base(nlevel));				//	FrameBase

	fp = sp;						// 	fp points to the start of the new frame

	push(oldFp);					//	FrameOldFp
	push(pc);						//	FrameRetAddr
	push(0u);						//	FrameRetVal

	pc = addr;
}

/********************************************************************************************//**
 * Unlinks the stack frame, setting the return address as the next instruciton.
 ************************************************************************************************/
void PInterp::RET() {
	sp = fp - 1; 					// "pop" the activaction frame
	pc = stack[fp + FrameRetAddr].natural();
	fp = stack[fp + FrameOldFp].natural();
	sp -= ir.addr.natural();		// Pop parameters, if any...
}

/********************************************************************************************//**
 * Unlink the stack frame, set the return address, and then push the function result
 ************************************************************************************************/
void PInterp::RETF() {
	// Save the function result, unlink the stack frame, return the result
	auto temp = stack[fp + FrameRetVal];
	RET();
	push(temp);
}

/********************************************************************************************//**
 * Assigns N Datums, where N is ir.addr, off the stack.  Stack layout before;
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
 * @param	n	Number of Datums to assign.
 ************************************************************************************************/
void PInterp::ASSIGN(unsigned n) {
	if (sp < n)
		throw Error(stackUnderflow, "stack underflow error");

	size_t dst = stack[sp - ir.addr.natural()].natural();
	if (!rangeCheck(dst, dst + n))
		throw Error(stackUnderflow, "stack underflow error");

	lastWrite = dst + n - 1;

	size_t src = sp - n + 1;
	for (size_t i = 0; i < n; ++i)
		stack[dst++] = stack[src++];

	sp -= n + 1;				// 'pop' the stack, including dest addr
}

/********************************************************************************************//**
 * Evaluate N Datums, whose starting address is on the stop of the stack. Replaces the address
 * with the values.
 *
 * @param	n	Number of Datums to assign.
 ************************************************************************************************/
void PInterp::EVAL(unsigned n) {
	if (sp < n)
		throw Error(stackUnderflow, "stack underflow error");

	unsigned dst = pop().natural();
	if (!rangeCheck(dst, dst + n))
		throw Error(stackUnderflow, "stack underflow error");

	for (unsigned i = 0; i < n; ++i)
		push(stack[dst++]);
}

/********************************************************************************************//**
 * Copy N Datums. Stack before;
 *
 * stack  | contents
 * ------ | ----------------------------------
 * sp+1   | dst - destination starting address
 * ------ | ----------------------------------
 * sp     | src - source starting address
 *
 ************************************************************************************************/
void PInterp::COPY(unsigned n) {
	unsigned src  = pop().natural();
	if (!rangeCheck(src, src + n))
		throw Error(stackUnderflow, "stack underflow error");

	unsigned dst = pop().natural();
	if (!rangeCheck(dst, dst + n))
		throw Error(stackUnderflow, "stack underflow error");

	lastWrite = dst + n;
	for (unsigned i = 0; i < ir.addr.natural(); ++i)
		stack[dst++] = stack[src++];
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
	DatumVecIter tos = stack.begin() + sp;

	switch(ir.op) {
	case OpCode::ITOR:		r = ITOR(tos);								break;	
	case OpCode::ITOR2:		r = ITOR(--tos);							break;
	case OpCode::ROUND:		r = ROUND(tos);								break;
	case OpCode::TRUNC:		r = TRUNC(tos);								break;
	case OpCode::ABS:		r = ABS(tos);								break;
	case OpCode::ATAN:		r = ATAN(tos);								break;
	case OpCode::EXP: 		r = EXP(tos);								break;
	case OpCode::LOG: 		r = LOG(tos);								break;
	case OpCode::DUP:		push(*tos);									break;
	case OpCode::ODD:		r = ODD(tos);								break;
	case OpCode::PRED:		r = PRED(tos);								break;
	case OpCode::SIN:		r = SIN(tos);								break;
	case OpCode::SQR:		r = SQR(tos);								break;
	case OpCode::SQRT:		r = SQRT(tos);								break;
	case OpCode::SUCC:		r = SUCC(tos);								break;
	case OpCode::WRITE:		r = WRITE(tos);								break;
	case OpCode::WRITELN:	r = WRITELN(tos);							break;
	case OpCode::NEW:		r = NEW(tos);								break;
	case OpCode::DISPOSE:	r = DISPOSE(tos);							break;
	case OpCode::NEG:		r = NEG(tos);								break;
	case OpCode::ADD:		r = ADD();									break;
	case OpCode::SUB:		r = SUB();									break;
	case OpCode::MUL:		r = MUL();									break;
	case OpCode::DIV:		r = DIV();									break;
	case OpCode::REM:		r = REM();									break;
	case OpCode::LT:		r = LT();									break;
	case OpCode::LTE:   	r = LTE();									break;
	case OpCode::EQU:   	r = EQU();									break;
	case OpCode::GTE:  		r = GTE();									break;
	case OpCode::GT:		r = GT();									break;
	case OpCode::NEQU: 		r = NEQU();									break;
	case OpCode::LOR:  		r = LOR();									break;
	case OpCode::LAND: 		r = LAND();									break;
	case OpCode::LNOT:		*tos = !*tos;								break;
	case OpCode::POP:		r = POP();									break;
	case OpCode::PUSH: 		push(ir.addr);								break;
	case OpCode::PUSHVAR:	push(base(ir.level) + ir.addr.integer());	break;
	case OpCode::EVAL:		EVAL(ir.addr.natural());					break;
	case OpCode::ASSIGN:	ASSIGN(ir.addr.natural());					break;
	case OpCode::COPY:		COPY(ir.addr.natural());					break;
	case OpCode::CALL: 		CALL(ir.level, ir.addr.natural());			break;
	case OpCode::RET:   	RET();  									break;
	case OpCode::RETF: 		RETF();										break;
	case OpCode::ENTER:		sp+=ir.addr.natural(); 						break;
	case OpCode::JUMP:		pc = ir.addr.natural();						break;
	case OpCode::JNEQ:		r = JNEQ(tos);								break;
	case OpCode::LLIMIT:	r = LLIMIT(tos);							break;
	case OpCode::ULIMIT: 	r = ULIMIT(tos);							break;
	case OpCode::HALT:		return Result::halted;						break;

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
	if (verbose) {
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
	}

	if (status != Result::success && status != Result::halted)
		cerr << "runtime error @pc " << prevPc << ", sp: " << sp << ": " << status << endl;

	return status;
}

//public

/********************************************************************************************//**
 * Initialize the machine into a reset state with verbose == false. 
 *
 * @param stackSz	Size of the evaluation & call stack, in Datums.
 * @param fstoreSz	Size of the free store, in Datums.
 ************************************************************************************************/
PInterp::PInterp(unsigned stackSz, unsigned fstoreSz)
	:	stackSize{stackSz},
		stack(stackSize + fstoreSz, -1),
		heap(stackSz, fstoreSz),
		verbose(false),
		ncycles(0)
{
	reset();
}

/********************************************************************************************//**
 *	@param	prog	The program to run
 *	@param 	ver		True for verbose/debugging messages
 * 
 *  @return	The number of machine cycles run
 ************************************************************************************************/
PInterp::Result PInterp::operator()(const InstrVector& prog, bool ver) {
	verbose = ver;
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
 * @brief PInterp::Result stream put operator
 *
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

