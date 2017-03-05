/**	@file	symbol.cc
 *
 * The PL/0C Symbol table
 * 
 * @author Randy Merkel, Slowly but Surly Software. 
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
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
	case SymValue::function:	return "function";
	default:
		assert(false);
	}
}
