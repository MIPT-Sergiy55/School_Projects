//
// Created by sergiy on 29.09.22.
//
#include <stack>
#include <fstream>
#include "RegularExpression.h"
#include "SMGraph.h"
#include "StateMachine.h"

re::RE::RE(const std::string& expr) : expression(expr) {}

re::RE::RE(sm::SM& sm) {
    sm.makeRegular();
    sm::nodeP node = sm.root;
    std::string cycle = "";
    std::string endSeq = "";
    for (auto edge : node->children) {
        if (edge->to == node)
            cycle = edge->sequence + "*";
        else
            endSeq = edge->sequence;
    }
    expression = cycle + endSeq;
}


re::parser::parser(const re::RE& re) {
    root = parse(re.expression);
}

re::nodeP re::parser::parse(const std::string &expr) {
    std::stack<char> brackets;
    nodeP res = std::make_shared<node>();

    int firstPlus = -1;
    int firstDot = -1;

    if (expr.empty())
        return res;

    for(int i = 0; i < expr.length(); ++i) {
        if (expr[i] == '(')
            brackets.push('(');
        if (expr[i] == ')')
            brackets.pop();
        if (brackets.empty()) {
            if (expr[i] == '+' && firstPlus == -1)
                firstPlus = i;
            if (firstDot == -1 && i < expr.length() - 1 && expr[i] != '(' && // TODO : check
                expr[i] != '+' && expr[i + 1] != ')' &&
                expr[i + 1] != '+' && expr[i + 1] != '*')
                firstDot = i;
        }
    }

    if (firstPlus != -1) {
        res->sequence = "+";
        res->lChild = parse(expr.substr(0, firstPlus));
        res->rChild = parse(expr.substr(firstPlus + 1, expr.length() - firstPlus - 1));
    } else if (firstDot != -1) {
        res->sequence = ".";
        res->lChild = parse(expr.substr(0, firstDot + 1));
        res->rChild = parse(expr.substr(firstDot + 1, expr.length() - firstDot - 1));
    } else if (expr.back() == '*') {
        res->sequence = "*";
        res->lChild = parse(expr.substr(0, expr.length() - 1));
    } else if (expr[0] == '(' && expr.back() == ')') {
        res->sequence = "(";
        res->lChild = parse(expr.substr(1, expr.length() - 2));
    } else if (expr.length() == 1){
        res->sequence = expr[0];
        return res;
    }
    if (res->lChild)
        res->lChild->parent = res;
    if (res->rChild)
        res->rChild->parent = res;
    return res;
}

void re::parser::print(const std::string &filename) {
    std::ofstream file(filename);
    file << "digraph {\n";
    int ctr = 0;
    root->print(file, ctr);
    file << "}\n";
    file.close();
}


void re::node::print(std::ofstream &file, int& ctr) {
    int c = ctr;
    file << c << " [label=\"" << sequence << "\"]\n";
    if (lChild) {
        file << c << " -> " << ++ctr << "\n";
        lChild->print(file, ctr);
    }
    if (rChild) {
        file << c << " -> " << ++ctr << "\n";
        rChild->print(file, ctr);
    }
}
