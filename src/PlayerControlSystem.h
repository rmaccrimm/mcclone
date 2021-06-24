#ifndef _PLAYERCONTROLSYSTEM_H_
#define _PLAYERCONTROLSYSTEM_H_

class EntityAdmin;

class PlayerControlSystem
{
public:
    PlayerControlSystem(EntityAdmin *admin);
    void tick();
    
private:
    EntityAdmin* m_admin;
};


#endif /* _PLAYERCONTROLSYSTEM_H_ */
