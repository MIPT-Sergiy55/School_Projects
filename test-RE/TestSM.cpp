//
// Created by sergiy on 30.09.22.
//

#include <gtest/gtest.h>
#include <fstream>
#include "../src/RegularExpression.h"
#include "../src/StateMachine.h"
#include "../src/SMGraph.h"

TEST(testSM, constructibleFromRE) {
    re::RE re("a+b");
    EXPECT_NO_THROW(sm::SM sm1(re::parser(re)));
}

TEST(testSM, constrructDSM) {
    re::RE re1("a+b");
    re::parser parser(re1);
    sm::SM sm1(parser);
    EXPECT_NO_THROW(sm::DSM dsm1(sm1, {"a", "b"}));
}


TEST(testSM, constrructFDSM) {
    re::RE re1("a+b");
    re::parser parser(re1);
    sm::SM sm1(parser);
    sm::DSM dsm1(sm1, {"a", "b"});
    EXPECT_NO_THROW(sm::FDSM fdsm1(dsm1));
}

TEST(testSM, minimiseFDSM) {
    re::RE re1("(a+b)*");
    re::parser parser(re1);
    sm::SM sm1(parser);
    sm::DSM dsm1(sm1, {"a", "b"});
    sm::FDSM fdsm1(dsm1);
    fdsm1.minimise();
    sm::graphModifier gM;
    sm::graphModifier::nodesFinder finder;
    gM.dfsWithCoworker(fdsm1.root, finder);
    EXPECT_EQ(1, finder.nodes.size());
}

TEST(testSM, minimiseFDSMhuge) {
    re::RE re1("(aa+bb+(ab+ba)(aa+bb)*(ab+ba))*");
    re::parser parser(re1);
    sm::SM sm1(parser);
    sm::DSM dsm1(sm1, {"a", "b"});
    sm::FDSM fdsm1(dsm1);
    fdsm1.minimise();
    sm::graphModifier gM;
    sm::graphModifier::nodesFinder finder;
    gM.dfsWithCoworker(fdsm1.root, finder);
    EXPECT_EQ(4, finder.nodes.size());
}


TEST(testSM, printSM) {
    re::RE re1("(aa+bb+(ab+ba)(aa+bb)*(ab+ba))*");
    re::parser parser(re1);
    sm::SM sm1(parser);
    sm::DSM dsm1(sm1, {"a", "b"});
    sm::FDSM fdsm1(dsm1);
    fdsm1.minimise();
    EXPECT_NO_THROW(fdsm1.print("./test.dot"));
}


TEST(testSM, rempveLongSeq) {
    re::RE re1("(aa+bb+(ab+ba)(aa+bb)*(ab+ba))*");
    re::parser parser(re1);
    sm::SM sm1(parser);
    sm::DSM dsm1(sm1, {"a", "b"});
    sm::FDSM fdsm1(dsm1);
    fdsm1.minimise();
    EXPECT_NO_THROW(fdsm1.removeLongSeq());
}