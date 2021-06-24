#ifndef _MOVEMENTSYSTEM_H_
#define _MOVEMENTSYSTEM_H_

class EntityAdmin;

class MovementSystem
{
public:
    MovementSystem(EntityAdmin *admin);
    void tick();

private:
    EntityAdmin* m_admin;
};



#endif /* _MOVEMENTSYSTEM_H_ */
