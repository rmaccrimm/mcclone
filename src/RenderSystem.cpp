#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>
#include <vector>

#include "RenderSystem.h"
#include "EntityAdmin.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "BlockComponent.h"


RenderSystem::RenderSystem(EntityAdmin *admin) : m_admin(admin) {}

void RenderSystem::Tick() {
    // for (auto iter: m_admin->ComponentIter<RenderComponent, TransformComponent, BlockComponent>()) {
	
    // }

}
