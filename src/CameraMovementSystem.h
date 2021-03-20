#ifndef _CAMERAMOVEMENTSYSTEM_H_
#define _CAMERAMOVEMENTSYSTEM_H_

class EntityAdmin;

class CameraMovementSystem
{
public:
    CameraMovementSystem(EntityAdmin *admin);
    ~CameraMovementSystem();

    void Tick();
    
private:
    EntityAdmin* m_admin;
};


#endif /* _CAMERAMOVEMENTSYSTEM_H_ */
