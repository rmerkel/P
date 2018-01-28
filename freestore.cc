/********************************************************************************************//**
 * @file freestore.cc
 *
 * class freeStore.
 ************************************************************************************************/

#include <cassert>
#include "freestore.h"

using namespace std;

/********************************************************************************************//**
 * class FreeStore::Block
 ************************************************************************************************/

/********************************************************************************************//**
 * Default constructor; zero blocks, starting at address zero
 ************************************************************************************************/
FreeStore::Block::Block() : addr{ 0 }, size{ 0 } {}

/********************************************************************************************//**
 * @param	addr	Starting address of the free list
 * @param	size	Lenght of the free list, in daumes
 ************************************************************************************************/
FreeStore::Block::Block(size_t _addr, size_t _size) : addr{_addr}, size{_size} {}

/********************************************************************************************//**
 * class FreeStore, public
 ************************************************************************************************/

/********************************************************************************************//**
 * @param	addr	The starting address of the free list
 * @param	size	The length of the free list, in Datums
 ************************************************************************************************/
FreeStore::FreeStore(size_t addr, size_t size)
	: initAddr{addr}, initSize{size}, freeStore{ { addr, size } } {
}

/********************************************************************************************//**
 * @return My initial starting address
 ************************************************************************************************/
size_t FreeStore::addr() const					{	return initAddr;	}

/********************************************************************************************//**
 * @return my initial free list size, in Datums
 ************************************************************************************************/
size_t FreeStore::size() const					{	return initSize;	}

/********************************************************************************************//**
 * @param	size	Number of Datums's to allocate
 * @return The starting address of the allocated block, or zero if insufficient free-space.
 ************************************************************************************************/
unsigned FreeStore::alloc(size_t size) {
   	Block result(0, 0);

   	FreeStoreMap::iterator ri = freeStore.end();
   	for (FreeStoreMap::iterator i = freeStore.begin(); i != freeStore.end(); ++i) {
       	if (i->second >= size) {           	// big enough?
           	if (ri == freeStore.end())
               	ri = i;                 	// 1st match
           	else if (i->second - size < ri->second - size)
               	ri = i;                 	// better match
       	}
   	}

   	if (ri != freeStore.end()) {
       	result.addr = ri->first; 
       	result.size = ri->second;
       	freeStore.erase(ri);            // Remove the block from free-store        

       	// split the block?
       	if (result.addr != 0 && result.size != size) {
           	Block fragment(result.addr + size, result.size - size);
           	result.size = size;
           	auto j = freeStore.insert({ fragment.addr, fragment.size });
           	assert(j.second);
       	}

       	// Mark the block as in-use...
       	auto j = allocated.insert({ result.addr, result.size });
       	assert(j.second);
  	}

   	return result.addr;
}

/********************************************************************************************//**
 * @param	addr	The starting address of the block to return to the free store
 * @return	true if the block identified by addr is valid, i.e., it was returned from alloc().
 ************************************************************************************************/
bool FreeStore::free(unsigned addr) {
   	auto i = allocated.find(addr);				// lookup the bock in the allocated list...

	if (i == allocated.end())
		return false;							// Proceed only if the block is valid

   	else {										// Move block from allocated to the free list...
       	Block block(i->first, i->second);
       	allocated.erase(i);
       	auto j = freeStore.insert({ block.addr, block.size });
       	assert(j.second);

       	// Merge with the preceeding and/or following blocks if adjacent...
       	if (j.first != freeStore.begin() && j.first != freeStore.end()) {
           	auto prev = j.first;
           	--prev;

			// Merge with the previous block?
           	if (prev->first + prev->second == block.addr) {
               	Block newBlk(prev->first, prev->second + block.size);
               	freeStore.erase(prev);
               	freeStore.erase(j.first);
               	j = freeStore.insert({ newBlk.addr, newBlk.size });
           	}
       	}

       	if (j.first != freeStore.end()) {		// Merge with the folowing block?
           	auto next = j.first;
           	++next;
           	if (next != freeStore.end() && block.addr + block.size == next->first) {
               	Block newBlk(block.addr, block.size + next->second);
               	freeStore.erase(j.first);
               	freeStore.erase(next);
               	freeStore.insert({ newBlk.addr, newBlk.size });
           	}
       	}
   	}

	return true;
}

/********************************************************************************************//**
 * @param	os	Stream to write the report on.
 ************************************************************************************************/
void FreeStore::dump(ostream& os) const {
   	os << "Free store: {";
   	for (auto blk : freeStore) {
       	os << "{" << hex << blk.first << ", " << dec << blk.second << "}, ";
   	}
   	os << "}\n";

   	os << "Allocated:  {";
   	for (auto blk : allocated)
       	os << "{" << hex << blk.first << ", " << dec << blk.second << "}, ";
   	os << "}\n";
}

