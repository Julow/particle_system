// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   main.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/04 13:50:05 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/26 18:53:32 by juloo            ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "ft/cl.h"
#include "ft/gl.h"

#include "ClContextProxy.hpp"
#include "ClKernel.hpp"
#include "GlfwWindowProxy.hpp"
#include "f.hpp"

#include <ctime>
#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

/*
** ========================================================================== **
** Get uniform
*/

#include "GlProgram.hpp"
#include "gl_utils.hpp"

/*
** W: Change the current program (glUseProgram)
*/
template<typename T, size_t SIZE = 1>
class	GlUniform
{
public:
	GlUniform(GLuint program, char const *name)
		: _program(program)
	{
		if ((_loc = glGetUniformLocation(program, name)) < 0)
			throw std::runtime_error("Unknown uniform %"_f(name));
	}
	GlUniform(GlProgram const &program, char const *name) :
		GlUniform(program.get_handle(), name)
	{}

	virtual ~GlUniform()
	{}

	GLuint			get_location() { return (_loc); }

	template<bool DUMMY = true,
		typename = std::enable_if_t<SIZE == 1 && DUMMY>>
	void			operator=(T const &v)
	{
		glUseProgram(_program);
		gl_utils::gl_type<T>::uniform_write(_loc, 1, &v);
	}

	void			operator=(T const *v)
	{
		glUseProgram(_program);
		gl_utils::gl_type<T>::uniform_write(_loc, SIZE, v);
	}

private:
	GLuint			_loc;
	GLuint			_program;

private:
	GlUniform() = delete;
	GlUniform(GlUniform &&src) = delete;
	GlUniform(GlUniform const &src) = delete;
	GlUniform		&operator=(GlUniform &&rhs) = delete;
	GlUniform		&operator=(GlUniform const &rhs) = delete;
};

/*
** ========================================================================== **
** ParticleSystem
*/

