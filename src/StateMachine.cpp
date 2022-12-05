//
// Created by sergiy on 29.09.22.
//
#include "RegularExpression.h"
#include "StateMachine.h"
#include "SMGraph.h"
#include <string>
#include <fstream>
#include <map>
#include <queue>
#include <stack>


void sm::SM::print(const std::string &filename) {
    std::ofstream file(filename);
    file << "digraph {\n";
    sm::graphModifier::printer printer(std::move(file));
    sm::graphModifier gM;
    gM.dfsWithCoworker(root, printer);
    std::ofstream file1(std::move(printer.file));
    file1 << "}\n";
    file1.close();
}

sm::SM::SM(const re::parser &parser) {
    sm::graphModifier gM;
    root = gM.make(parser.root);
}

void sm::SM::removeLongSeq() {
    sm::graphModifier::shortener sh;
    sm::graphModifier gM;
    gM.dfsWithCoworker(root, sh);
}

void sm::SM::removeEpsSeq() {
    sm::graphModifier gM;
    sm::graphModifier::epsSequencesRemover removeHelper;
    gM.dfsWithCoworker(root, removeHelper);
    auto map = std::move(removeHelper.map);
    for (auto nodeNeighbour: map) {
        for (auto epsNeighbour: nodeNeighbour.second) {
            for (auto edgeNeighbour: epsNeighbour->children) {
                if (edgeNeighbour->sequence == "")
                    continue;
                gM.connectWithSeq(nodeNeighbour.first, edgeNeighbour->to, edgeNeighbour->sequence);
            }
            nodeNeighbour.first->final = nodeNeighbour.first->final || epsNeighbour->final;
        }
    }
    for (auto node1: map) {
        if (!node1.first)
            continue;
        auto node = node1.first;
        std::set<edgeP> epsEdges;
        for (auto ch: node->children)
            if (ch->sequence == "")
                epsEdges.insert(ch);

        for (auto ch : epsEdges)
            node->children.erase(ch);
    }
}

void sm::SM::compressFinal() {
    auto nd = std::make_shared<node>();
    nd->final = true;
    sm::graphModifier gM;
    gM.concatenate(root, nd);
}

sm::DSM::DSM(sm::SM &sm, const std::vector<std::string> &sigma) : SM(sm) {
    alphabet = sigma;
    std::set<nodeSet> Qd;
    sm.removeEpsSeq();
    std::queue<nodeSet> P;
    nodeSet qd;
    std::map<std::pair<nodeSet, nodeSet>, std::string> transitions;
    qd.insert(sm.root);
    P.push(qd);
    Qd.insert(qd);

    std::map<nodeSet, nodeP> newNodes;
    root = std::make_shared<node>();
    if (sm.root->final)
        root->final = true;
    newNodes[*Qd.begin()] = root;

    while (!P.empty()) {
        auto pd = P.front();
        P.pop();
        for (auto c : alphabet) {
            qd.clear();
            for (auto p: pd) {
                for (auto e: p->children) {
                    if (e->sequence != c)
                        continue;
                    qd.insert(e->to);
                }
            }
            if (qd.empty())
                continue;
            transitions[std::make_pair(pd, qd)] = c;
            if (Qd.find(qd) == Qd.end()) {
                P.push(qd);
                Qd.insert(qd);
            }
        }
    }

    for (auto ndSet: Qd) {
        if (newNodes.find(ndSet) == newNodes.end())
            newNodes[ndSet] = std::make_shared<node>();
        for (auto v: ndSet)
            newNodes[ndSet]->final |= v->final;
    }

    std::map<nodeSet, std::vector<std::pair<nodeP, std::string>>> graph;
    for (auto edgeSeq: transitions)
        graph[edgeSeq.first.first].push_back(std::make_pair(newNodes[edgeSeq.first.second], edgeSeq.second));

    sm::graphModifier gM;

    for (auto v: graph) {
        auto from = newNodes[v.first];
        for (auto to: v.second)
            gM.connectWithSeq(from, to.first, to.second);
    }
}

void sm::SM::makeRegular()
{
    compressFinal();
    sm::graphModifier gM;
    sm::graphModifier::nodesFinder finder;
    gM.dfsWithCoworker(root, finder);
    auto vs = std::move(finder.nodes);
    for (auto v: vs)
        gM.makeRegular(v);
    for (auto v : vs) {
        if (v == root ||  v->final || v->parents.empty())
            continue;
        gM.compress(v);
    }
}

