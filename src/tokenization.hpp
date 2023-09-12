//
// Created by khusn on 9/8/2023.
//

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <unordered_set>

//TODO floating point division
enum class TokenType {
    exit,
    let,
    _if,            //TODO if else statement
    bool_lit,       //TODO bool literal
    int_lit,
    ident,
    semi,
    open_paren,
    close_paren,
    open_curly,
    close_curly,
    eq,
    plus,
    sub,
    mult,
    div,
    pow,            //TODO power operation
    less,           //TODO < comparison
    lesseq,         //TODO <= comparison
    greater,        //TODO > comparison
    greatereq,      //TODO >= comparison
    comp            //TODO == comparison
};

struct Token {
    TokenType type;
    std::optional<std::string> value {};
};

bool isOperatorSymbol(char c) {
    std::unordered_set<char> chars{'<', '>', '=', '!', '/', '*', '+', '-', '^', '%', '|', '&'};
    return chars.find(c) != chars.end();
}

class Tokenizer {
private:
    const std::string m_src;
    size_t m_index = 0;

public:
    inline explicit Tokenizer(std::string src)
            : m_src(std::move(src)) {}

    inline std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        std::string buf;
        while (peek().has_value()) {
            if (std::isalpha(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && std::isalnum(peek().value())) {
                    buf.push_back(consume());
                }
                if (buf == "exit") {
                    tokens.push_back({ .type = TokenType::exit });
                    buf.clear();
                }
                else if (buf == "let") {
                    tokens.push_back({ .type = TokenType::let });
                    buf.clear();
                }
//                else if (buf == "if") {
//                    tokens.push_back({ .type = TokenType::_if });
//                    buf.clear();
//                }
                else {
                    tokens.push_back({ .type = TokenType::ident, .value = buf });
                    buf.clear();
                }
            }
            else if (std::isdigit(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && std::isdigit(peek().value())) {
                    buf.push_back(consume());
                }
                tokens.push_back({ .type = TokenType::int_lit, .value = buf });
                buf.clear();
            }
            else if (isOperatorSymbol(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && isOperatorSymbol(peek().value())) {
                    buf.push_back(consume());
                }
                if (buf == "<=") {
                    tokens.push_back({ .type = TokenType::lesseq });
                    buf.clear();
                }
                else if (buf == ">=") {
                    tokens.push_back({ .type = TokenType::greatereq });
                    buf.clear();
                }
                else if (buf == "==") {
                    tokens.push_back({ .type = TokenType::comp });
                    buf.clear();
                }
                else if (buf == "=") {
                    tokens.push_back({ .type = TokenType::eq });
                    buf.clear();
                }
                else if (buf == "+") {
                    tokens.push_back({ .type = TokenType::plus });
                    buf.clear();
                }
                else if (buf == "-") {
                    tokens.push_back({ .type = TokenType::sub });
                    buf.clear();
                }
                else if (buf == "*") {
                    tokens.push_back({ .type = TokenType::mult });
                    buf.clear();
                }
                else if (buf == "/") {
                    tokens.push_back({ .type = TokenType::div });
                    buf.clear();
                }
                else {
                    //TODO logical operators, change and assign (+=) etc
                    std::cerr << "Unknown symbol" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            else if (peek().value() == '(') {
                consume();
                tokens.push_back({ .type = TokenType::open_paren });
            }
            else if (peek().value() == ')') {
                consume();
                tokens.push_back({ .type = TokenType::close_paren });
            }
            // TODO curly brackets
            // else if (peek().value() == '{') {
            //     consume();
            //     tokens.push_back({ .type = TokenType::open_curly });
            // }
            // else if (peek().value() == '}') {
            //     consume();
            //     tokens.push_back({ .type = TokenType::close_curly });
            // }
            else if (peek().value() == ';') {
                consume();
                tokens.push_back({ .type = TokenType::semi });
            }
            else if (std::isspace(peek().value())) {
                consume();
            }
            else {
                std::cerr << "Unknown symbol" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        m_index = 0;
        return tokens;
    }

private:
    [[nodiscard]] inline std::optional<char> peek(int offset = 0) const {
        if (m_index + offset >= m_src.length()) {
            return {};
        }
        else {
            return m_src.at(m_index + offset);
        }
    }

    inline char consume() {
        return m_src.at(m_index++);
    }
};