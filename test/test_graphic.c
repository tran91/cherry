#include "graphics/vga.h"

struct tester
{
	id screen;
	id screen_buffer;
	id fb;
	id group;

	struct {
		id quad;
		id image;
	} program;
};
make_type(tester);

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
	p->screen = id_null;
	p->screen_buffer = id_null;
	p->fb = id_null;
	p->program.quad = id_null;
	p->program.image = id_null;
	p->group = id_null;
}

static void tester_clear(struct tester *p)
{
	release(p->fb);
	release(p->program.quad);
	release(p->program.image);
	release(p->group);
	release(p->screen_buffer);
	release(p->screen);
}

static void tester_setup(id pid, unsigned width, unsigned height, unsigned glid)
{
	struct tester *raw;
	id attr;

	tester_fetch(pid, &raw);
	assert(raw != NULL);

#if defined(BUILD_OPENGL_2) || defined(BUILD_OPENGL_3)
	vga_screen_new(&raw->screen);
	vga_screen_set_size(raw->screen, width, height);

	void vga_screen_set_glid(id pid, unsigned glid);
	vga_screen_set_glid(raw->screen, glid);
#endif

	vga_screenbuffer_new(&raw->screen_buffer);
	vga_screenbuffer_set_screen(raw->screen_buffer, raw->screen);

	vga_framebuffer_new(&raw->fb);
	vga_framebuffer_set_size(raw->fb, width, height);
	vga_framebuffer_set_multisampling(raw->fb, 4);
	vga_framebuffer_add_texture(raw->fb, "diffuse");

	vga_program_new(&raw->program.quad);
	vga_program_load(raw->program.quad, "inner://res/shaders/quad.vert", "inner://res/shaders/quad.frag");

	vga_program_new(&raw->program.image);
	vga_program_load(raw->program.image, "inner://res/shaders/image.vert", "inner://res/shaders/image.frag");

	vga_attribute_group_new(&raw->group);
	vga_attribute_new(&attr);
	vga_attribute_fill(attr, positions, sizeof(positions), VGA_STATIC);
	vga_attribute_group_add(raw->group, attr, "positions", VGA_FLOAT, 3, VGA_FALSE, sizeof(float) * 3, 0);
	release(attr);
}

static void tester_update(id pid)
{
	struct tester *raw;
	id tex;

	tester_fetch(pid, &raw);
	assert(raw != NULL);

	/*
	 * render to user framebuffer
	 */
	vga_framebuffer_begin(raw->fb);
	vga_program_set_uniform_vec4_scalar(raw->program.quad, 1, 0, 0, 1, "u_color", 0);
	vga_program_draw_array(raw->program.quad, raw->group, VGA_TRIANGLES, 0, 6);
	vga_framebuffer_end(raw->fb);

	/*
	 * flush framebuffer to screen
	 */
	vga_framebuffer_get_texture(raw->fb, "diffuse", &tex);
	vga_screenbuffer_begin(raw->screen_buffer);
	vga_program_set_texture(raw->program.image, tex, "diffuse", 0);
	vga_program_draw_array(raw->program.image, raw->group, VGA_TRIANGLES, 0, 6);
	vga_screenbuffer_end(raw->screen_buffer);
}

#if OS == WEB
#include <GL/glew.h>
#include <GL/glfw.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

static id game = id_null;

int init_gl()
{
	unsigned width = 800;
	unsigned height = 480;
	signed glid;

	if (glfwInit() != GL_TRUE) {
		printf("glfwInit() failed\n");
		return GL_FALSE;
	}

	if (glfwOpenWindow(width, height, 8, 8, 8, 8, 16, 8, GLFW_WINDOW) != GL_TRUE) {
		printf("glfwOpenWindow() failed\n");
		return GL_FALSE;
	}
	
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &glid);

	tester_new(&game);
	tester_setup(game, width, height, glid);

	// emscripten_set_mousedown_callback("#canvas", NULL, 1, mouse_down);
	// emscripten_set_mouseup_callback("#canvas", NULL, 1, mouse_up);
	// emscripten_set_mousemove_callback("#canvas", NULL, 1, mouse_move);
	// emscripten_set_mouseout_callback("#canvas", NULL, 1, mouse_cancel);
	// emscripten_set_mouseleave_callback("#canvas", NULL, 1, mouse_cancel);

	// emscripten_set_touchstart_callback("#canvas", NULL, 1, touch_down);
	// emscripten_set_touchend_callback("#canvas", NULL, 1, touch_up);
	// emscripten_set_touchmove_callback("#canvas", NULL, 1, touch_move);
	// emscripten_set_touchcancel_callback("#canvas", NULL, 1, touch_cancel);
	return GL_TRUE;
}

void do_frame()
{
	tester_update(game);
	glfwSwapBuffers();
}

void shutdown_gl()
{
	release(game);
	glfwTerminate();
}

int main(int args, char **argv)
{
	if (init_gl() == GL_TRUE) {
		emscripten_set_main_loop(do_frame, 0, 1);
	}

	shutdown_gl();

	return 0;
}

#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

int main(int argc, char **argv)
{
	id tstr;
	SDL_Event event;
	SDL_Window* glwindow;
	SDL_GLContext glcontext;
	SDL_DisplayMode DM;
	unsigned width = 800;
	unsigned height = 480;
	signed glid;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GetCurrentDisplayMode(0, &DM);
	glwindow = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			width, height, SDL_WINDOW_OPENGL);
	glcontext = SDL_GL_CreateContext(glwindow);
	SDL_SetWindowSize(glwindow, width, height);

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &glid);

	tester_new(&tstr);
	tester_setup(tstr, width, height, glid);
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
#endif