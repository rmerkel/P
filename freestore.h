/********************************************************************************************//**
 * @file freestore.h
 *
 * class FreeStore, a dynmaic memory manager.
 ************************************************************************************************/

#ifndef	FREESTORE_H
#define	FREESTORE_H

#include <ostream>
#include <map>

/********************************************************************************************//**
 * A dynamic memory manager.
 *
 * Creates a free store arena, maintaining a free and an in-use block list. Allocation is
 * best-fit, and blocks are split to requested size. Blocks are automatically merged when freed.
 ************************************************************************************************/
class FreeStore {
	/// A block range; its starting address and number of Datums
	struct Block {
    	size_t  addr;						///< Address of the block
    	size_t  size;						///< Length of the block in datums

    	Block();
    	Block(size_t _addr, size_t _size);
    	virtual ~Block() {}
	};

	/// Map blocks by starging address
	typedef std::map<size_t, size_t> FreeStoreMap;

	size_t			initAddr;				///< Free store starting address
	size_t			initSize;				///< Free store maximum size
	FreeStoreMap	freeStore;				///< Free block list
	FreeStoreMap	allocated;				///< Allocated block list

public:
	FreeStore(size_t addr, size_t size);	///< Construct a free store arena
	virtual ~FreeStore() {}					///< Destructor

	size_t addr() const;					///< Return the base address of the arena
	size_t size() const;					///< Return the size of the arena, in Datum's
	
	size_t alloc(size_t size);			///< Allocate a block of Datum's from the free list
	bool free(unsigned addr);				///< Return a previously allocated block to free list

	void dump(std::ostream& os) const;		///< Write a free/allocated list resport
};

#endif

