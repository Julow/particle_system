// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   GlfwWindowProxy.hpp                                :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/05 13:49:09 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/10 19:13:57 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef GLFWWINDOWPROXY_HPP
# define GLFWWINDOWPROXY_HPP

# include "ft/gl.h"
# include "opt.hpp"

# include <utility>

/*
** Manage the glfw library
** Proxy to a GLFWwindow object
** Should be extended and on_* functions overrided
** -
** Events are implemented that way because
** 	GLFWwindow only allow 1 custom pointer
*/
class	GlfwWindowProxy
{
public:
	GlfwWindowProxy(std::optional<std::pair<unsigned, unsigned>> win_size,
			char const *title,
			std::pair<unsigned, unsigned> gl_version = {4, 1});

	virtual ~GlfwWindowProxy();

	GLFWwindow		*get_window();

	unsigned		get_window_width();
	unsigned		get_window_height();

protected:
	/*
	** Cursor events
	*/
	virtual void	on_cursor_enter();
	virtual void	on_cursor_leave();
	virtual void	on_cursor_move(double xpos, double ypos);

	/*
	** Keyboard events
	*/
	virtual void	on_key_press(int key, int scancode, int mods);
	virtual void	on_key_release(int key, int scancode, int mods);
	virtual void	on_key_repeat(int key, int scancode, int mods);

	/*
	** Mouse events
	*/
	virtual void	on_mouse_button_press(int button, int mods);
	virtual void	on_mouse_button_release(int button, int mods);
	virtual void	on_mouse_scroll(double xoffset, double yoffset);

	/*
	** Window events
	** on_window_move			Position changed
	** on_window_resize			Size changed
	** on_window_close			Closed by user
	** on_window_focus			Gained focus
	** on_window_defocus		Lose focus
	** on_window_refresh		Refresh needed
	** on_window_iconify		Hidden
	** on_window_restore		Restored from iconified state
	** on_framebuffer_resize	Framebuffer resized
	*/
	virtual void	on_window_move(int x, int y);
	virtual void	on_window_resize(int width, int height);
	virtual void	on_window_close();
	virtual void	on_window_focus();
	virtual void	on_window_defocus();
	virtual void	on_window_refresh();
	virtual void	on_window_iconify();
	virtual void	on_window_restore();
	virtual void	on_framebuffer_resize(int width, int height);

private:
	GLFWwindow		*_window;

	unsigned		_width;
	unsigned		_height;

private: // static
	static void		init();
	static void		deinit();

	static void		c_cursor_enter(GLFWwindow*, int);
	static void		c_cursor_pos(GLFWwindow*, double, double);
	static void		c_key(GLFWwindow*, int, int, int, int);
	static void		c_mouse(GLFWwindow*, int, int, int);
	static void		c_scroll(GLFWwindow*, double, double);
	static void		c_win_close(GLFWwindow*);
	static void		c_win_focus(GLFWwindow*, int);
	static void		c_win_iconify(GLFWwindow*, int);
	static void		c_win_pos(GLFWwindow*, int, int);
	static void		c_win_refresh(GLFWwindow*);
	static void		c_win_size(GLFWwindow*, int, int);
	static void		c_win_framebuffer_size(GLFWwindow*, int, int);

	static unsigned	window_count;

private:
	GlfwWindowProxy() = delete;
	GlfwWindowProxy(GlfwWindowProxy &&src) = delete;
	GlfwWindowProxy(GlfwWindowProxy const &src) = delete;
	GlfwWindowProxy	&operator=(GlfwWindowProxy &&rhs) = delete;
	GlfwWindowProxy	&operator=(GlfwWindowProxy const &rhs) = delete;
};

#endif
