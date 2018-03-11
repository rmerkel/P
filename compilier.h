/********************************************************************************************//**
 * @file compilier.h
 *
 * A recursive decent compilier framework.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#ifndef	COMPBASE_H
#define	COMPBASE_H

#include <iostream>
#include <set>
#include <string>
#include <utility>

#include "instr.h"
#include "datum.h"
#include "symbol.h"
#include "token.h"

/************************************************************************************************
 * LogLevel
 ************************************************************************************************/

/// Control the log index level
struct LogLevel {
	static unsigned n;						///< indent level
	LogLevel();								///< Enter the next level
	~LogLevel();							///< Exit the previous level
};

std::ostream& LogIndex(std::ostream& oss);	///< Indent per the current log level...

/********************************************************************************************//**
 * Framework for a recursive decent compilier
 ************************************************************************************************/
class Compilier {
public:
	Compilier();							///< Constructor
	virtual ~Compilier() {}					///< Destructor

	/// Compile a P source file
	unsigned operator()(
		const	std::string&	fName,
				InstrVector&	instructions,
				bool			lst,
				bool			ver);

protected:
	/// A table, indexed by instruction address, yeilding source line numbers
	typedef std::vector<unsigned> SourceIndex;

	std::string			progName;			///< The compilier's name, used in error messages
	unsigned			nErrors;			///< Total # of compilier errors
	bool				verbose;			///< Dump debugging information if true
	TokenStream			ts;					///< The input token stream (the source)
	SymbolTable			symtbl;				///< Symbol table
	InstrVector*		code;				///< Emitted code
	SourceIndex			indextbl;			///< Source cross-index for listings

	void error(const std::string& msg);		///< Write an error message...

	/// Write an error message...
	void error(const std::string& msg, const std::string& name);

	/// Return the current token kind..
	Token::Kind current() 					{	return ts.current().kind;	}

	Token next();							///< Read and return the next token...

	/// Accept the next token if it's a k...
	bool accept(Token::Kind k, bool get = true);

	/// Expect the next token to be a k...
	bool expect(Token::Kind k, bool get = true);

	bool oneOf(Token::KindSet set);			///< Is the current token one of the given set?

	size_t emit(const OpCode op);			///< Emit an instruction

	/// Emit an instruction...
	template <class T> size_t emit(const OpCode op, int8_t level, const T& addr);

	/// Emit a variable reference, e.g., an absolute address...
	TDescPtr emitVarRef(int level, const SymValue& val);

	/// Create a listing...
	void listing(std::istream& source, std::ostream& out);

	/// Purge symtbl of entries from a given block level
	void purge(int level);

	/// lookup identifier in the symbol table...
	SymbolTable::iterator lookup(const std::string& id);

	/// name (identifier) check...
	const std::string nameDecl(int level, const std::string& prefix = "");

	virtual void run() = 0;					///< Compile...
};

/********************************************************************************************//**
 * Assembles op, level, addr into a new instruction, and then appends the instruciton on
 * the end of code[], returning it's address/index.
 *
 * Side effect; updates the cross index for the listing.
 *
 * @param	op		The instruction operation code
 * @param	level	The instruction block level value. Defaults to zero.
 * @param	addr	The instructions address/value. Defaults to zero.
 *
 * @return The address (code[] index) of the new instruction.
 ************************************************************************************************/
template <class T> size_t Compilier::emit(const OpCode op, int8_t level, const T& addr) {
	const int lvl = static_cast<int>(level);
	if (verbose)
		std::cout
			<< progName << ": emitting " << code->size() << ": "
			<< OpCodeInfo::info(op).name() << " " << lvl << ", "
			<< addr << "\n";

	code->push_back({op, level, Datum(addr)});
	indextbl.push_back(ts.lineNum);			// update the cross index

	return code->size() - 1;				// so it's the address of just emitted instruction
}

#endif
