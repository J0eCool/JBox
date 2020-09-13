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
    func log(string, s32);
}
export {
    func lex(string) -> array(Token);
}

/**IT_END**/

#include <string>
#include <vector>

enum CharKind {
    none,
    identifier,
    numeric,
    whitespace,
    breaking,
};

CharKind kindOf(char c) {
    switch (c) {
    case ' ':
    case '\t':
        return whitespace;
    case '\n':
    case '(':
    case ')':
    case ';':
        return breaking;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return numeric;
    default:
        return identifier;
    }
}

TokenKind tokenKind(const char* str) {
    if (str[0] == '\n') {
        return TokenKind::Newline;
    }
    auto charKind = kindOf(str[0]);
    switch (charKind) {
    case whitespace:
        return TokenKind::Whitespace;
    case numeric:
        return TokenKind::Number;
    case identifier:
    default:
        return TokenKind::Identifier;
    }
}

Buffer<Token>* lex(const char* input) {
    std::vector<Token> tokens;
    int inputLen = strlen(input);
    int startIdx = 0;
    int line = 1;
    int column = 1;
    CharKind curKind = none;
    auto addToken = [&](int idx) {
        int len = idx - startIdx;
        if (len <= 0) {
            return;
        }
        char* str = new char[len+1];
        strncpy(str, input+startIdx, len);
        str[len] = 0;
        auto kind = tokenKind(str);
        tokens.push_back(Token(str, kind, line, column));

        column += len;
        curKind = none;
    };
    for (int i = 0; i < inputLen; ++i) {
        auto c = input[i];
        auto kind = kindOf(c);
        if (curKind == none) {
            curKind = kind;
        } else if (curKind != kind) {
            addToken(i);
            startIdx = i;
            curKind = kind;
        }
        if (kind == breaking) {
            addToken(i+1);
            startIdx = i+1;
        }

        if (c == '\n') {
            column = 1;
            line++;
        }
    }
    addToken(inputLen);
    return new Buffer<Token>(tokens);
}
