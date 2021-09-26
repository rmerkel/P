# The P grammer

                  program = "program" ident param-lst "is" block-decl "endprog" ;
               block-decl = [ const-decl-lst ';' ]
                            [ var-decl-blk ';'   ]
                            [ sub-decl-lst ';'   ]
                            "begin" stmt-lst "end" ;
           const-decl-lst = "const" const-decl { ';' const-decl } ;
               const-decl = ident '=' const-expr ;
            type-decl-lst = type-decl { ';' type-decl } ;
                type-decl = "type" "is" type ;
             var-decl-blk = "var" var-decl-lst ;
             var-decl-lst = var-decl { ';' var-decl } ;
                 var-decl = ident-lst : type ;
                     type = simple-type | structured-type | pointer-type ;
              simple-type = "real" | ordinal-type ;
         ordinal-type-lst = ordinal-type { ',' ordinal-type } ;
             ordinal-type = '(' ident-lst ')' | const-expr ".." const-expr |
                            "boolean" | "integer" | "natual" | "positive" | "character" ;
          structured-type = "array" '[' ordinal-type-lst ']' "of" type |
                            "record" field-lst "end" ;
                field-lst = var-decl-lst ;
             pointer-type = '^' ident ;
             sub-decl-lst = func-decl | proc-decl ;
                proc-decl = "procedure" ident param-lst "is" block-decl "endproc" ;
                func-decl = "function"  ident param-lst ':' type "is" block-decl "endfunc" ;
                param-lst = '(' [ param-decl-lst ] ')' ;
           param-decl-lst = param-decl { ';' param-decl } ;
               param-decl = [ "var" ] ident-lst : type ;
                ident-lst = ident { ',' ident } ;
                 stmt-lst = stmt {';' stmt }  ;
                  if-stmt = "if" expr "then" stmt-lst
                               { "elif" expr "then" stmt-lst }
                               [ "else" stmt-lst ]
                               "endif" ;
               while-stmt = "while" expr "loop" stmt-lst "endloop" ;
                 for-stmt = "for" ident "in" [ "reverse" ] ordinal-type
                               "loop" stmt "endloop" ;
              return-stmt = "return" [ expr ] ;
                     stmt = [  variable ':=' expr                   |
                               ident '(' [ expr-lst ] ')'           |
                               if-stmt                              |
                               while-stmt                           |
                               "repeat" stmt "until" expr endloop   |
                               for-stmt                             |
                               return-stmt ] ;
               const-expr = [ '+' | '-' ] number | ident | character | string ;
               const-expr = const-simple-expr { expr-op const-simple-expr } ;
        const-simple-expr = [ simple-expr-pre] const-term { simple-expr-op const-term } ;
               const-term = const-fact { term-op  const-fact } ;
               const-fact = ident | attr-fact | number | '(' const-expr ')' ;
                 expr-lst = expr { ',' expr } ;
                     expr = simple-expr { expr-op simple-expr } ;
                  expr-op = '<' | "<=" | '=' | ">=" | '>' | "<>" simple-expr ;
              simple-expr = [ simple-expr-pre ] terminal { simple-expr-op terminal } ;
          simple-expr-pre = '+' | '-' | "bit_not" ;
           simple-expr-op = '+' | '-' | "bit_or" | "bit_xor" | "bit_sleft" | "bit_sright" | "or' ;
                 terminal = fact { term-op  fact } ;
                  term-op = '*' | '/' | "rem" | "bit_and" | "and" ;
                     fact = variable | attr-fact | character | string |
                            ident '(' [ expr-lst ] ')'              |
                            "round" '(' expr ')'                    |
                            number                                  |
                            '(' expr ')' ;
                 variable = ident [ composite-desc { composite-desc } ] ;
           composite-desc = '[' expr-lst ']'                        |
                            '.' ident                               |
                            '^' ident ;
                attr-fact = ident '`' "min" | "max" ;
                character = nul | sox | .. | '}' | '`' | del ;
                   string = '"' { character } '"' ;
                   number = whole-number | floting-point-number ;
                   letter = "A" | "B" | "C" | ... | "Y" | "Z" |
                            "a" | "b" | "c" | ... | "y" | "z" ;
                    digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
                    ident = [ '_' | letter ] { letter | digit | '_' } ;

## Key

         {} Repeat zero or more times
         [] Optional; zero or *one* times
         () Grouping
          | One of ...
          ; End of production

See [EBNF](https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form) for more information.

