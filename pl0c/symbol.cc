/**	@file	symbol.cc
 *
 *	@brief	The PL/0C Symbol table
 *
 *	Created by Randy Merkel on 6/7/2013.
 *  Copyright (c) 2016 Randy Merkel. All rights reserved.
 */

#include "symbol.h"

#include <cassert>

using namespace std;

// public static

string SymValue::toString(SymValue::Kind k) {
	switch(k) {
	case SymValue::none:		return "none";
	case SymValue::identifier:	return "identifier";
	case SymValue::constant:	return "constant";
	case SymValue::proc:		return "proc";
	default:
		assert(false);
	}
}
