// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   GlfwWindowProxy.cpp                                :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/05 13:48:59 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/16 18:06:18 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "GlfwWindowProxy.hpp"
#include <stdexcept>

static GLFWwindow	*full_screen_window(char const *title)
{
	GLFWmonitor *const	monitor = glfwGetPrimaryMonitor();
	int					vidmode_count;
	GLFWvidmode const	*vidmodes;

	vidmodes = glfwGetVideoModes(monitor, &vidmode_count);
	if (vidmode_count == 0)
		return (nullptr);
	return (glfwCreateWindow(vidmodes[vidmode_count - 1].width,
		vidmodes[vidmode_count - 1].height, title, monitor, nullptr));
}

#ifdef MAC_OS_MODE
# define INIT_GLEW			true
# define OPENGL_PROFILE		GLFW_OPENGL_CORE_PROFILE
#else
# define INIT_GLEW			(glewInit() == GLEW_OK)
# define OPENGL_PROFILE		GLFW_OPENGL_COMPAT_PROFILE
#endif
#ifdef GLFW_DOUBLEBUFFER
# define HINT_DOUBLE_BUFFER	true
#else
# define GLFW_DOUBLEBUFFER	0
# define HINT_DOUBLE_BUFFER	false
#endif

GlfwWindowProxy::GlfwWindowProxy(
		std::optional<std::pair<unsigned, unsigned>> win_size,
		char const *title, std::pair<unsigned, unsigned> gl_version)
{
	init();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_version.first);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_version.second);
	if (HINT_DOUBLE_BUFFER)
		glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, OPENGL_PROFILE);
	_window = (win_size)
		? glfwCreateWindow(win_size->first, win_size->second, title, 0, 0)
		: full_screen_window(title);
	if (_window == nullptr)
	{
		deinit();
		throw std::runtime_error("Failed to create window");
	}
	glfwSetKeyCallback(_window, &GlfwWindowProxy::c_key);
	glfwSetMouseButtonCallback(_window, &GlfwWindowProxy::c_mouse);
	glfwSetCursorPosCallback(_window, &GlfwWindowProxy::c_cursor_pos);
	glfwSetCursorEnterCallback(_window, &GlfwWindowProxy::c_cursor_enter);
	glfwSetScrollCallback(_window, &GlfwWindowProxy::c_scroll);
	glfwSetWindowPosCallback(_window, &GlfwWindowProxy::c_win_pos);
	glfwSetWindowSizeCallback(_window, &GlfwWindowProxy::c_win_size);
	glfwSetWindowCloseCallback(_window, &GlfwWindowProxy::c_win_close);
	glfwSetWindowRefreshCallback(_window, &GlfwWindowProxy::c_win_refresh);
	glfwSetWindowFocusCallback(_window, &GlfwWindowProxy::c_win_focus);
	glfwSetWindowIconifyCallback(_window, &GlfwWindowProxy::c_win_iconify);
	glfwSetFramebufferSizeCallback(_window,
			&GlfwWindowProxy::c_win_framebuffer_size);
	glfwSetWindowUserPointer(_window, this);
	glfwMakeContextCurrent(_window);
	if (!INIT_GLEW)
	{
		deinit();
		throw std::runtime_error("Failed to init GLEW");
	}
	{
		auto size = get_window_size(true);
		glViewport(0, 0, size.first, size.second);
	}
}

GlfwWindowProxy::~GlfwWindowProxy()
{
	glfwDestroyWindow(_window);
	deinit();
}

GLFWwindow		*GlfwWindowProxy::get_window() { return (_window); }

std::pair<unsigned, unsigned>
				GlfwWindowProxy::get_window_size(bool pixel) const
{
	int				width;
	int				height;

	if (pixel)
		glfwGetFramebufferSize(_window, &width, &height);
	else
		glfwGetWindowSize(_window, &width, &height);
	return (std::make_pair(width, height));
}

void			GlfwWindowProxy::init()
{
	if (GlfwWindowProxy::window_count == 0 && !glfwInit())
		throw std::runtime_error("Failed to init glfw");
	GlfwWindowProxy::window_count++;
}