sm::FDSM::FDSM(const sm::DSM &dsm) : DSM(dsm) {
    auto stock1 = std::make_shared<node>();
    sm::graphModifier gM;
    sm::graphModifier::stockCreator creator(alphabet, stock1);
    gM.dfsWithCoworker(root, creator);

    for (auto c : alphabet)
        gM.connectWithSeq(stock1, stock1, c);

    stock = stock1;
}

void sm::FDSM::inverse() {
    sm::graphModifier gM;
    sm::graphModifier::endsFinder endsFinder;
    sm::graphModifier::nodesFinder nodesFinder;

    gM.dfsWithCoworker(root, endsFinder);
    gM.dfsWithCoworker(root, nodesFinder);

    auto ends1 = std::move(endsFinder.ends);
    auto vertexes1 = std::move(nodesFinder.nodes);
    for (auto v: vertexes1) {
        if (ends1.find(v) == ends1.end())
            v->final = true;
        else
            v->final = false;
    }
}

void sm::FDSM::minimise() {
    sm::graphModifier gM;
    sm::graphModifier::nodesFinder nodesFinder;
    gM.dfsWithCoworker(root, nodesFinder);

    auto vs = std::move(nodesFinder.nodes);
    int n = vs.size();
    if (vs.find(stock) == vs.end())
        ++n;
    std::vector<nodeP> nodes;
    std::map<nodeP, int> map;
    nodes.push_back(stock);
    nodes.push_back(root);
    map[stock] = 0;
    map[root] = 1;

    for (auto v: vs) {
        if (v == stock || v == root)
            continue;
        nodes.push_back(v);
        map[v] = nodes.size() - 1;
    }

    std::vector<std::vector<bool>> marked(n, std::vector<bool>(n, false));
    std::queue<std::pair<int, int>> Q;

    for (int i = 0; i < n; ++i) {
        if (!nodes[i]->final)
            continue;
        for (int j = 0; j < n; ++j) {
            if (nodes[j]->final)
                continue;

            Q.push(std::make_pair(i, j));
            marked[i][j] = 1;
            marked[j][i] = 1;
        }
    }

    while (!Q.empty()) {
        auto pr = Q.front();
        Q.pop();
        for (auto c : alphabet) { // TODO : rework with gM
            for (auto e1: nodes[pr.first]->parents) {
                if (e1.lock()->sequence != c)
                    continue;
                for (auto e2: nodes[pr.second]->parents) {
                    if (e2.lock()->sequence != c)
                        continue;
                    if (marked[map[e1.lock()->from.lock()]][map[e2.lock()->from.lock()]])
                        continue;
                    marked[map[e1.lock()->from.lock()]][map[e2.lock()->from.lock()]] = 1;
                    marked[map[e2.lock()->from.lock()]][map[e1.lock()->from.lock()]] = 1;
                    Q.push(std::make_pair(map[e1.lock()->from.lock()], map[e2.lock()->from.lock()]));
                }
            }
        }
    }

    std::vector<int> component(n, -1);
    for (int i = 0; i < n; ++i)
        if (!marked[0][i])
            component[i] = 0;

    int componentCtr = 0;
    for (int i = 0; i < n; ++i) {
        if (component[i] == -1) {
            component[i] = ++componentCtr;
            for (int j = i + 1; j < n; ++j)
                if (!marked[i][j])
                    component[j] = componentCtr;
        }
    }

    std::map<int, nodeP> newNodes;
    for (int i = 0; i <= componentCtr; ++i)
        newNodes[i] = std::make_shared<node>();

    for (int i = 0; i < n; ++i) {
        if (nodes[i]->final)
            newNodes[component[i]]->final = true;
        for (auto e: nodes[i]->children) {
            bool alreadyHas = false;
            for (auto E: newNodes[component[i]]->children)
                if (E->to == newNodes[component[map[e->to]]] && E->sequence == e->sequence)
                    alreadyHas = true;
            if (!alreadyHas)
                gM.connectWithSeq(newNodes[component[i]], newNodes[component[map[e->to]]], e->sequence);
        }
    }

    root = newNodes[component[1]];
    stock = newNodes[component[0]];
}