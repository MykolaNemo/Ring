#ifndef VAR_DECL_H
#define VAR_DECL_H

#include "node.h"

class VAR_DECL: public NODE
{
public:
    VAR_DECL();
    static bool classof(const NODE *N);

    std::string name;
    std::string type;
    int m_value = 0;
};

#endif // VAR_DECL_H
