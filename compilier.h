/********************************************************************************************//**
 * @file compilier.h
 *
 * A recursive decent compilier base.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#ifndef	COMPBASE_H
#define	COMPBASE_H

#include <set>
#include <string>
#include <utility>

#include "instr.h"
#include "datum.h"
#include "symbol.h"
#include "token.h"

/********************************************************************************************//**
 * Framework for a recursive decent compilier
 *
 * Language independent utilities for a recursive decent compilier.
 ************************************************************************************************/
class Compilier {
public:
	Compilier(const std::string& pName);	///< Constructor; use pName for error messages
	virtual ~Compilier() {}					///< Destructor

	/// Run the compiler
	unsigned operator()(const std::string& inFile, InstrVector& prog, bool verb = false);

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

	/// Name, kind pair
	struct NameKind {
		std::string		name;				///< Variable/parameter
		TDescPtr		type;				///< it's type descriptor

		/// Construct a name/kind pair
		NameKind(const std::string n, TDescPtr p) : name{n}, type{p} {}
	};

	/// A vector of name kind pairs
	typedef std::vector<NameKind>	NameKindVec;

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

	/// Emit an instruction...
	size_t emit(const OpCode op, int8_t level = 0, Datum addr = 0);

	/// Emit a variable reference, e.g., an absolute address...
	TDescPtr emitVarRef(int level, const SymValue& val);

	/// Create a listing...
	void listing(const std::string& name, std::istream& source, std::ostream& out);

	/// Purge symtbl of entries from a given block level
	void purge(int level);

	/// lookup identifier in the symbol table...
	SymbolTable::iterator lookup(const std::string& id);

	const std::string nameDecl(int level);	///< name (identifier) check...

	virtual void run() = 0;					///< Compile...
};

#endif
