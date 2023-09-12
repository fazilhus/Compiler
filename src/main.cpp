
// Created by khusn on 9/8/2023.
//

#include <iostream>
#include <fstream>
#include <sstream>

#include "tokenization.hpp"
#include "parser.hpp"
#include "generation.hpp"
#include "arena.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Incorrect usage" <<std::endl;
        return 1;
    }

    std::string contents;
    {
        std::fstream input(argv[1], std::ios::in);
        std::stringstream ss;
        ss << input.rdbuf();
        contents = ss.str();
        input.close();
    }

    Tokenizer tokenizer(std::move(contents));
    auto tokens = tokenizer.tokenize();

    Parser parser(std::move(tokens));
    auto prog = parser.parseProg();

    if (!prog.has_value()) {
        std::cerr << "Invalid program" << std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(prog.value());
    {
        std::fstream file("out.asm", std::ios::out);
        file << generator.genProg();
    }

    system("nasm -felf64 out.asm");
    system("ld -o out out.o");
    return EXIT_SUCCESS;
}