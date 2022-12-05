//
// Created by sergiy on 29.09.22.
//

#ifndef FORMALLANG_REGULAREXPRESSION_H
#define FORMALLANG_REGULAREXPRESSION_H

#include <memory>
#include <string>
#include "../test-RE/all_classes.h"

namespace re {

    class RE{
    public:
        std::string expression;

        explicit RE(const std::string &expr);
        explicit RE(sm::SM& sm);
    };

    class parser {
    public:
        nodeP root;

        explicit parser(const RE& re);

        void print(const std::string& filename);
    private:
        nodeP parse(const std::string& expr);

    };

    class node {
    public:
        nodeP parent;
        nodeP lChild;
        nodeP rChild;
        std::string sequence;

        void print(std::ofstream& file, int& ctr);
    };
}

#endif //FORMALLANG_REGULAREXPRESSION_H
