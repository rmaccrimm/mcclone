#ifndef _CAMERACONTROLSYSTEM_H_
#define _CAMERACONTROLSYSTEM_H_

class EntityAdmin;

class CameraControlSystem
{
public:
    CameraControlSystem(EntityAdmin* admin);
    void tick();
    
private:
    EntityAdmin* m_admin;
};

#endif /* _CAMERACONTROLSYSTEM_H_ */
