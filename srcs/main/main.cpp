// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   main.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/04 13:50:05 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/18 16:04:15 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "ft/cl.h"
#include "ft/gl.h"

#include "ClContextProxy.hpp"
#include "ClKernel.hpp"
#include "GlfwWindowProxy.hpp"
#include "f.hpp"

#include <functional>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

__attribute__ ((noreturn))
static void		cl_error(cl_int err, char const *str)
{
	throw std::runtime_error("%: %"_f(str, ClContextProxy::strerror(err)));
}

/*
** ========================================================================== **
** CL: Get program
*/

static uint32_t	get_log_size(cl_program program, cl_device_id device)
{
	size_t			size;

	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
	return (size);
}

__attribute__ ((noreturn))
static void		program_build_error(cl_program program, cl_device_id device)
{
	uint32_t const	log_size = get_log_size(program, device);
	char			log[log_size];

	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
			log_size, log, NULL);
	clReleaseProgram(program);
	throw std::runtime_error("Failed to build program: %"_f
					(std::string(log, log_size)));
}

cl_program		get_program(cl_context context, char const *str)
{
	cl_program		program;
	cl_int			err;
	cl_device_id	device;

	if ((err = clGetContextInfo(context, CL_CONTEXT_DEVICES,
			sizeof(device), &device, NULL)) != CL_SUCCESS)
		cl_error(err, "clGetContextInfo");
	program = clCreateProgramWithSource(context, 1, &str, NULL, &err);
	if (err != CL_SUCCESS)
		cl_error(err, "clCreateProgramWithSource");
	err = clBuildProgram(program, 1, &device, "", NULL, NULL);
	if (err == CL_BUILD_PROGRAM_FAILURE)
		program_build_error(program, device);
	else if (err != CL_SUCCESS)
	{
		clReleaseProgram(program);
		cl_error(err, "clBuildProgram");
	}
	return (program);
}

/*
** ========================================================================== **
** CL: Get buffer
*/

cl_mem			get_buffer(cl_context context, cl_mem_flags flags, uint32_t size)
{
	cl_int			err;
	cl_mem			buff;

	buff = clCreateBuffer(context, flags, size, NULL, &err);
	if (err != CL_SUCCESS)
		cl_error(err, "clCreateBuffer");
	return (buff);
}

/*
** ========================================================================== **
** GL: Get shader
*/

#include "ft/gl.h"
#include <vector>

struct	shader_info
{
	GLenum			type;
	char const		*src;
};

static GLuint	get_shader_obj(shader_info const &info)
{
	GLuint			shader;
	char			err_buffer[128];
	GLint			status;

	if ((shader = glCreateShader(info.type)) == 0)
		throw std::runtime_error("Invalid shader type %"_f(info.type));
	glShaderSource(shader, 1, &info.src, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		glGetShaderInfoLog(shader, sizeof(err_buffer), NULL, err_buffer);
		glDeleteShader(shader);
		throw std::runtime_error("Shader compilation error: %"_f(err_buffer));
	}
	return (shader);
}

static GLuint	link_shaders(GLuint *shaders, unsigned count)
{
	GLuint			program;
	GLint			status;
	char			err_buffer[128];

	if ((program = glCreateProgram()) == 0)
		throw std::runtime_error("Failed to create shader program object");
	for (unsigned i = 0; i < count; i++)
		glAttachShader(program, shaders[i]);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		glGetProgramInfoLog(program, sizeof(err_buffer), NULL, err_buffer);
		glDeleteProgram(program);
		throw std::runtime_error("Failed to link program: %"_f(err_buffer));
	}
	for (unsigned i = 0; i < count; i++)
	{
		glDetachShader(program, shaders[i]);
		glDeleteShader(shaders[i]);
	}
	return (program);
}

GLuint			get_shaders(std::vector<shader_info> const &shader_infos)
{
	GLuint			shaders[shader_infos.size()];
	unsigned		i;

	i = 0;
	try
	{
		for (; i < shader_infos.size(); i++)
			shaders[i] = get_shader_obj(shader_infos[i]);
		return (link_shaders(shaders, i));
	}
	catch (std::runtime_error const &e)
	{
		while (i > 0)
			glDeleteShader(shaders[--i]);
		throw;
	}
}

