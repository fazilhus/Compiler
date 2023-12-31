//
// Created by khusn on 9/8/2023.
//

#pragma once

#include <cassert>
#include <variant>

#include "./arena.hpp"
#include "tokenization.hpp"

namespace node {

    struct TermIntLit {
        Token int_lit;
    };

    struct TermIdent {
        Token ident;
    };

    struct Expr;

    struct TermParen {
        Expr* expr;
    };

    struct BinExprAdd {
        Expr* lhs;
        Expr* rhs;
    };

    struct BinExprMult {
        Expr* lhs;
        Expr* rhs;
    };

    struct BinExprSub {
        Expr* lhs;
        Expr* rhs;
    };

    struct BinExprDiv {
        Expr* lhs;
        Expr* rhs;
    };

    struct BinExpr {
        std::variant<BinExprAdd*, BinExprMult*, BinExprSub*, BinExprDiv*> var;
    };

    struct Term {
        std::variant<TermIntLit*, TermIdent*, TermParen*> var;
    };

    struct Expr {
        std::variant<Term*, BinExpr*> var;
    };

    struct Stmt;

    struct StmtExit {
        Expr* expr;
    };

    struct StmtLet {
        Token ident;
        Expr* expr;
    };

    struct StmtIdent {
        Token ident;
        Expr* expr;
    };

//    struct StmtIf {
//        Expr* expr;
//        std::vector<Stmt*> stmts;
//    };

    struct Stmt {
        std::variant<StmtExit*, StmtLet*, StmtIdent*/*, StmtIf**/> var;
    };

    struct Prog {
        std::vector<Stmt*> stmts;
    };

}

std::optional<int> binPrec(TokenType type) {
    switch (type) {
        case TokenType::mult:
        case TokenType::div:
            return 2;
        case TokenType::plus:
        case TokenType::sub:
            return 1;
        case TokenType::less:
        case TokenType::lesseq:
        case TokenType::greater:
        case TokenType::greatereq:
        case TokenType::comp:
            return 0;
        default:
            return {};
    }
}

class Parser {
public:
    inline explicit Parser(std::vector<Token> tokens)
            : m_tokens(std::move(tokens))
            , m_allocator(1024 * 1024 * 4) // 4 mb
    {
    }