#include "ClBuffer.hpp"
#include "ClGlBuffer.hpp"
#include "ClProgram.hpp"
#include "GlBuffer.hpp"
#include "gen.h"
#include "particle.cl.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class	ParticleSystem
{
public:
	ParticleSystem(cl_context context, unsigned particle_count)
		: _particule_count(particle_count),
		_cl_context(context),

		_update_program(_cl_context,
				{prog_particle_init, prog_particle_update}),
		_init_square_kernel(_update_program, "init_square"),
		_init_sphere_kernel(_update_program, "init_sphere"),
		_init_cube_kernel(_update_program, "init_cube"),
		_init_sphere_rand_kernel(_update_program, "init_rand_sphere"),
		_init_cube_rand_kernel(_update_program, "init_rand_cube"),

		_update_gravity_kernel(_update_program, "update_gravity"),
		_update_spring_kernel(_update_program, "update_spring"),
		_explode_kernel(_update_program, "explode"),

		_render_program({
				{GL_VERTEX_SHADER, prog_particle_render_vert},
				{GL_FRAGMENT_SHADER, prog_particle_render_frag},
			}),
		_uniform_matrix(_render_program, "u_matrix"),

		_particle_vertices(_cl_context, _particule_count, nullptr),
		_particle_infos(_cl_context, _particule_count),

		_center{{0.f, 0.f, 0.f, 0.f}}

	{}

	virtual ~ParticleSystem() {}

	void			init(cl_command_queue queue)
	{
		auto	p_vertices = cl_acquire(queue, _particle_vertices);

		// _init_square_kernel.make_work<1>(_particule_count)
		_init_sphere_kernel.make_work<1>(_particule_count)
		// _init_cube_kernel.make_work<1>(_particule_count)
				(queue, std::get<0>(p_vertices).get_handle(), _particle_infos.get_handle());
		// _init_sphere_rand_kernel.make_work<1>(_particule_count)
		// _init_cube_rand_kernel.make_work<1>(_particule_count)
				// (queue, std::get<0>(p_vertices).get_handle(), _particle_infos.get_handle(), std::clock());
	}

	void			set_center(glm::vec3 const &center)
	{
		_center = {{center.x, center.y, center.z, 0.f}};
	}

	void			set_matrix(glm::mat4 const &m)
	{
		_uniform_matrix = m;
	}

	void			explode(float force)
	{
		_explode = force;
	}

	void			update(cl_command_queue queue, float delta_t)
	{
		auto	p_vertices = cl_acquire(queue, _particle_vertices);

		if (_explode)
		{
			_explode_kernel.make_work<1>(_particule_count)
					(queue, std::get<0>(p_vertices).get_handle(),
						_particle_infos.get_handle(), _center, *_explode);
			_explode = std::nullopt;
		}
		_update_spring_kernel
		// _update_gravity_kernel
				.make_work<1>(_particule_count)
				(queue, std::get<0>(p_vertices).get_handle(), _particle_infos.get_handle(),
						_center, delta_t);
	}

	void			render()
	{
		_render_program.use();
		_particle_vertices.gl_bind();
		glDrawArrays(GL_POINTS, 0, _particule_count);
	}

private:
	unsigned		_particule_count;

	cl_context		_cl_context;

	ClProgram		_update_program;
	ClKernel<cl_mem, cl_mem>	_init_square_kernel;
	ClKernel<cl_mem, cl_mem>	_init_sphere_kernel;
	ClKernel<cl_mem, cl_mem>	_init_cube_kernel;
	ClKernel<cl_mem, cl_mem, cl_uint>	_init_sphere_rand_kernel;
	ClKernel<cl_mem, cl_mem, cl_uint>	_init_cube_rand_kernel;
	ClKernel<cl_mem, cl_mem, cl_float4, cl_float>	_update_gravity_kernel;
	ClKernel<cl_mem, cl_mem, cl_float4, cl_float>	_update_spring_kernel;
	ClKernel<cl_mem, cl_mem, cl_float4, cl_float>	_explode_kernel;

	GlProgram		_render_program;
	GlUniform<glm::mat4>	_uniform_matrix;

	ClGlBuffer<particle::p_vertex,
			attrib<particle::p_vertex, particle::float4, &particle::p_vertex::pos>,
			attrib<particle::p_vertex, particle::float4, &particle::p_vertex::color>
		>			_particle_vertices;

	ClBuffer<particle::p_info>	_particle_infos;

	cl_float4				_center;

	std::optional<float>	_explode;

private:
	ParticleSystem() = delete;
	ParticleSystem(ParticleSystem &&src) = delete;
	ParticleSystem(ParticleSystem const &src) = delete;
	ParticleSystem	&operator=(ParticleSystem &&rhs) = delete;
	ParticleSystem	&operator=(ParticleSystem const &rhs) = delete;
};

/*
** ========================================================================== **
*/

#include "FpsCounter.hpp"
#include "polymorphic_union.hpp"

#include <iomanip>

static float	get_window_ratio(GlfwWindowProxy const &w)
{
	unsigned		width, height;

	std::tie(width, height) = w.get_window_size();
	return (width / (float)height);
}

class	Main final : GlfwWindowProxy, ClContextProxy
{
public:
	typedef typename std::chrono::steady_clock	std_clock;

	Main() :
		GlfwWindowProxy(std::nullopt, "lol", {3, 2}),
		ClContextProxy(true),
		_particle_system(get_context(), 30000),
		_cl_fps(),
		_gl_fps(),

		_hold_keys(0),

		_pause(false),

		_m_proj(glm::perspective(90.f, get_window_ratio(*this), 0.01f, 1000.f)),

