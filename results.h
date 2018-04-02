/********************************************************************************************//**
 * @file results.h
 *
 * P Programming Language operation results
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ********************************************************************************************//**/

#ifndef	RESULTS_H
#define RESULTS_H

#include <iostream>

/********************************************************************************************//**
 * Results
 ********************************************************************************************//**/

enum class Result {
	success,								///< No error
	divideByZero,							///< Divide by zero
	badFetch,								///< Attempt to fetch uninitialized code
	badDataType,							///< Wrong data type for operation
	unknownInstr,							///< Attempt to execute an undefined instruction
	stackOverflow,							///< Attempt to access beyound the end of the statck
	stackUnderflow,							///< Attempt to access an empty stack
	freeStoreError,							///< Allocation or free error
	outOfRange,								///< Attempt to index object with out-of-range index
	illegalOp,								///< Illegal operation
	halted									///< Machine has halted
};

/********************************************************************************************//**
 * PInterp::Result stream put operator
 ************************************************************************************************/
std::ostream& operator<<(std::ostream& os, Result result);

#endif

