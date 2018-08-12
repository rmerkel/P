/**
 */

#include "streams.h"

#include <iostream>
#include <fstream>
#include <map>

using namespace std;

typedef Stream<ios>	FileStream;


/// A table of fstreams, identified by a file descriptor (integer)
typedef map<int, FileStream>	StreamTable;

/// A table of streams, initially populated with stdin, stdout and stderr
StreamTable	strtbl = {
	{ 0, FileStream{cin}	},		/// 0 = standard input
	{ 1, FileStream{cout}	},		/// 1 = standard output
	{ 2, FileStream{cerr}	}		/// 2 = standard error 
};
