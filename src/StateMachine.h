//
// Created by sergiy on 29.09.22.
//

#ifndef FORMALLANG_STATEMACHINE_H
#define FORMALLANG_STATEMACHINE_H

#include <set>
#include <vector>
#include "../test-RE/all_classes.h"

namespace sm {

    class SM;

    class DSM;

    class FDSM;

    class SM {
    public:
        nodeP root;

        explicit SM(const re::parser& parser);

        void print(const std::string& filename);

        void removeEpsSeq();

        void makeRegular();

        void removeLongSeq();

    protected:

        void compressFinal();
    };

    class DSM : public SM {
    public:
        std::vector<std::string> alphabet;

        DSM(SM& sm, const std::vector<std::string>& sigma);
    };

    class FDSM : public DSM {
    public:
        nodeP stock;

        explicit FDSM(const DSM& dsm);

        void inverse();

        void minimise();
    };
}

#endif //FORMALLANG_STATEMACHINE_H
