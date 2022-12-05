#include <stack>
#include "SMGraph.h"
#include "RegularExpression.h"

//
// Created by sergiy on 29.09.22.
//
sm::edge::edge(sm::nodeP f, sm::nodeP t, std::string seq) :
        from(f), to(t), sequence(seq)
{}


bool sm::operator<(const sm::edgeW& l, const sm::edgeW& r)
{
    return l.lock() < r.lock();
}

bool sm::operator<(const sm::nodeW& l, const sm::nodeW& r)
{
    return l.lock() < r.lock();
}

void sm::edgeSet::erase(sm::edgeP& edge)
{
    auto from = edge->from;
    auto to = edge->to;
    from.lock()->children.set::erase(edge);
    to->parents.set::erase(edge);
}

void sm::edgeSet::insert(sm::edgeP& edge)
{
    auto from = edge->from;
    auto to = edge->to;
    from.lock()->children.set::insert(edge);
    to->parents.set::insert(edge);
}

bool sm::edgeSet::isIn(const sm::edgeP& edge) const
{
    return (find(edge) != end());
}

void sm::edgeSet::clear()
{
    std::set<edgeP> copy;
    for (const auto& e: *this)
        copy.insert(e);
    for (auto e: copy)
        erase(e);
}


bool sm::nodeSet::isIn(const sm::nodeP& node) const
{
    return (find(node) != end());;
}

void sm::edgeSetW::erase(sm::edgeW& edge)
{
    auto from = edge.lock()->from;
    auto to = edge.lock()->to;
    from.lock()->children.set::erase(edge.lock());
    to->parents.set::erase(edge);
}

//void sm::edgeSetW::insert(sm::edgeW& edge)
//{
//    auto from = edge.lock()->from;
//    auto to = edge.lock()->to;
//    from.lock()->children.set::insert(edge.lock());
//    to->parents.set::insert(edge);
//}
//
//bool sm::edgeSetW::isIn(const sm::edgeW& edge) const
//{
//    return (find(edge) != end());
//}

void sm::edgeSetW::clear()
{
    std::set<edgeW> copy;
    for (const auto& e: *this)
        copy.insert(e);
    for (auto e: copy)
        erase(e);
}

sm::nodeP sm::graphModifier::make(const re::nodeP& reNode)
{
    if (reNode->sequence == "+")
    {
        nodeP root1 = make(reNode->lChild);
        nodeP root2 = make(reNode->rChild);
        return unite(root1, root2);
    } else if (reNode->sequence == ".")
    {
        nodeP root1 = make(reNode->lChild);
        nodeP root2 = make(reNode->rChild);
        return concatenate(root1, root2);
    } else if (reNode->sequence == "*")
    {
        nodeP root1 = make(reNode->lChild);
        return kliniStar(root1);
    } else if (reNode->sequence == "(")
    {
        return make(reNode->lChild);
    }
    return make(reNode->sequence);
}

sm::nodeP sm::graphModifier::make(std::string seq)
{
    nodeP root = std::make_shared<node>();
    nodeP end = std::make_shared<node>();
    end->final = true;
    connectWithParsedSeq(root, end, seq);
    return root;
}

sm::nodeP sm::graphModifier::connectWithSeq(sm::nodeP root, sm::nodeP end, std::string seq)
{
    edgeP newEdge = std::make_shared<edge>(root, end, seq);
    root->children.insert(newEdge);
    return root;
}


sm::nodeP sm::graphModifier::connectWithParsedSeq(sm::nodeP root, sm::nodeP end, std::string seq)
{
    if (seq.length() <= 1)
    {
        connectWithSeq(root, end, seq);
    } else
    {
        auto newNode = std::make_shared<node>();
        connectWithParsedSeq(root, newNode, seq.substr(0, 1));
        connectWithParsedSeq(newNode, end, seq.substr(1, seq.length() - 1));
    }
    return root;
}

sm::nodeP sm::graphModifier::concatenate(sm::nodeP root1, sm::nodeP root2)
{
    endsFinder endsFinder;
    dfsWithCoworker(root1, endsFinder);
    auto ends1 = endsFinder.ends;

    for (auto end: ends1)
    {
        connectWithParsedSeq(end, root2, "");
        end->final = false;
    }
    return root1;
}

sm::nodeP sm::graphModifier::kliniStar(sm::nodeP root1)
{
    auto root0 = std::make_shared<node>();
    root0->final = true;
    concatenate(root1, root0);
    connectWithParsedSeq(root0, root1, "");
    return root0;
}

sm::nodeP sm::graphModifier::unite(sm::nodeP root1, sm::nodeP root2)
{
    nodeP root = std::make_shared<node>();
    nodeP end = std::make_shared<node>();
    end->final = true;
    connectWithParsedSeq(root, root1, "");
    connectWithParsedSeq(root, root2, "");
    concatenate(root, end);
    return root;
}

