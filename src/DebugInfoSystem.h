#ifndef _DEBUGINFOSYSTEM_H_
#define _DEBUGINFOSYSTEM_H_

class EntityAdmin;

class DebugInfoSystem
{
public:
    DebugInfoSystem(EntityAdmin *admin);
    void tick();
     
private:
    EntityAdmin* m_admin;
};


#endif /* _DEBUGINFOSYSTEM_H_ */
