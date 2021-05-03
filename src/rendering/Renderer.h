#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <array>
#include <glm/mat4x4.hpp>
#include <memory>
#include <vector>
#include <string>
#include <map>

#include <SDL2/SDL.h>

#include "Vertex.h"

class ChunkManager;

class Renderer {

public:
    Renderer(SDL_Window* window);
    ~Renderer();

    int initialize();

    void draw(SDL_Surface* surface);

    void clearBuffers();

    void setViewMatrix(glm::mat4 view_matrix);

    void copyVertexData(const std::array<Vertex, 4>& verts, const std::array<int, 6>& inds);

    void reset();

private:
    int cleanup();
    int initGeometry();
    int initTextures();

    int initShader(std::string shader_name);
    void useShader(std::string shader_name);

    int loadGrassTexture();

    int m_SCREEN_W;
    int m_SCREEN_H;

    struct GLdata {
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        GLuint tex;
	GLuint depth_buffer;
    };

    std::map<std::string, GLuint> m_shader_prog_map;

    SDL_Window* m_window;
    SDL_GLContext m_context;

    GLdata m_scene;
    GLdata m_screen;

    GLuint m_frame_buffer;

    std::unique_ptr<GLfloat[]> m_vert_buff;
    std::unique_ptr<GLint[]> m_index_buff;
    int m_vert_buff_pos;
    int m_index_buff_pos;

    glm::mat4 m_view_matrix;
};

#endif /* _RENDERER_H_ */
