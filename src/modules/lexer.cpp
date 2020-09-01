/**IT_START**/

// Reminder of variant syntax
// type Literal = variant {
//     String {
//         name: string;
//     }
//     Num {
//         val: s32;
//     }
// }

type Token = struct {
    text: string;
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

enum TokenKind {
    none,
    identifier,
    whitespace,
    breaking,
};

TokenKind kindOf(char c) {
    switch (c) {
    case ' ':
    case '\t':
        return whitespace;
    case '\n':
    case '(':
    case ')':
    case ';':
        return breaking;
    default:
        return identifier;
    }
}

ITBuffer* lex(const char* input) {
    std::vector<Token> tokens;
    int inputLen = strlen(input);
    int startIdx = 0;
    int line = 1;
    int column = 1;
    TokenKind curKind = none;
    auto addToken = [&](int idx) {
        int len = idx - startIdx;
        if (len <= 0) {
            return;
        }
        char* str = new char[len+1];
        strncpy(str, input+startIdx, len);
        str[len] = 0;
        tokens.push_back(Token(str, line, column));

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