		_look_at(0.f, M_PI/2.f),
		_look_at_vec(0.f, 0.f, -1.f),
		_camera_dist(1.5f)

	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_PROGRAM_POINT_SIZE);
		_set_matrix();
	}

	virtual ~Main() {}

	void			init()
	{
		_particle_system.init(get_queue());
		clFinish(get_queue());
	}

	void			loop()
	{
		std_clock::time_point	last_frame;
		std_clock::time_point	now;
		std::chrono::duration<float, std::ratio<1>>	delta_t;

		last_frame = std_clock::now();
		while (!glfwWindowShouldClose(get_window()))
		{

			now = std_clock::now();
			delta_t = now - last_frame;
			last_frame = now;

			_cl_fps.begin(now);
			if (!_pause)
			{
				_particle_system.update(get_queue(), delta_t.count());
				clFinish(get_queue());
			}
			_cl_fps.end();

			_gl_fps.begin();
			{ // render particles
				glClearColor(0.f, 0.f, 0.f, 1.f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				_particle_system.render();
				glFinish();
			}
			if (_gl_fps.end())
			{
				_print_stats("\033[93mgl", _gl_fps.get_fps(), _gl_fps.get_stats());
				_print_stats("\033[92mcl", _cl_fps.get_fps(), _cl_fps.get_stats());
			}

			glfwSwapBuffers(get_window());

			_update_input(delta_t.count());
			glfwPollEvents();
		}
		_print_stats("\033[33mavg gl", _gl_fps.get_global_fps(), _gl_fps.get_global_stats());
		_print_stats("\033[32mavg cl", _cl_fps.get_global_fps(), _cl_fps.get_global_stats());
	}

protected:
	virtual void	on_key_press(int key, int, int mods)
	{
		auto const		f = _key_map.find({key, mods});

		if (f != _key_map.end())
			f->second->press(*this);
		else
			ft::f(std::cout, "KEY PRESS %\n", key);
	}

	virtual void	on_key_release(int key, int, int mods)
	{
		auto const		f = _key_map.find({key, mods});

		if (f != _key_map.end())
			f->second->release(*this);
	}

	virtual void	on_cursor_move(double x, double y)
	{
		unsigned		w_width, w_height;

		std::tie(w_width, w_height) = get_window_size();
		_particle_system.set_center({ // TODO: handle variable camera_dist
			(x / w_width * 2.f - 1.f) * -1.f * _camera_dist * (w_width / (float)w_height),
			(y / w_height * 2.f - 1.f) * -1.f * _camera_dist, 0.f});
	}

private:
	void			_set_matrix()
	{
		_particle_system.set_matrix(_m_proj * glm::lookAt(
					_look_at_vec * _camera_dist,
					glm::vec3(0.f, 0.f, 0.f),
					glm::vec3(0.f, 1.f, 0.f)
				));
	}

private:
	void			_print_stats(char const *name, float fps,
						std::tuple<float, float, float> const &stats)
	{
		float		min, max, avg;

		std::tie(min, max, avg) = stats;
		ft::f(std::cout, "%% fps: %%%; min/max/avg (ms): %%%%  %%%%  %%%%\033[0m\n",
				std::setw(11), std::string(name),
				std::setprecision(3), std::fixed, fps,
				std::setw(6), std::setprecision(1), std::fixed, min * 1000.f,
				std::setw(6), std::setprecision(1), std::fixed, max * 1000.f,
				std::setw(6), std::setprecision(1), std::fixed, avg * 1000.f);
	}

	bool			_update_look_at(float delta_t)
	{
		if (!(_hold_keys & (HOLD_KEY_UP | HOLD_KEY_RIGHT
					| HOLD_KEY_DOWN | HOLD_KEY_LEFT)))
			return (false);
		if (_hold_keys & HOLD_KEY_UP)
			_look_at.x = std::min(_look_at.x + delta_t, (float)M_PI/2.f);
		if (_hold_keys & HOLD_KEY_RIGHT)
			_look_at.y = _look_at.y - delta_t;
		if (_hold_keys & HOLD_KEY_DOWN)
			_look_at.x = std::max(_look_at.x - delta_t, (float)-M_PI/2.f);
		if (_hold_keys & HOLD_KEY_LEFT)
			_look_at.y = _look_at.y + delta_t;
		ft::f(std::cout, "X % Y %\n", _look_at.x, _look_at.y);
		return (true);
	}

	bool			_update_look_at_vec(float delta_t)
	{
		if (!_update_look_at(delta_t))
			return (false);
		_look_at_vec = glm::vec3(
			cos(_look_at.x) * cos(_look_at.y),
			sin(_look_at.x),
			cos(_look_at.x) * sin(-_look_at.y)
		);
		return (true);
	}

	bool			_update_camera_dist(float delta_t)
	{
		if (!(_hold_keys & (HOLD_KEY_BACK | HOLD_KEY_FRONT)))
			return (false);
		if (_hold_keys & HOLD_KEY_BACK)
			_camera_dist -= delta_t;
		if (_hold_keys & HOLD_KEY_FRONT)
			_camera_dist += delta_t;
		return (true);
	}

	bool			_update_viewproj(float delta_t)
	{
		if (!(_update_look_at_vec(delta_t) | _update_camera_dist(delta_t)))
			return (false);
		_set_matrix();
		return (true);
	}

	void			_update_input(float delta_t)
	{
		_update_viewproj(delta_t); // TODO: call only if needed
	}

private:
	enum	hold_key
	{
		HOLD_KEY_UP = 1 << 0,
		HOLD_KEY_RIGHT = 1 << 1,
		HOLD_KEY_DOWN = 1 << 2,
		HOLD_KEY_LEFT = 1 << 3,
		HOLD_KEY_BACK = 1 << 4,
		HOLD_KEY_FRONT = 1 << 5,
	};

	struct	_key_handler
	{
		virtual ~_key_handler() {}
		virtual void		press(Main&) const {};
		virtual void		release(Main&) const {};
	};

	struct	key_hold : _key_handler
	{
		key_hold(Main::hold_key k) :
			_k(k)
		{}

		virtual ~key_hold() {}

		virtual void		press(Main &m) const { m._hold_keys |= _k; };
		virtual void		release(Main &m)  const{ m._hold_keys &= ~_k; };

		Main::hold_key		_k;
	};

	struct	key_callback : _key_handler
	{
		typedef std::function<void (Main&)>		callback_t;

		key_callback(callback_t callback) :
			_callback(callback)
		{}

		virtual ~key_callback() {}

		virtual void		press(Main &m) const { _callback(m); };

		callback_t			_callback;
	};

	typedef polymorphic_union<_key_handler, key_hold, key_callback>	key_handler;

private:
	ParticleSystem			_particle_system;

	FpsCounter<std_clock, 1>	_cl_fps;
	FpsCounter<std_clock, 1>	_gl_fps;

	unsigned				_hold_keys;

	bool					_pause;

	glm::mat4				_m_proj;

	glm::vec2				_look_at;
	glm::vec3				_look_at_vec;
	float					_camera_dist;

	static std::map<std::pair<int, int>, key_handler> const	_key_map;

private:
	Main(Main &&src) = delete;
	Main(Main const &src) = delete;
	Main			&operator=(Main &&rhs) = delete;
	Main			&operator=(Main const &rhs) = delete;
};

