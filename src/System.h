#ifndef _SYSTEM_H_
#define _SYSTEM_H_

class EntityAdmin;

class System {
public:
    System(EntityAdmin& admin) : m_admin { admin } { }
    virtual void tick() = 0;

protected:
    EntityAdmin& m_admin;
};

#endif /* _SYSTEM_H_ */