/*
** ========================================================================== **
** Get uniform
*/

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
#include "GlBuffer.hpp"
#include "gen.h"
#include "particule.cl.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class	ParticleSystem
{
public:
	ParticleSystem(cl_context context, unsigned particle_count)
		: _particule_count(particle_count),
		_cl_context(context),

		_init_program(get_program(_cl_context, prog_particle_init)),
		_update_program(get_program(_cl_context, prog_particle_update)),
		_init_square_kernel(_init_program, "init_square"),
		_init_sphere_kernel(_init_program, "init_sphere"),
		_init_cube_kernel(_init_program, "init_cube"),
		_init_sphere_rand_kernel(_init_program, "init_rand_sphere"),
		_init_cube_rand_kernel(_init_program, "init_rand_cube"),

		_update_kernel(_update_program, "update"),
		_explode_kernel(_update_program, "explode"),

		_render_program(get_shaders({
				{GL_VERTEX_SHADER, prog_particle_render_vert},
				{GL_FRAGMENT_SHADER, prog_particle_render_frag},
			})),
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
		// _init_sphere_kernel.make_work<1>(_particule_count)
		// _init_cube_kernel.make_work<1>(_particule_count)
				// (queue, std::get<0>(p_vertices).get_handle(), _particle_infos.get_handle());
		// _init_sphere_rand_kernel.make_work<1>(_particule_count)
		_init_cube_rand_kernel.make_work<1>(_particule_count)
				(queue, std::get<0>(p_vertices).get_handle(), _particle_infos.get_handle(), std::clock());
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
		_update_kernel.make_work<1>(_particule_count)
				(queue, std::get<0>(p_vertices).get_handle(), _particle_infos.get_handle(),
						_center, delta_t);
	}

	void			render()
	{
		glUseProgram(_render_program);
		glBindVertexArray(_particle_vertices.get_gl_handle());
		glDrawArrays(GL_POINTS, 0, _particule_count);
		glBindVertexArray(0);
	}

private:
	unsigned		_particule_count;

	cl_context		_cl_context;

	cl_program			_init_program;
	cl_program			_update_program;
	ClKernel<cl_mem, cl_mem>	_init_square_kernel;
	ClKernel<cl_mem, cl_mem>	_init_sphere_kernel;
	ClKernel<cl_mem, cl_mem>	_init_cube_kernel;
	ClKernel<cl_mem, cl_mem, cl_uint>	_init_sphere_rand_kernel;
	ClKernel<cl_mem, cl_mem, cl_uint>	_init_cube_rand_kernel;
	ClKernel<cl_mem, cl_mem, cl_float4, cl_float>	_update_kernel;
	ClKernel<cl_mem, cl_mem, cl_float4, cl_float>	_explode_kernel;

	GLuint			_render_program;

	GlUniform<glm::mat4>	_uniform_matrix;

	ClGlBuffer<particule::p_vertex,
			attrib<particule::p_vertex, particule::vec4, &particule::p_vertex::pos>,
			attrib<particule::p_vertex, particule::vec4, &particule::p_vertex::color>
		>			_particle_vertices;

	ClBuffer<particule::p_info>	_particle_infos;

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
		GlfwWindowProxy(std::nullopt, "lol"),
		ClContextProxy(true),
		_particle_system(get_context(), 3000000),
		_cl_fps(),
		_gl_fps(),

		_hold_keys(0),

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
			{
				_particle_system.update(get_queue(), delta_t.count());
				clFinish(get_queue());
			}
			if (_cl_fps.end())
				_print_stats("\033[92mcl", _cl_fps.get_fps(), _cl_fps.get_stats());

			_gl_fps.begin();
			{ // render particles
				glClearColor(0.f, 0.f, 0.f, 1.f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				_particle_system.render();
				glFinish();
			}
			if (_gl_fps.end())
				_print_stats("\033[93mgl", _gl_fps.get_fps(), _gl_fps.get_stats());

			glfwSwapBuffers(get_window());

			_update_input(delta_t.count());
			glfwPollEvents();
		}
		_print_stats("\033[33mavg gl", _gl_fps.get_global_fps(), _gl_fps.get_global_stats());
		_print_stats("\033[32mavg cl", _cl_fps.get_global_fps(), _cl_fps.get_global_stats());
	}

protected:
	virtual void	on_key_press(int key, int, int)
	{
		auto const		f = _key_map.find({key, 0});

		if (f != _key_map.end())
			f->second->press(*this);
		else
			ft::f(std::cout, "KEY PRESS %\n", key);
	}

	virtual void	on_key_release(int key, int, int)
	{
		auto const		f = _key_map.find({key, 0});

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
		ft::f(std::cout, " % %%% fps; min=%%%; max=%%%; avg=%%%\033[0m\n", name,
				std::setprecision(3), std::fixed, fps,
				std::setprecision(6), std::fixed, min,
				std::setprecision(6), std::fixed, max,
				std::setprecision(6), std::fixed, avg);
	}

	bool			_update_look_at(float delta_t)
	{
		auto	rad_mod = [](float a){
			return (std::fmod(a, M_PI - M_PI/2.f));
		};

		if (!(_hold_keys & (HOLD_KEY_UP | HOLD_KEY_RIGHT
					| HOLD_KEY_DOWN | HOLD_KEY_LEFT)))
			return (false);
		if (_hold_keys & HOLD_KEY_UP)
			_look_at.x = rad_mod(_look_at.x + delta_t);
		if (_hold_keys & HOLD_KEY_RIGHT)
			_look_at.y = rad_mod(_look_at.y - delta_t);
		if (_hold_keys & HOLD_KEY_DOWN)
			_look_at.x = rad_mod(_look_at.x - delta_t);
		if (_hold_keys & HOLD_KEY_LEFT)
			_look_at.y = rad_mod(_look_at.y + delta_t);
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
