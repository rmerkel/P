/********************************************************************************************//**
 * @file compilier.cc
 *
 * class Compilier implementation.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#include "compilier.h"
#include "interp.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>
#include <vector>

using namespace std;

/************************************************************************************************
 * LogLevel
 ************************************************************************************************/

LogLevel::LogLevel() {
	if (n < numeric_limits<unsigned>::max())
		++n;
}

LogLevel::~LogLevel() {
	assert(n > 0);
	if (n > 0)
		--n;
}

unsigned LogLevel::n = 0;

/// Indent the log level...
ostream& LogIndex(ostream& oss) {
	if (LogLevel::n > 0)
		oss << setw(LogLevel::n) << ' ';
	return oss;
}

/************************************************************************************************
 * class Compilier
 ************************************************************************************************/

// protected:

/********************************************************************************************//**
 * Write a diagnostic on standard error output, incrementing the error count.
 * @param msg The error message
 ************************************************************************************************/
void Compilier::error(const std::string& msg) {
	cerr << progName << ": " << msg << " near line " << ts.lineNum << endl;
	++nErrors;
}

/********************************************************************************************//**
 * Write a diagnostic in the form "msg 'name'", on standard error output, incrementing the error
 * count.
 * @param msg The error message
 * @param name Parameter to msg.
 ************************************************************************************************/
void Compilier::error(const std::string& msg, const std::string& name) {
	error (msg + " '" + name + "'");
}

/********************************************************************************************//**
 * @return The next token from the token stream
 ************************************************************************************************/
Token Compilier::next() {
	const Token t = ts.get();

	if (Token::Unknown == t.kind) {
		ostringstream oss;
		oss << "Unknown token: '" << t.string_value << "', (0x" << hex << t.integer_value << ")";
		error(oss.str());
		return next();
	}

	if (verbose)
		cout	<< progName			<< ": "
				<< "getting '"		<< t.kind << "', "
				<< t.string_value	<< ", "
				<< t.integer_value	<< "\n";

	return t;
}

/********************************************************************************************//**
 * Returns true, consuming the current token, if the current tokens "kind" is equal to kind.
 *
 * @param	kind	The token Kind to accept.
 * @param	get		Consume the token, if true. Defaults true.
 *
 * @return	true if the current token is a kind.
 ************************************************************************************************/
bool Compilier::accept(Token::Kind kind, bool get) {
	if (current() == kind) {
		if (get) next();			// consume the token
		return true;
	}

	return false;
}

/********************************************************************************************//**
 * Evaluate and return accept(kind, get). Generate an error if accept() returns false.
 *
 * @param	kind	The token Kind to accept.
 * @param	get		Consume the token, if true. Defaults true.
 *
 * @return	true if the current token is a k.
 ************************************************************************************************/
bool Compilier::expect(Token::Kind kind, bool get) {
	if (accept(kind, get)) return true;

	ostringstream oss;
	oss << "expected '" << kind << "' got '" << current() << "'";
	error(oss.str());
	return false;
}

/********************************************************************************************//**
 * @param set	Token kind set to test
 * @return true if current() is a member of set.
 ************************************************************************************************/
bool Compilier::oneOf(Token::KindSet set) {
	return set.end() != set.find(current());
}

/********************************************************************************************//**
 * Assembles op, 0, 0 into a new instruction, and then appends the instruciton on
 * the end of code[], returning it's address/index.
 *
 * Side effect; updates the cross index for the listing.
 *
 * @param	op		The instruction operation code
 *
 * @return The address (code[] index) of the new instruction.
 ************************************************************************************************/
size_t Compilier::emit(const OpCode op) {
	return emit(op, 0, Datum(0));
}

/********************************************************************************************//**
 * Local variables have an offset from the *end* of the current stack frame
 * (bp), while parameters have a negative offset from the *start* of the frame
 *  -- offset locals by the size of the activation frame.
 *
 *  @param	level	The current block level
 *  @param	val		The variable symbol table entry
 *  @return			Data type reference
 ************************************************************************************************/
TDescPtr Compilier::emitVarRef(int level, const SymValue& val) {
	const auto offset = val.value().integer() >= 0			?
						val.value().integer() + FrameSize	:	// local
						val.value().integer();					// parameter

	emit(OpCode::PUSHVAR, level - val.level(), offset);
	return TypeDesc::newPointerDesc(val.type());
}

