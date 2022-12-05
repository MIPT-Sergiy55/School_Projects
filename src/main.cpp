//
// Created by sergiy on 29.09.22.
//

#include <iostream>
#include "RegularExpression.h"
#include "SMGraph.h"
#include "StateMachine.h"

int main() {
    // work of parser visualised
    std::string expr;
    std::cin >> expr;
    re::RE re1(expr);
    re::parser parser1(re1);
    parser1.print("../graphs/parser.dot");
    //

    sm::SM sm1(parser1);
    sm1.print("../graphs/sm.dot");

    sm1.removeEpsSeq();
    sm1.print("../graphs/sm_without_eps.dot");

    sm::DSM dsm1(sm1, {"a", "b"});
    dsm1.print("../graphs/dsm.dot");

    sm::FDSM fdsm1(dsm1);
    fdsm1.print("../graphs/fdsm.dot");

    fdsm1.inverse();
    fdsm1.print("../graphs/fdsm_inversed.dot");
    fdsm1.inverse();
//
//    std::string word;
//    while(std::cin >> word) {
//        std::cout << fdsm1.isReadable(word) << '\n';
//    }

    fdsm1.minimise();
    fdsm1.print("../graphs/fdsm_minimised.dot");

    fdsm1.makeRegular();
    fdsm1.print("../graphs/fdsm_regular.dot");

    re::RE re2(fdsm1);
    std::cout << re2.expression << '\n';

    return 0;
}