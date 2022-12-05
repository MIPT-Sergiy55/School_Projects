//
// Created by sergiy on 29.09.22.
//

#ifndef FORMALLANG_SMGRAPH_H
#define FORMALLANG_SMGRAPH_H

#include <memory>
#include <string>
#include <set>
#include <unordered_set>
#include <fstream>
#include <map>
#include <vector>
#include "../test-RE/all_classes.h"

namespace sm {

    class edge {
    public:
        nodeW from;
        nodeP to;
        std::string sequence;

        edge(nodeP f, nodeP t, std::string seq);
    };

    bool operator<(const nodeW& l, const nodeW& r);
    bool operator<(const edgeW& l, const edgeW& r);

    class edgeSet : public std::set<edgeP, std::less<>> {
    public:
        void erase(edgeP& edge);
        void insert(edgeP& edge);
        bool isIn(const edgeP& edge) const;
        void clear();
    };

    class edgeSetW : public std::set<edgeW> {
    public:
        void erase(edgeW& edge);
//        void insert(edgeW& edge);
//        bool isIn(const edgeW& edge) const;
        void clear();
    };

    class node {
    public:
        edgeSetW parents;
        edgeSet children;
        bool final = false;
    };

    class nodeSet : public std::set<nodeP> {
    public:
        bool isIn(const nodeP& node) const;
    };

    class graphModifier {
    public:
        nodeP make(const re::nodeP& reNode);

        class dfsCoworker {
        public:
            virtual void workNode(nodeP node);
            virtual bool workEdge(edgeP edge);
        };

        class endsFinder : public dfsCoworker {
        public:
            nodeSet ends;

            void workNode(nodeP node) override;
        };

        class nodesFinder : public dfsCoworker {
        public:
            nodeSet nodes;

            void workNode(nodeP node) override;
        };

        class printer : public dfsCoworker {
        public:
            std::ofstream file;
            std::map<nodeP, int> visited;

            explicit printer(std::ofstream&& file);

            void workNode(nodeP node) override;
            bool workEdge(edgeP edge) override;
        };

        class shortener : public dfsCoworker {
        public:
            bool workEdge(edgeP edge) override;
        };

        class epsNeighboursFinder : public  dfsCoworker {
        public:
            nodeSet neighbours;

            void workNode(nodeP node) override;
            bool workEdge(edgeP edge) override;
        };

        class epsSequencesRemover : public dfsCoworker {
        public:
            std::map<nodeP, nodeSet> map;

            void workNode(sm::nodeP node) override;
        };

        class stockCreator : public dfsCoworker {
        public:
            std::vector<std::string> alphabet;
            std::set<std::string> symbols;
            nodeP stock;

            stockCreator(const std::vector<std::string> &sigma, nodeP stock);

            bool workEdge(sm::edgeP edge) override;
            void workNode(sm::nodeP node) override;
        };

        void dfsWithCoworker(nodeP root, dfsCoworker& coworker);
        nodeP connectWithSeq(nodeP root, nodeP end, std::string seq);
        nodeP concatenate(nodeP root1, nodeP root2);
        void makeRegular(nodeP nd);
        void compress(nodeP nd);
    private:
        nodeP connectWithParsedSeq(nodeP root, nodeP end, std::string seq);
        nodeP kliniStar(nodeP root1);
        nodeP make(std::string seq);

        nodeP unite(nodeP root1, nodeP root2);
    };
}

#endif //FORMALLANG_SMGRAPH_H
