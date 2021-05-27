#ifndef _CHUNKLOADINGSYSTEM_H_
#define _CHUNKLOADINGSYSTEM_H_

class EntityAdmin;

class ChunkLoadingSystem
{
public:
    ChunkLoadingSystem(EntityAdmin *m_admin);
    void tick();
    
private:
    EntityAdmin *m_admin;
};


#endif /* _CHUNKLOADINGSYSTEM_H_ */
