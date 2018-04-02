/********************************************************************************************//**
 * @file subrange.h
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#ifndef	SUBRANGE_H
#define	SUBRANGE_H

#include <iostream>

/********************************************************************************************//**
 * Subrange - a contiguoush sub-range of integer values
 ************************************************************************************************/
class Subrange {
	int		_min;						///< The sub-ranges minimum value
	int		_max;						///< The Sub-ranges maximum value

public:
	static Subrange	maxRange;			///< Maximum possbile range

	Subrange() : _min{0}, _max{0} {}	///< Default constructor
	Subrange(int minimum, int maximum);	///< Constructor
	virtual ~Subrange() {}				///< Destructor

	int minimum() const;				///< Return type sub-range minimum value
	int maximum() const;				///< Return type sub-range maximum value
	unsigned span() const;				///< Return type sub-range's span
};	

bool operator<(const Subrange& lhs, const Subrange& rhs);
bool operator==(const Subrange& lhs, const Subrange& rhs);
std::ostream& operator<<(std::ostream& os, const Subrange& srange);

#endif

