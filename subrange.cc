/****************************************************************************
 * @file subrange.cc
 ****************************************************************************/

#include <cassert>
#include "type.h"

#include "results.h"
#include "subrange.h"

using namespace std;

// public

/********************************************************************************************//**
 * @throws	Result::outOfRange if minimum > maximum
 *
 * @param	min	The minimum value in the range
 * @param	max	The maximum value in the range
 ************************************************************************************************/
Subrange::Subrange(int min, int max) : minimum{min}, maximum{max} {
	if (minimum > maximum)
		throw Result::outOfRange;
}

/********************************************************************************************//**
 * @return my minmum value
 ************************************************************************************************/
int Subrange::min() const						{	return minimum;		}

/********************************************************************************************//**
 * @return my maximum value
 ************************************************************************************************/
int Subrange::max() const						{	return maximum;		}

/********************************************************************************************//**
 * @return the max() - min() + 1;
 ************************************************************************************************/
unsigned Subrange::span() const	{
	return max() - min() + 1;
}

// operators

/********************************************************************************************//**
 * @brief Subrange less-then operator
 * @param lhs	The left-hand side
 * @param rhs	The right-hand side
 * @return true if lhs == rhs
 ************************************************************************************************/
bool operator<(const Subrange& lhs, const Subrange& rhs) {
	if (lhs.min() < rhs.min())
		return true;
	else if (lhs.max() < rhs.max())
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
	return	lhs.min() == rhs.min() && lhs.max() == rhs.max();
}

/********************************************************************************************//**
 * @brief Puts value on os
 * @param	os		The stream to write field's value on
 * @param	srange	Value to write 
 * @return	os
 ************************************************************************************************/
ostream& operator<<(std::ostream& os, const Subrange& srange) {
	return os << srange.min() << ".." << srange.max();
}