    std::optional<node::Term*> parseTerm()
    {
        if (auto intLit = tryConsume(TokenType::int_lit)) {
            auto termIntLit = m_allocator.alloc<node::TermIntLit>();
            termIntLit->int_lit = intLit.value();
            auto term = m_allocator.alloc<node::Term>();
            term->var = termIntLit;
            return term;
        }
        else if (auto ident = tryConsume(TokenType::ident)) {
            auto exprIdent = m_allocator.alloc<node::TermIdent>();
            exprIdent->ident = ident.value();
            auto term = m_allocator.alloc<node::Term>();
            term->var = exprIdent;
            return term;
        }
        else if (auto open_paren = tryConsume(TokenType::open_paren)) {
            auto expr = parseExpr();
            if (!expr.has_value()) {
                std::cerr << "Expected expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            tryConsume(TokenType::close_paren, "Expected `)`");
            auto termParen = m_allocator.alloc<node::TermParen>();
            termParen->expr = expr.value();
            auto term = m_allocator.alloc<node::Term>();
            term->var = termParen;
            return term;
        }
        else {
            return {};
        }
    }

    std::optional<node::Expr*> parseExpr(int minPrec = 0)
    {
        std::optional<node::Term*> termLhs = parseTerm();
        if (!termLhs.has_value()) {
            return {};
        }
        auto expr_lhs = m_allocator.alloc<node::Expr>();
        expr_lhs->var = termLhs.value();

        while (true) {
            std::optional<Token> currToken = peek();
            std::optional<int> prec;
            if (currToken.has_value()) {
                prec = binPrec(currToken->type);
                if (!prec.has_value() || prec < minPrec) {
                    break;
                }
            }
            else {
                break;
            }
            Token op = consume();
            int nextMinPrec = prec.value() + 1;
            auto expr_rhs = parseExpr(nextMinPrec);
            if (!expr_rhs.has_value()) {
                std::cerr << "Unable to parse expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            auto expr = m_allocator.alloc<node::BinExpr>();
            auto exprLhs2 = m_allocator.alloc<node::Expr>();
            if (op.type == TokenType::plus) {
                auto add = m_allocator.alloc<node::BinExprAdd>();
                exprLhs2->var = expr_lhs->var;
                add->lhs = exprLhs2;
                add->rhs = expr_rhs.value();
                expr->var = add;
            }
            else if (op.type == TokenType::mult) {
                auto multi = m_allocator.alloc<node::BinExprMult>();
                exprLhs2->var = expr_lhs->var;
                multi->lhs = exprLhs2;
                multi->rhs = expr_rhs.value();
                expr->var = multi;
            }
            else if (op.type == TokenType::sub) {
                auto sub = m_allocator.alloc<node::BinExprSub>();
                exprLhs2->var = expr_lhs->var;
                sub->lhs = exprLhs2;
                sub->rhs = expr_rhs.value();
                expr->var = sub;
            }
            else if (op.type == TokenType::div) {
                auto div = m_allocator.alloc<node::BinExprDiv>();
                exprLhs2->var = expr_lhs->var;
                div->lhs = exprLhs2;
                div->rhs = expr_rhs.value();
                expr->var = div;
            }
            else {
                assert(false); // Unreachable;
            }
            expr_lhs->var = expr;
        }
        return expr_lhs;
    }

    std::optional<node::Stmt*> parseStmt()
    {
        if (peek().value().type == TokenType::exit && peek(1).has_value()
            && peek(1).value().type == TokenType::open_paren) {
            consume();
            consume();
            auto stmtExit = m_allocator.alloc<node::StmtExit>();
            if (auto nodeExpr = parseExpr()) {
                stmtExit->expr = nodeExpr.value();
            }
            else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            tryConsume(TokenType::close_paren, "Expected `)`");
            tryConsume(TokenType::semi, "Expected `;`");
            auto stmt = m_allocator.alloc<node::Stmt>();
            stmt->var = stmtExit;
            return stmt;
        }
        else if (peek().has_value() && peek().value().type == TokenType::let && peek(1).has_value()
                && peek(1).value().type == TokenType::ident && peek(2).has_value()
                && peek(2).value().type == TokenType::eq) {
            consume();
            auto stmtLet = m_allocator.alloc<node::StmtLet>();
            stmtLet->ident = consume();
            consume();
            if (auto expr = parseExpr()) {
                stmtLet->expr = expr.value();
            }
            else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            tryConsume(TokenType::semi, "Expected `;`");
            auto stmt = m_allocator.alloc<node::Stmt>();
            stmt->var = stmtLet;
            return stmt;
        }
        else if (peek().has_value() && peek().value().type == TokenType::ident &&
                peek(1).has_value() && peek(1).value().type == TokenType::eq) {
            auto stmtIdent = m_allocator.alloc<node::StmtIdent>();
            stmtIdent->ident = consume();
            consume();
            if (auto expr = parseExpr()) {
                stmtIdent->expr = expr.value();
            }
            else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            tryConsume(TokenType::semi, "Expected `;`");
            auto stmt = m_allocator.alloc<node::Stmt>();
            stmt->var = stmtIdent;
            return stmt;
        }
//        else if (peek().has_value() && peek().value().type == TokenType::_if &&
//                peek(1).value().type == TokenType::open_paren) {
//            consume();
//            consume();
//            auto stmtIf = m_allocator.alloc<node::StmtIf>();
//            if (auto expr = parseExpr()) {
//                stmtIf->expr = expr.value();
//            }
//            else {
//                std::cerr << "Invalid expression" << std::endl;
//                exit(EXIT_FAILURE);
//            }
//
//            tryConsume(TokenType::close_paren, "Expected `)`");
//            tryConsume(TokenType::open_curly, "Expected `{`");
//
//            while (peek().has_value()) {
//                if (auto stmtInsideIf = parseStmt()) {
//                    stmtIf->stmts.push_back(stmtInsideIf.value());
//                }
//                else {
//                    std::cerr << "Invalid statement" << std::endl;
//                    exit(EXIT_FAILURE);
//                }
//            }
//
//            tryConsume(TokenType::close_curly, "Expecteed '}'");
//        }
        else {
            return {};
        }
    }

    std::optional<node::Prog> parseProg()
    {
        node::Prog prog;
        while (peek().has_value()) {
            if (auto stmt = parseStmt()) {
                prog.stmts.push_back(stmt.value());
            }
            else {
                std::cerr << "Invalid statement" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return prog;
    }

private:
    [[nodiscard]] inline std::optional<Token> peek(int offset = 0) const
    {
        if (m_index + offset >= m_tokens.size()) {
            return {};
        }
        else {
            return m_tokens.at(m_index + offset);
        }
    }

    inline Token consume()
    {
        return m_tokens.at(m_index++);
    }

    inline Token tryConsume(TokenType type, const std::string& err_msg)
    {
        if (peek().has_value() && peek().value().type == type) {
            return consume();
        }
        else {
            std::cerr << err_msg << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    inline std::optional<Token> tryConsume(TokenType type)
    {
        if (peek().has_value() && peek().value().type == type) {
            return consume();
        }
        else {
            return {};
        }
    }

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
    ArenaAllocator m_allocator;
};
