/********************************************************************************************//**
 * @file results.cc
 *
 * P Programming Language results
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#include <cassert>
#include "results.h"

using namespace std;

// operators

/********************************************************************************************//**
 * Write Result value on os
 *
 * @param	os		Stream to write value to 
 * @param	result	The value to write 
 * @return	os 
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const Result result) {
	switch (result) {
	case Result::success:			os << "success";				break;
	case Result::divideByZero:		os << "divide-by-zero";			break;
	case Result::badFetch:			os << "bad fetch";				break;
	case Result::badDataType:		os << "bad data type";			break;
	case Result::unknownInstr:		os << "unknown instruction";	break;
	case Result::stackOverflow:		os << "stack overflow";			break;
	case Result::stackUnderflow:	os << "stack underflow";		break;
	case Result::freeStoreError:	os << "free-store error";		break;
	case Result::outOfRange:		os << "out-of-range";			break;
	case Result::illegalOp:			os << "illegal operation";		break;
	case Result::halted:			os << "halted";					break;
	default:
		return os << "undefined result!";
		assert(false);
	}

	return os;
}

