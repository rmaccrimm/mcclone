#ifndef _PHYSICSSYSTEM_H_
#define _PHYSICSSYSTEM_H_

class EntityAdmin;

class PhysicsSystem
{
public:
    PhysicsSystem(EntityAdmin *admin);
    void tick();
    
private:
    EntityAdmin* m_admin;
};


#endif /* _PHYSICSSYSTEM_H_ */
