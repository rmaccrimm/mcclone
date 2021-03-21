#ifndef _RENDERSYSTEM_H_
#define _RENDERSYSTEM_H_

class EntityAdmin;

/* Should eventually inherit from a System base class
 */
class RenderSystem
{
public:
    RenderSystem(EntityAdmin* admin);
    void tick();
private:
    EntityAdmin* m_admin;
};

#endif /* _RENDERSYSTEM_H_ */