/********************************************************************************************//**
 * Uses the cross index to write a listing on the output stream
 *
 * @param source 	The source file stream
 * @param out		The listing file stream
 ************************************************************************************************/
 void Compilier::listing(istream& source, ostream& out) {
	string 		line;   						// Current source line
	unsigned 	linenum = 1;					// source line number
	unsigned	addr = 0;						// code address (index)

	while (addr < indextbl.size()) {			// dump instructions, if any...
		while (linenum <= indextbl[addr]) {		// Print lines that lead up to code[addr]...
			getline(source, line);
			cout << "# " << progName << ", " << linenum++ << ": " << line << "\n";
		}

		disasm(out, addr, (*code)[addr]);		// Disasmble resulting instructions...
		while (linenum-1 == indextbl[++addr])
			disasm(out, addr, (*code)[addr]);
	}

	while (getline(source, line))				// Any lines following '.' ...
		cout << "# " << progName << ", " << linenum++ << ": " << line << "\n";

	out << endl;
}

/********************************************************************************************//**
 * @param level  The block level
 ************************************************************************************************/
void Compilier::purge(int level) {
	for (auto i = symtbl.begin(); i != symtbl.end(); ) {
		if (i->second.level() == level) {
			if (verbose)
				cout	<< progName << ": "
						<< LogIndex << "purging "
						<< i->first << ": "
						<< i->second.kind() << ", "
						<< static_cast<int>(i->second.level()) << ", "
						<< i->second.value()
						<< " from the symbol table\n";
			i = symtbl.erase(i);

		} else
			++i;
	}
}


/********************************************************************************************//**
 * return the 'closest' (highest block level) identifer...
 *
 * @param	id	identifier to look up in the symbol table
 * @return symtbl.end() or an iterator positioned at a symbol table entry.
 ************************************************************************************************/
SymbolTable::iterator Compilier::lookup(const string& id) {
	auto range = symtbl.equal_range(id);
	if (range.first == range.second) {
		error("Undefined identifier", id);
		return symtbl.end();

	} else {										// Find the closest...
		auto closest = range.first;
		for (auto it = closest; it != range.second; ++it)
			if (it->second.level() > closest->second.level())
				closest = it;

		return closest;
	}
}

/********************************************************************************************//**
 * If next token is an identifer, checks to see, and reports, if the identifier was previously 
 * defined in the current scope (level), and returns the identifer string. Returns "unknown" if
 * the next token is not an identifer.
 *
 * The prefixed identifier is prefix.identifier if prefix != "", otherwise it's the same as
 * the identifier. The decorated identifier is what is looked up in the symbol table, but
 * the undecroated version is returned. Prefixed identifiers are for record elements, so they
 * can't collide with variable, type or procedure names.
 *
 * @param 	level	The current block level.
 * @param	prefix	The identifier prefix.
 * @return 	the next, undecorated, identifier in the token stream, "unknown" if the next token
 * 			 wasn't an identifier.
 ************************************************************************************************/
const std::string Compilier::nameDecl(int level, const string& prefix) {
	const string id = ts.current().string_value;	// Copy the identifer before consuming it
	const string prefixed = prefix.empty()	? id : prefix + string(".") + id;

	if (expect(Token::Identifier)) {				// Consume the identifier
		auto range = symtbl.equal_range(prefixed);	// Already defined?
		for (auto it = range.first; it != range.second; ++it) {
			if (it->second.level() == level) {
				error("previously was defined", prefixed);
				break;
			}
		}

		return prefixed;
	}

	return "unknown";
}

/************************************************************************************************
 * public:
 ************************************************************************************************/

/********************************************************************************************//**
 * Construct a new compilier with the token stream initially bound to std::cin.
 ************************************************************************************************/
Compilier::Compilier() : nErrors{0}, verbose {false}, ts{cin} {}

/********************************************************************************************//**
 * Compile the contents of fName, generating code in prog.
 *
 * @param	fName			The source file name, where "-" means the standard input stream
 * @param	instructions	The generated machine code is appended here
 * @param	lst				Write listing on standard output.
 * @param	ver				Run in verbose mode if true
 *
 * @return	The number of errors encountered
 ************************************************************************************************/
unsigned Compilier::operator()(
	const	string&			fName,
			InstrVector&	instructions,
			bool			lst,
			bool			ver)
{
	progName = fName;
	code = &instructions;
	verbose = ver;

	if ("-" == fName)  {					// "-" means standard input
		ts.set_input(cin);
		run();

		// Just disasmemble as we can't rewind standard input!
		for (unsigned loc = 0; loc < code->size(); ++loc)
			disasm(cout, loc, (*code)[loc]);

	} else {
		ifstream ifile(fName);
		if (!ifile.is_open())
			error("error opening source file", fName);

		else {
			ts.set_input(ifile);
			run();

			ifile.close();					// Rewind the source (seekg(0) isn't working!)...
			if (lst) {
				ifile.open(fName);
				listing(ifile, cout);	// 	create a listing...
			}
		}
	}

	return nErrors;
}

