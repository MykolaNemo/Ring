#ifndef DECL_STMT_H
#define DECL_STMT_H

#include "stmt_node.h"

class VAR_DECL;
class DECL_STMT : public STMT_NODE
{
public:
    DECL_STMT();
    static bool classof(const NODE *N);
    std::variant<int, bool> execute() override;

protected:
    DECL_STMT(NodeKind K);

private:
    std::vector<VAR_DECL*> declarations;
};

#endif // DECL_STMT_H
