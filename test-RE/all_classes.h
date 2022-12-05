//
// Created by sergiy on 29.09.22.
//

#ifndef FORMALLANG_ALL_CLASSES_H
#define FORMALLANG_ALL_CLASSES_H

#include <memory>
namespace sm {
    class SM;
    class DSM;
    class FDSM;

    class node;
    class edge;
    class nodeSet;
    class edgeSet;
    class edgeSetW;

    class graphModifier;

    using nodeP = std::shared_ptr<node>;
    using nodeW = std::weak_ptr<node>;
    using edgeP = std::shared_ptr<edge>;
    using edgeW = std::weak_ptr<edge>;
}

namespace re {

    class RE;
    class parser;
    class node;

    using nodeP = std::shared_ptr<node>;
    using nodeW = std::weak_ptr<node>;
}

#endif //FORMALLANG_ALL_CLASSES_H
