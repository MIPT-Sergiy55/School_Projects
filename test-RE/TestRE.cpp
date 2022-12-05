//
// Created by sergiy on 30.09.22.
//

#include <gtest/gtest.h>
#include <fstream>
#include "../src/RegularExpression.h"
#include "../src/StateMachine.h"

TEST(testRE, stringConstructor) {
    re::RE re("a+b");
    EXPECT_EQ(re.expression, "a+b");
}

TEST(testRE, parse) {
    re::RE re("a+b");
    re::parser parser(re);
    std::string file("./parse.dot");
    EXPECT_NO_THROW(parser.print(file));

    std::ifstream in(file);
    std::string str, x;
    while(in >> x) {
        str += x;
    }
    EXPECT_EQ("digraph{0[label=\"+\"]0->11[label=\"a\"]0->22[label=\"b\"]}", str);
}

TEST(testRE, hugeExpression) {
    re::RE re("(aa+bb+(ab+ba)(aa+bb)*(ab+ba))*");
    re::parser parser(re);
    std::string file("./parse1.dot");
    EXPECT_NO_THROW(parser.print(file));

    std::ifstream in(file);
    std::string str, x;
    while(in >> x) {
        str += x + " ";
    }
    EXPECT_EQ("digraph { 0 [label=\"*\"] 0 -> 1 1 [label=\"(\"] 1 -> 2 2 [label=\"+\"] 2 -> 3 3 [label=\".\"] 3 -> 4 4 [label=\"a\"] 3 -> 5 5 [label=\"a\"] 2 -> 6 6 [label=\"+\"] 6 -> 7 7 [label=\".\"] 7 -> 8 8 [label=\"b\"] 7 -> 9 9 [label=\"b\"] 6 -> 10 10 [label=\".\"] 10 -> 11 11 [label=\"(\"] 11 -> 12 12 [label=\"+\"] 12 -> 13 13 [label=\".\"] 13 -> 14 14 [label=\"a\"] 13 -> 15 15 [label=\"b\"] 12 -> 16 16 [label=\".\"] 16 -> 17 17 [label=\"b\"] 16 -> 18 18 [label=\"a\"] 10 -> 19 19 [label=\".\"] 19 -> 20 20 [label=\"*\"] 20 -> 21 21 [label=\"(\"] 21 -> 22 22 [label=\"+\"] 22 -> 23 23 [label=\".\"] 23 -> 24 24 [label=\"a\"] 23 -> 25 25 [label=\"a\"] 22 -> 26 26 [label=\".\"] 26 -> 27 27 [label=\"b\"] 26 -> 28 28 [label=\"b\"] 19 -> 29 29 [label=\"(\"] 29 -> 30 30 [label=\"+\"] 30 -> 31 31 [label=\".\"] 31 -> 32 32 [label=\"a\"] 31 -> 33 33 [label=\"b\"] 30 -> 34 34 [label=\".\"] 34 -> 35 35 [label=\"b\"] 34 -> 36 36 [label=\"a\"] } ",
              str);
}

TEST(testRE, constructFromSM) {
    re::RE re1("(aa+bb+(ab+ba)(aa+bb)*(ab+ba))*");
    re::parser parser1(re1);
    sm::SM sm1(parser1);
    sm::DSM dsm1(sm1, {"a", "b"});
    sm::FDSM fdsm1(dsm1);
    fdsm1.minimise();
    re::RE re2(fdsm1);
//    EXPECT_EQ("((a(bb)*ba+b)((aa+ab(bb)*ba))*(ab(bb)*a+b)+a(bb)*a)*" ,re2.expression);
    EXPECT_TRUE("((a(bb)*ba+b)((aa+ab(bb)*ba))*(ab(bb)*a+b)+a(bb)*a)*" == re2.expression || "((aa+bb)+(ab+ba)((aa+bb))*(ab+ba))*" == re2.expression || "((aa+ab(bb)*ba)+(ab(bb)*a+b)(a(bb)*a)*(a(bb)*ba+b))*" == re2.expression);
}