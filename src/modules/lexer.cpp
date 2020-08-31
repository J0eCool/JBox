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

ITBuffer* lex(const char* input) {
    std::vector<Token> tokens;
    int inputLen = strlen(input);
    int start = 0;
    auto addToken = [&](int idx) {
        int len = idx - start;
        if (len <= 0) {
            return;
        }
        char* str = new char[len+1];
        strncpy(str, input+start, len);
        str[len] = 0;
        tokens.push_back(Token(str));
    };
    for (int i = 0; i < inputLen; ++i) {
        auto c = input[i];
        if (c == ' ') {
            addToken(i);
            start = i+1;
        } else if (c == '(' || c == ')') {
            addToken(i);
            start = i;
            addToken(i+1);
            start = i+1;
        }
    }
    addToken(inputLen);
    return new Buffer<Token>(tokens);
}
