#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "graphics/vga.h"

struct tester
{
    id fb;
    id prog;
    id group;
};
make_type_detail(tester);

static float positions[] = {
    1, 1, 0,
    1, -1, 0,
    -1, 1, 0,

    1, -1, 0,
    -1, -1, 0,
    -1, 1, 0
};

static void tester_init(struct tester *p, key k)
{
    id attr;

    vga_framebuffer_new(&p->fb);
    vga_framebuffer_set_size(p->fb, 800, 480);
    vga_framebuffer_add_texture(p->fb, "diffuse");

    vga_program_new(&p->prog);
    vga_program_load(p->prog, "inner://res/shaders/quad.vert", "inner://res/shaders/quad.frag");

    vga_attribute_group_new(&p->group);

    vga_attribute_new(&attr);
    vga_attribute_fill(attr, positions, sizeof(positions), VGA_STATIC);
    vga_attribute_group_add(p->group, attr, "positions", VGA_FLOAT, 3, VGA_FALSE, sizeof(float) * 3, 0);
    release(attr);
}

static void tester_clear(struct tester *p)
{
    release(p->fb);
    release(p->prog);
    release(p->group);
}

static void tester_update(id pid)
{
    struct tester *raw;

    fetch(pid, &raw);
    assert(raw != NULL);

    vga_program_set_uniform_vec4_scalar(raw->prog, 1, 1, 1, 1, "u_color", 0);
    vga_program_draw_array(raw->prog, raw->group, VGA_TRIANGLES, 0, 6);
}

int main(int argc, char **argv)
{
    id tstr;
    SDL_Event event;
    SDL_Window* glwindow;
    SDL_GLContext glcontext;
    SDL_DisplayMode DM;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GetCurrentDisplayMode(0, &DM);
    glwindow = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            800, 480, SDL_WINDOW_OPENGL);
    glcontext = SDL_GL_CreateContext(glwindow);
    SDL_SetWindowSize(glwindow, 800, 480);

    tester_new(&tstr);

    while (1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)) {
                goto finish;
            }
        }

        tester_update(tstr);

        SDL_GL_SwapWindow(glwindow);
    }

finish:
    release(tstr);
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(glwindow);
    SDL_Quit();

    return 0;
}