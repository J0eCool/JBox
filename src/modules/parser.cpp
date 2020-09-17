/**IT_START**/

type TokenKind = enum {
    Identifier,
    Number,
    Whitespace,
    Newline,
    Symbol,
}

type Token = struct {
    text: string;
    kind: TokenKind;
    line: s32;
    column: s32;
}

type SExpr = variant {
    Identifier {
        name: string;
    }
    List {
        elems: array(SExpr);
    }
}

import input {
    func log(SExpr);
}
export {
    func parse(array(Token)) -> SExpr;
    func roundtrip(SExpr);
}

/**IT_END**/

#include <string>
#include <vector>

SExpr* parse(Buffer<Token>* tokens) {
    return new List(new Buffer<SExpr*>(2, (SExpr*[]){
        new Identifier("wow"),
        new Identifier("dang"),
    }));
}

void roundtrip(SExpr* expr) {
    input::log(expr);
}
