#ifndef VAR_DECL_H
#define VAR_DECL_H

#include "decl_node.h"

class VAR_DECL: public DECL_NODE
{
public:
    VAR_DECL();
    static bool classof(const NODE *N);

    std::variant<int, bool> execute() override;

    std::string name;
    // std::string type;
    std::variant<int, bool> m_value;
};

#endif // VAR_DECL_H
