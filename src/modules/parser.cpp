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
    func log(string);
}
export {
    func parse(array(Token)) -> SExpr;
    func prettyPrint(SExpr) -> SExpr;
}

/**IT_END**/

#include <string>
#include <vector>

class Parser {
    Buffer<Token>& tokens;
    int idx;

    Token& pop() {
        return tokens[idx++];
    }
    Token& peek() {
        return tokens[idx];
    }
    bool done() {
        return idx >= tokens.size();
    }

    List* fromVec(std::vector<SExpr*> elems) {
        return new List(new Buffer<SExpr*>(elems));
    }

    // parse newline-separated exprs
    List* parseBraceList() {
        std::vector<SExpr*> lines;
        std::vector<SExpr*> curLine;
        auto endLine = [&]() {
            List* list;
            if (curLine.size() == 1 && (list = curLine[0]->as_List())) {
                lines.push_back(list);
            } else if (curLine.size() > 0) {
                lines.push_back(fromVec(curLine));
            }
            curLine.clear();
        };
        while (!done()) {
            auto& tok = pop();
            if (tok.kind == TokenKind::Whitespace) {
                // just ignore whitespace entirely
                continue;
            }
            if (tok.kind == TokenKind::Newline) {
                // newlines end the line
                endLine();
                continue;
            }
            if (tok.kind == TokenKind::Symbol) {
                auto c = tok.text[0];
                if (c == '}') {
                    // end of a brace statement entirely
                    break;
                }
                if (c == '(') {
                    auto list = parseParenList();
                    curLine.push_back(list);
                    continue;
                }
                // TODO: error handling
            }
            // others are simple identifiers (for now)
            curLine.push_back(new Identifier(tok.text));
        }
        endLine();
        return fromVec(lines);
    }

    // parse newline-separated exprs
    List* parseParenList() {
        std::vector<SExpr*> exprs;
        while (!done()) {
            auto& tok = pop();
            if (tok.kind == TokenKind::Whitespace || tok.kind == TokenKind::Newline) {
                // just ignore whitespace entirely, including newlines
                continue;
            }
            if (tok.kind == TokenKind::Symbol) {
                auto c = tok.text[0];
                if (c == ')') {
                    // end of a paren statement entirely
                    break;
                }
                if (c == '(') {
                    auto list = parseParenList();
                    exprs.push_back(list);
                    continue;
                }
                // TODO: error handling
            }
            // others are simple identifiers (for now)
            exprs.push_back(new Identifier(tok.text));
        }
        return fromVec(exprs);
    }
public:
    Parser(Buffer<Token>& _tokens) : tokens(_tokens) {}

    SExpr* parse() {
        idx = 0;
        return parseBraceList();
    }
};

SExpr* parse(Buffer<Token>* tokens) {
    return Parser(*tokens).parse();
}

class Printer {
    std::string text;
    int indent;

    const std::string tab = "    ";

    void emit(SExpr* expr) {
        if (auto ident = expr->as_Identifier()) {
            text += ident->name;
        } else if (auto list = expr->as_List()) {
            text += "(";
            auto& elems = *list->elems;
            bool hasSubLists = false;
            for (int i = 0; i < elems.size(); ++i) {
                if (!hasSubLists && elems[i]->as_List()) {
                    hasSubLists = true;
                    indent++;
                }
                if (i > 0) {
                    if (!hasSubLists) {
                        text += " ";
                    } else {
                        emitNewline();
                    }
                }
                emit(elems[i]);
            }
            if (hasSubLists) {
                indent--;
                emitNewline();
            }
            text += ")";
        }
    }

    void emitNewline() {
        text += "\n";
        for (int i = 0; i < indent; ++i) {
            text += tab;
        }
    }
public:
    Printer(SExpr* expr) : text(""), indent(0) {
        if (auto list = expr->as_List()) {
            input::log("Toplevel list");
            // special-case a top-level list
            auto &elems = *list->elems;
            for (int i = 0; i < elems.size(); ++i) {
                emit(elems[i]);
                text += "\n";
            }
        } else {
            input::log("Toplevel elem");
            emit(expr);
        }
    }
    std::string str() {
        return text;
    }
};

SExpr* prettyPrint(SExpr* expr) {
    input::log(Printer(expr).str().c_str());
    return expr;
}
