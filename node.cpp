#include "node.h"

NODE::NODE():
    kind(NK_UndefinedNode)
{

}

NODE::NODE(NodeKind K):
    kind(K)
{

}

bool NODE::classof(const NODE *N)
{
    return (N->getKind() >= NK_UndefinedNode)
        && (N->getKind() < NK_LAST_NODE);
}

void NODE::addChild(NODE *child)
{
    children.push_back(child);
}

NODE::NodeKind NODE::getKind() const
{
    return kind;
}
