// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   GlfwWindowProxy.cpp                                :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/05 13:48:59 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/05 19:27:53 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "GlfwWindowProxy.hpp"
#include <stdexcept>

GlfwWindowProxy::GlfwWindowProxy(unsigned width, unsigned height,
		char const *title)
{
	init();
	_window = glfwCreateWindow(width, height, title, NULL, NULL);
	// _window = glfwCreateWindow(width, height, title, glfwGetPrimaryMonitor(), NULL);
	if (_window == NULL)
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
}

GlfwWindowProxy::~GlfwWindowProxy()
{
	glfwDestroyWindow(_window);
	deinit();
}

GLFWwindow		*GlfwWindowProxy::get_window()
{
	return (_window);
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
	if (entered == GLFW_TRUE)
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
	if (focused == GLFW_TRUE)
		get_instance(w)->on_window_focus();
	else
		get_instance(w)->on_window_defocus();
}

void			GlfwWindowProxy::c_win_iconify(GLFWwindow *w, int iconified)
{
	if (iconified == GLFW_TRUE)
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