void			GlfwWindowProxy::deinit()
{
	if (--GlfwWindowProxy::window_count == 0)
		glfwTerminate();
}

unsigned		GlfwWindowProxy::window_count = 0;

/*
** ========================================================================== **
** Events handlers
*/

static GlfwWindowProxy	*get_instance(GLFWwindow *w)
{
	return (static_cast<GlfwWindowProxy*>(glfwGetWindowUserPointer(w)));
}

void			GlfwWindowProxy::c_cursor_enter(GLFWwindow *w, int entered)
{
	if (entered)
		get_instance(w)->on_cursor_enter();
	else
		get_instance(w)->on_cursor_leave();
}

void			GlfwWindowProxy::c_cursor_pos(GLFWwindow *w, double x, double y)
{
	get_instance(w)->on_cursor_move(x, y);
}

void			GlfwWindowProxy::c_key(GLFWwindow *w, int key,
					int scancode, int action, int mods)
{
	GlfwWindowProxy *const	p = get_instance(w);

	if (action == GLFW_PRESS)
		p->on_key_press(key, scancode, mods);
	else if (action == GLFW_RELEASE)
		p->on_key_release(key, scancode, mods);
	else
		p->on_key_repeat(key, scancode, mods);
}

void			GlfwWindowProxy::c_mouse(GLFWwindow *w, int button,
					int action, int mods)
{
	GlfwWindowProxy *const	p = get_instance(w);

	if (action == GLFW_PRESS)
		p->on_mouse_button_press(button, mods);
	else
		p->on_mouse_button_release(button, mods);
}

void			GlfwWindowProxy::c_scroll(GLFWwindow *w, double x, double y)
{
	get_instance(w)->on_mouse_scroll(x, y);
}

void			GlfwWindowProxy::c_win_close(GLFWwindow *w)
{
	get_instance(w)->on_window_close();
}

void			GlfwWindowProxy::c_win_focus(GLFWwindow *w, int focused)
{
	if (focused)
		get_instance(w)->on_window_focus();
	else
		get_instance(w)->on_window_defocus();
}

void			GlfwWindowProxy::c_win_iconify(GLFWwindow *w, int iconified)
{
	if (iconified)
		get_instance(w)->on_window_iconify();
	else
		get_instance(w)->on_window_restore();
}

void			GlfwWindowProxy::c_win_pos(GLFWwindow *w, int x, int y)
{
	get_instance(w)->on_window_move(x, y);
}

void			GlfwWindowProxy::c_win_refresh(GLFWwindow *w)
{
	get_instance(w)->on_window_refresh();
}

void			GlfwWindowProxy::c_win_size(GLFWwindow *win, int w, int h)
{
	get_instance(win)->on_window_resize(w, h);
}

void			GlfwWindowProxy::c_win_framebuffer_size(GLFWwindow *win,
					int w, int h)
{
	get_instance(win)->on_framebuffer_resize(w, h);
}

/*
** -
** Dummy callback methods
*/

void			GlfwWindowProxy::on_cursor_enter() {}
void			GlfwWindowProxy::on_cursor_leave() {}
void			GlfwWindowProxy::on_cursor_move(double, double) {}

void			GlfwWindowProxy::on_key_press(int, int, int) {}
void			GlfwWindowProxy::on_key_release(int, int, int) {}
void			GlfwWindowProxy::on_key_repeat(int, int, int) {}

void			GlfwWindowProxy::on_mouse_button_press(int, int) {}
void			GlfwWindowProxy::on_mouse_button_release(int, int) {}
void			GlfwWindowProxy::on_mouse_scroll(double, double) {}

void			GlfwWindowProxy::on_window_move(int, int) {}
void			GlfwWindowProxy::on_window_resize(int, int) {}
void			GlfwWindowProxy::on_window_close() {}
void			GlfwWindowProxy::on_window_focus() {}
void			GlfwWindowProxy::on_window_defocus() {}
void			GlfwWindowProxy::on_window_refresh() {}
void			GlfwWindowProxy::on_window_iconify() {}
void			GlfwWindowProxy::on_window_restore() {}
void			GlfwWindowProxy::on_framebuffer_resize(int, int) {}
