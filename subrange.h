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
	int	minimum;						///< The sub-ranges minimum value
	int	maximum;						///< The Sub-ranges maximum value

public:
	Subrange(int min = 0, int max = 0);	///< Constructor
	virtual ~Subrange() {}				///< Destructor

	int min() const;					///< Return type sub-range minimum value
	int max() const;					///< Return type sub-range maximum value
	unsigned span() const;				///< Return type sub-range's span
};	

bool operator<(const Subrange& lhs, const Subrange& rhs);
bool operator==(const Subrange& lhs, const Subrange& rhs);
std::ostream& operator<<(std::ostream& os, const Subrange& srange);

#endif

