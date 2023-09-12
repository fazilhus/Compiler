/*
* fazilhus
*/

#pragma once

#include "tokenization.hpp"

#include <vector>
#include <variant>

namespace node {

    namespace term {

        struct IntLit {
            Token int_lit;
        };

        struct Ident {
            Token ident;
        };

        struct Paren {
            Expr* expr;
        };
    }
    
    struct Expr;

    namespace unary {
        struct ExprNeg {
            Expr* rhs;
        };
    }

    namespace binary {

        struct ExprAdd {
            Expr* lhs;
            Expr* rhs;
        };

        struct ExprMult {
            Expr* lhs;
            Expr* rhs;
        };

        struct ExprSub {
            Expr* lhs;
            Expr* rhs;
        };

        struct ExprDiv {
            Expr* lhs;
            Expr* rhs;
        };

        struct ExprLess {
            Expr* lhs;
            Expr* rhs;
        };

        struct ExprLessOrEq {
            Expr* lhs;
            Expr* rhs;
        };

        struct ExprGreater {
            Expr* lhs;
            Expr* rhs;
        };

        struct ExprGreaterOrEq {
            Expr* lhs;
            Expr* rhs;
        };

        struct ExprEq {
            Expr* lhs;
            Expr* rhs;
        };

    }

    struct UnExpr {
        std::variant<unary::ExprNeg> var;
    };

    struct BinExpr {
        std::variant<binary::ExprAdd*, binary::ExprMult*, binary::ExprSub*, binary::ExprDiv*, binary::ExprLess*, binary::ExprLessOrEq*, binary::ExprGreater*, binary::ExprGreaterOrEq*, binary::ExprEq*> var;
    };

    struct Term {
        std::variant<term::IntLit*, term::Ident*, term::Paren*> var;
    };

    struct Expr {
        std::variant<Term*, UnExpr*, BinExpr*> var;
    };

    namespace statement {

        struct Stmt;

        struct Exit {
            Expr* expr;
        };

        struct Let {
            Token ident;
            Expr* expr;
        };

        struct Ident {
            Token ident;
            Expr* expr;
        };

        // struct If {
        //     Expr* expr;
        //     std::vector<Stmt*> stmts;
        // };

    }

    struct Stmt {
        std::variant<statement::Exit*, statement::Let*, statement::Ident*> var;
    };

    struct Prog {
        std::vector<Stmt*> stmts;
    };

}
