/** @file pl0com.cpp
 *
 * A port of the pl/0 compilier to C++.
 *
 * Grammer (EBNF)
 * --------------
 * prog =	block "." ;
 * 
 * block = 	[ "const" ident "=" number {"," ident "=" number} ";"]
 *         	[ "var" ident {"," ident} ";"]
 *         	{ "procedure" ident ";" block ";" }
 *          "begin" stmt {";" stmt } "end" 
 * 
 * stmt = 	[ ident ":=" expr
 * 		  	| "call" ident 
 *          | "?" ident 
 * 		  	| "!" expr 
 *          | "begin" stmt {";" stmt } "end" 
 *          | "if" cond "then" stmt { "else" stmt }
 *          | "while" cond "do" stmt ] ;
 * 
 * cond = 	"odd" expr
 * 		  	| expr ("="|"#"|"<"|"<="|">"|">=") expr ;
 * 
 * expr = 	[ "+"|"-"] term { ("+"|"-") term } ;
 * 
 * term = 	fact {("*"|"/") fact} ;
 * 
 * fact = 	ident | number | "(" expr ")" ;
 * 
 * Key
 * 	- {}	zero or more times
 * 	- []	zero or one times
 *
 * The orignal PL/0 grammer was 'improved' by requiring a begin..end in a block,
 * and adding if ... else.
 */


///
typedef enum {ident, number, lparen, rparen, times, slash, plus,
    minus, eql, neq, lss, leq, gtr, geq, callsym, beginsym, semicolon,
    endsym, ifsym, elsesym, whilesym, becomes, thensym, dosym, constsym, comma,
    varsym, procsym, period, oddsym} Symbol;

Symbol sym;
void nextsym(void);
void error(const char msg[]);

/// Return true and consume the current symbol if s is the current symbol
static bool accept(Symbol s) {
    if (sym == s) {
        nextsym();
        return true;
    }
    return false;
}

///	Return accept(s), emitting an error if accept(s) fails.
static bool expect(Symbol s) {
    if (accept(s))
        return true;
    error("expect: unexpected symbol");
    return false;
}

///
static void fact(void) {
    if (accept(ident)) {					// identifier; loa identifer
        ;

    } else if (accept(number)) {			// number; lit number
        ;

    } else if (accept(lparen)) {			// { expr }
        expr();
        expect(rparen);

    } else {
        error("fact: syntax error");
        nextsym();
    }
}

///
static void term(void) {
    fact();
    while (sym == times || sym == slash) {
        nextsym();
        fact();
    }
}

/// Expressions
static void expr(void) {
    if (sym == plus || sym == minus)
        nextsym();					// unary +-

    term();
    while (sym == plus || sym == minus) {
        nextsym();
        term();
    }
}

/// Conditions...
static void cond(void) {
    if (accept(oddsym)) {
        expr();

    } else {
        expr();
        if (sym == eql || sym == neq || sym == lss || sym == leq || sym == gtr || sym == geq) {
            nextsym();
            expr();
        } else {
            error("cond: invalid operator");
            nextsym();
        }
    }
}

/// Statements
static void stmt(void) {
    if (accept(ident)) {			// assignment
        expect(becomes);
        expr();

    } else if (accept(callsym)) {	// procedure call
        expect(ident);

    } else if (accept(beginsym)) {	// begin ... end
        do {
            stmt();
        } while (accept(semicolon));
        expect(endsym);

    } else if (accept(ifsym)) {		// if cond...
        cond();
        expect(thensym);
        stmt();
		if (accept(elsesym))		// 	else stmt...
			stmt();

    } else if (accept(whilesym)) {	// while cond...
        cond();
        expect(dosym);
        stmt();

    } else {
        error("stmt: syntax error");
        nextsym();
    }
}

/// Blocks
static void block(void) {
    if (accept(constsym)) {			// const ident = number, ... 
        do {
            expect(ident);
            expect(eql);
            expect(number);
        } while (accept(comma));
        expect(semicolon);
    }

    if (accept(varsym)) {			// var ident, ... 
        do {
            expect(ident);
        } while (accept(comma));
        expect(semicolon);
    }

    while (accept(procsym)) {		// procedure ident; ... 
        expect(ident);
        expect(semicolon);
        block();
        expect(semicolon);
    }

    accept(beginsym);				// begin ... end
    do {
    	stmt();
    } while (accept(semicolon));
    expect(endsym);
}

/// Program
void prog(void) {
    nextsym();
    block();
    expect(period);
}

