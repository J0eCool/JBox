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

import input {
    func log(s32, Token);
}
export {
    func parse(array(Token)) -> array(string);
}

/**IT_END**/

#include <string>
#include <vector>

Buffer<const char*>* parse(Buffer<Token> *tokens) {
    input::log(tokens->size(), nullptr);
    for (int i = 0; i < tokens->size(); ++i) {
        input::log(i, &(tokens->ref(i)));
    }
    return new Buffer<const char*>(2, (const char*[]) { "Wow", "jeex" });
}
