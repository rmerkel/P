/****************************************************************************
 * @file subrange.cc
 ****************************************************************************/

#include <cassert>
#include <limits>
#include "type.h"

#include "results.h"
#include "subrange.h"

using namespace std;

// public

/********************************************************************************************//**
 * Maximum possible rnage for an interger
 ************************************************************************************************/
Subrange	Subrange::maxRange(numeric_limits<int>::min(), numeric_limits<int>::max());

/********************************************************************************************//**
 * @throws	Result::outOfRange if minimum > maximum
 * @param	minimum	The minimum value in the range
 * @param	maximum	The maximum value in the range
 ************************************************************************************************/
Subrange::Subrange(int minimum, int maximum) : _min{minimum}, _max{maximum} {
	if (_min > _max)
		throw Result::outOfRange;
}

/********************************************************************************************//**
 * @return my minmum value
 ************************************************************************************************/
int Subrange::minimum() const					{	return _min;		}

/********************************************************************************************//**
 * @return my maximum value
 ************************************************************************************************/
int Subrange::maximum() const					{	return _max;		}

/********************************************************************************************//**
 * @return the maximum() - minimum() + 1;
 ************************************************************************************************/
unsigned Subrange::span() const	{
	return maximum() - minimum() + 1;
}

// operators

/********************************************************************************************//**
 * @brief Subrange less-then operator
 * @param lhs	The left-hand side
 * @param rhs	The right-hand side
 * @return true if lhs == rhs
 ************************************************************************************************/
bool operator<(const Subrange& lhs, const Subrange& rhs) {
	if (lhs.minimum() < rhs.minimum())
		return true;
	else if (lhs.maximum() < rhs.minimum())
		return true;
	else
		return false;
}

/********************************************************************************************//**
 * @brief Subrange equality operator
 * @param lhs	The left-hand side
 * @param rhs	The right-hand side
 * @return true if lhs == rhs
 ************************************************************************************************/
bool operator==(const Subrange& lhs, const Subrange& rhs) {
	return	lhs.minimum() == rhs.minimum()	&&
			lhs.maximum() == rhs.maximum();
}

/********************************************************************************************//**
 * @brief Puts value on os
 * @param	os		The stream to write field's value on
 * @param	srange	Value to write 
 * @return	os
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const Subrange& srange) {
	return os << srange.minimum() << ".." << srange.maximum();
}