std::map<std::pair<int, int>, Main::key_handler> const	Main::_key_map = {
	{{265, 0}, Main::key_hold(Main::HOLD_KEY_UP)},
	{{262, 0}, Main::key_hold(Main::HOLD_KEY_RIGHT)},
	{{264, 0}, Main::key_hold(Main::HOLD_KEY_DOWN)},
	{{263, 0}, Main::key_hold(Main::HOLD_KEY_LEFT)},
	{{83, 0}, Main::key_hold(Main::HOLD_KEY_FRONT)},
	{{87, 0}, Main::key_hold(Main::HOLD_KEY_BACK)},
	{{75, 0}, Main::key_callback([](Main &m){ m._particle_system.explode(2.f); })},
	{{76, 0}, Main::key_callback([](Main &m){ m._particle_system.explode(-1.f); })},
	{{81, 0}, Main::key_callback([](Main &m){ glfwSetWindowShouldClose(m.get_window(), true); })},
	{{32, 0}, Main::key_callback([](Main &m){ m._pause = !m._pause; })},
};

int				main()
{
	Main			*m;

	try
	{
		m = new Main();
	}
	catch (std::runtime_error const &e)
	{
		std::cout << "Init error: " << e.what() << std::endl;
		return (1);
	}
	m->init();
	m->loop();
	delete m;
	return (0);
}
