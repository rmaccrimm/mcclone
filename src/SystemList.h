#ifndef _SYSTEMLIST_H_
#define _SYSTEMLIST_H_

#include "System.h"

#include <memory>
#include <vector>

class SystemList {
public:
    SystemList(EntityAdmin& admin);
    void runAll();

private:
    std::vector<std::unique_ptr<System>> m_systems;
};

#endif /* _SYSTEMLIST_H_ */
