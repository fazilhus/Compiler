//
// Created by khusn on 9/8/2023.
//

#pragma once

#include <string>
#include <sstream>
#include <unordered_map>

#include "parser.hpp"

class Generator {
public:
    inline explicit Generator(node::Prog prog) : m_prog(std::move(prog)) {}

    inline void genTerm(const node::Term* term) {
        struct TermVisitor {
            Generator* gen;

            void operator()(const node::term::IntLit* termIntLit) const {
                gen->m_output << "\tmov rax, " << termIntLit->int_lit.value.value() << '\n';
                gen->push("rax");
            }

            void operator()(const node::term::Ident* termIdent) const {
                if (gen->m_vars.find(termIdent->ident.value.value()) == gen->m_vars.end()) {
                    std::cerr << "Undeclared identifier: " <<  termIdent->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }

                const auto& var = gen->m_vars.at(termIdent->ident.value.value());
                std::stringstream offset;
                offset << "QWORD [rsp + " << (gen->m_stackSize - var.stackLoc - 1) * 8 << ']';
                gen->push(offset.str());
            }

            void operator()(const node::term::Paren* termParen) const {
                gen->genExpr(termParen->expr);
            }
        };

        TermVisitor visitor({ .gen = this });
        std::visit(visitor, term->var);
    }

    inline void genBinExpr(const node::BinExpr* binExpr) {
        struct BinExprVisitor {
            Generator* gen;

            void operator()(const node::binary::ExprAdd* binExprAdd) const {
                gen->genExpr(binExprAdd->lhs);
                gen->genExpr(binExprAdd->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output << "\tadd rax, rbx\n";
                gen->push("rax");
            }

            void operator()(const node::binary::ExprSub* binExprSub) const {
                gen->genExpr(binExprSub->lhs);
                gen->genExpr(binExprSub->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output << "\tsub rax, rbx\n";
                gen->push("rax");
            }

            void operator()(const node::binary::ExprMult* binExprMult) const {
                gen->genExpr(binExprMult->lhs);
                gen->genExpr(binExprMult->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output << "\tmul rbx\n";
                gen->push("rax");
            }

            void operator()(const node::binary::ExprDiv* binExprDiv) const {
                gen->genExpr(binExprDiv->lhs);
                gen->genExpr(binExprDiv->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output << "\tdiv rbx\n";
                gen->push("rax");
            }

            //void operator()(const node::binary::)
        };

        BinExprVisitor visitor({ .gen = this });
        std::visit(visitor, binExpr->var);
    }
    
    inline void genExpr(const node::Expr* expr)  {
        struct ExprVisitor {
            Generator* gen;

           void operator()(const node::Term* term) const {
               gen->genTerm(term);
           }

           void operator()(const node::BinExpr* binExpr) const {
               gen->genBinExpr(binExpr);
           }
        };
        
        ExprVisitor visitor{ .gen = this };
        std::visit(visitor, expr->var);
    }
    
    inline void genStmt(const node::Stmt* stmt)  {
        struct StmtVisitor {
            Generator* gen;

            void operator()(const node::statement::Exit* stmtExit) const {
                gen->b_returned = true;
                gen->genExpr(stmtExit->expr);

                gen->m_output << "\tmov rax, 60\n";
                gen->pop("rdi");
                gen->m_output << "\tsyscall\n";
            }
            
            void operator()(const node::statement::Let* stmtLet) const {
                if (gen->m_vars.find(stmtLet->ident.value.value()) != gen->m_vars.end()) {
                    std::cerr << "Identifier already exists: " << stmtLet->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }

                gen->m_vars.insert({stmtLet->ident.value.value(), Var{.stackLoc = gen->m_stackSize}});
                gen->genExpr(stmtLet->expr);
            }

            void operator()(const node::statement::Ident* stmtIdent) const {
                if (gen->m_vars.find(stmtIdent->ident.value.value()) == gen->m_vars.end()) {
                    std::cerr << "Identifier " << stmtIdent->ident.value.value() << " is not declared" << std::endl;
                    exit(EXIT_FAILURE);
                }

                gen->m_vars[stmtIdent->ident.value.value()] = Var{ .stackLoc = gen->m_stackSize};
                gen->genExpr(stmtIdent->expr);
            }
        };
        
        StmtVisitor visitor{ .gen = this };
        std::visit(visitor, stmt->var);
    }

    [[nodiscard]] inline std::string genProg() {
        m_output << "global _start\n_start:\n";
        
        for (const auto& stmt : m_prog.stmts) {
            genStmt(stmt);
        }

        if (!b_returned) {
            m_output << "\tmov rax, 60\n";
            m_output << "\tmov rdi, 0\n";
            m_output << "\tsyscall\n";
        }
        return m_output.str();
    }
    
private:
    struct Var {
        std::size_t stackLoc;
    };

    const node::Prog m_prog;
    std::stringstream m_output;
    std::size_t m_stackSize{0};
    std::unordered_map<std::string, Var> m_vars{};
    bool b_returned{false};

    void push(const std::string& reg) {
        m_output << "\tpush " << reg << '\n';
        ++m_stackSize;
    }

    void pop(const std::string& reg) {
        m_output << "\tpop " << reg << '\n';
        --m_stackSize;
    }
};