void sm::graphModifier::dfsWithCoworker(sm::nodeP root, sm::graphModifier::dfsCoworker& coworker)
{
    nodeSet visited;
    std::stack<nodeP> s;
    s.push(root);
    visited.insert(root);
    while (!s.empty())
    {
        auto node = s.top();
        s.pop();
        for (auto e: node->children)
        {
            auto x = e->to;
            if (coworker.workEdge(e) && !visited.isIn(x))
            {
                visited.insert(x);
                s.push(x);
            }
        }
        coworker.workNode(node);
    }
}

void sm::graphModifier::makeRegular(sm::nodeP node)
{
    std::map<nodeP, std::set<std::string>> childrenEdges;
    for (auto e: node->children)
        childrenEdges[e->to].insert(e->sequence);
    std::map<nodeP, std::string> newSequences;
    for (auto nd: childrenEdges)
    {
        if (nd.second.size() == 1)
        {
            newSequences[nd.first] += *nd.second.begin();
            continue;
        }
        newSequences[nd.first] += "(";
        for (auto s: nd.second)
            newSequences[nd.first] += (s + "+");
        newSequences[nd.first].erase(--newSequences[nd.first].end());
        newSequences[nd.first] += ")";
    }
    node->children.clear();
    sm::graphModifier gM;
    for (auto nd: newSequences)
        gM.connectWithSeq(node, nd.first, nd.second);
}

void sm::graphModifier::compress(sm::nodeP node)
{

    std::string seqToSelf = "";
    for (auto e: node->children) {
        if (e->to == node) {
            if (e->sequence.length() == 1)
                seqToSelf = e->sequence + "*";
            else
                seqToSelf = "(" + e->sequence + ")*";
        }
    }
    sm::graphModifier gM;
    for (auto ep : node->parents)
        for (auto ech : node->children)
            if (ep.lock()->from.lock() != node && ech->to != node)
                gM.connectWithSeq(ep.lock()->from.lock(), ech->to, ep.lock()->sequence+seqToSelf+ech->sequence);

    nodeSet ps;

    for (auto ep : node->parents)
        ps.insert(ep.lock()->from.lock());

    node->children.clear();
    node->parents.clear();

    for (auto nd : ps)
        if (nd != node)
            makeRegular(nd);
}

void sm::graphModifier::endsFinder::workNode(sm::nodeP node)
{
    if (node->final)
        ends.insert(node);
}

bool sm::graphModifier::dfsCoworker::workEdge(sm::edgeP edge)
{
    return true;
}

void sm::graphModifier::dfsCoworker::workNode(sm::nodeP node)
{
    return;
}

void sm::graphModifier::nodesFinder::workNode(sm::nodeP node)
{
    nodes.insert(node);
}

bool sm::graphModifier::printer::workEdge(sm::edgeP edge)
{
    if (visited.find(edge->from.lock()) == visited.end())
        visited.insert(std::make_pair(edge->from.lock(), visited.size() + 1));
    if (visited.find(edge->to) == visited.end())
        visited.insert(std::make_pair(edge->to, visited.size() + 1));
    file << visited[edge->from.lock()] << " -> " << visited[edge->to] << " [label=\"" << edge->sequence << "\"]\n";
    return true;
}

void sm::graphModifier::printer::workNode(sm::nodeP node)
{
    if (visited.find(node) == visited.end())
        visited.insert(std::make_pair(node, visited.size() + 1));
    if (node->final)
        file << visited[node] << " [color=\"red\"]\n";
}

sm::graphModifier::printer::printer(std::ofstream&& file) : file(std::move(file))
{}

bool sm::graphModifier::shortener::workEdge(edgeP edge)
{
    if (edge->sequence.length() <= 1)
        return true;

    auto end = edge->to;
    auto root = edge->from;
    auto seq = edge->sequence;
    root.lock()->children.erase(edge);
    sm::graphModifier gM;
    gM.connectWithParsedSeq(root.lock(), end, seq);

    return true;
}

void sm::graphModifier::epsNeighboursFinder::workNode(sm::nodeP node)
{
    neighbours.insert(node);
}

bool sm::graphModifier::epsNeighboursFinder::workEdge(sm::edgeP edge)
{
    return edge->sequence.empty();
}

void sm::graphModifier::epsSequencesRemover::workNode(sm::nodeP node)
{
    sm::graphModifier gM;
    sm::graphModifier::epsNeighboursFinder finder;
    gM.dfsWithCoworker(node, finder);
    finder.neighbours.erase(node);
    map[node] = std::move(finder.neighbours);
}

sm::graphModifier::stockCreator::stockCreator(const std::vector<std::string>& sigma, nodeP stock) :
        alphabet(sigma), stock(stock), symbols()
{}

void sm::graphModifier::stockCreator::workNode(sm::nodeP node)
{
    sm::graphModifier gM;
    for (auto c: alphabet)
    {
        if (symbols.find(c) != symbols.end())
            continue;
        gM.connectWithParsedSeq(node, stock, c);
    }
    symbols.clear();
}

bool sm::graphModifier::stockCreator::workEdge(sm::edgeP edge)
{
    symbols.insert(edge->sequence);
    return true;
}
