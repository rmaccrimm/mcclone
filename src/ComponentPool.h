#ifndef COMPONENTPOOL_H
#define COMPONENTPOOL_H

class Component;

class ComponentPool {
public:
    ComponentPool();
    ~ComponentPool();
    
    Component* alloc();
    void free();

private:
    Component* pool;
};


#endif
