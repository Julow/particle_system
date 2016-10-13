// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   main.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/04 13:50:05 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/13 16:27:05 by jaguillo         ###   ########.fr       //
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
** LookAtController
*/

class	LookAtController
{
public:
	enum flag
	{
		UP = 1 << 0,
		RIGHT = 1 << 1,
		DOWN = 1 << 2,
		LEFT = 1 << 3,
	};

	LookAtController()
		: _flags(0), _look_at(0.f, M_PI/2.f)
	{}

	virtual ~LookAtController() {}

	void				press(flag k) { _flags |= k; }
	void				release(flag k) { _flags &= ~k; }

	glm::vec3			get_look_at() const
	{
		return (glm::vec3(
				cos(_look_at.x) * cos(_look_at.y),
				sin(_look_at.x),
				cos(_look_at.x) * sin(-_look_at.y)
			));
	}

	bool				update(float delta_t)
	{
		glm::vec2			d(0.f, 0.f);

		if (!(_flags & (UP | RIGHT | DOWN | LEFT)))
			return (false);
		if (_flags & UP)
			d.x += 1.f;
		else if (_flags & RIGHT)
			d.y -= 1.f;
		else if (_flags & DOWN)
			d.x -= 1.f;
		else if (_flags & LEFT)
			d.y += 1.f;
		_look_at += d * delta_t;
		return (true);
	}

private:
	unsigned			_flags;

	glm::vec2			_look_at;

private:
	LookAtController(LookAtController &&src) = delete;
	LookAtController(LookAtController const &src) = delete;
	LookAtController	&operator=(LookAtController &&rhs) = delete;
	LookAtController	&operator=(LookAtController const &rhs) = delete;
};

/*
** ========================================================================== **
** ParticleSystem
*/

#include "ClBuffer.hpp"
#include "ClGlBuffer.hpp"
#include "GlBuffer.hpp"
#include "particule.cl.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern char const *const	cl_program_particle;

std::vector<shader_info>	gl_program_particle = {
	{GL_VERTEX_SHADER,
		"#version 410 core\n"
		""
		"layout (location = 0) in vec4		buff_pos;"
		"layout (location = 1) in vec4		buff_color;"
		""
		"uniform mat4	u_matrix;"
		""
		"out vec4		p_color;"
		"\n"
		"#define PARTICLE_SIZE		1.f\n"
		""
		"void		main()"
		"{"
		"	p_color = buff_color;"
		"	gl_Position = u_matrix * buff_pos;"
		""
		"	float		p_size = PARTICLE_SIZE / gl_Position.w;"
		""
		"	if (p_size < 1.f)"
		"	{"
		"		p_color *= p_size;"
		// "		p_color *= p_size * p_size;"
		// "		p_color *= sqrt(p_size);"
		"		gl_PointSize = 1.f;"
		"	}"
		"	else"
		"		gl_PointSize = p_size;"
		"}"
	},

	{GL_FRAGMENT_SHADER,
		"#version 410 core\n"
		"in vec4		p_color;"
		"out vec4		color;"
		"void		main() { color = p_color; }"
	},
};

class	ParticleSystem
{
public:
	ParticleSystem(cl_context context, unsigned particle_count)
		: _particule_count(particle_count),
		_cl_context(context),

		_update_program(get_program(_cl_context, cl_program_particle)),
		_init_square_kernel(_update_program, "init_square"),
		_init_sphere_kernel(_update_program, "init_sphere"),
		_init_cube_kernel(_update_program, "init_cube"),
		_update_kernel(_update_program, "update"),
		_explode_kernel(_update_program, "explode"),

		_render_program(get_shaders(gl_program_particle)),
		_uniform_matrix(_render_program, "u_matrix"),

		_particle_vertices(_cl_context, _particule_count, nullptr),
		_particle_infos(_cl_context, _particule_count)

	{}

	virtual ~ParticleSystem() {}

	void			init(cl_command_queue queue)
	{
		auto		p_vertices = _particle_vertices.cl_acquire(queue);

		// _init_square_kernel.make_work<1>(_particule_count)
		// _init_sphere_kernel.make_work<1>(_particule_count)
		_init_cube_kernel.make_work<1>(_particule_count)
				(queue, p_vertices->get_handle(), _particle_infos.get_handle());
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
		auto		p_vertices = _particle_vertices.cl_acquire(queue);

		if (_explode)
		{
			_explode_kernel.make_work<1>(_particule_count)
					(queue, p_vertices->get_handle(),
						_particle_infos.get_handle(),
						{{0.f, 0.f, 0.f}}, *_explode);
			_explode = std::nullopt;
		}
		_update_kernel.make_work<1>(_particule_count)
				(queue, p_vertices->get_handle(), _particle_infos.get_handle(),
						{{0.f, 0.f, 0.f}}, delta_t);
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

	cl_program			_update_program;
	ClKernel<cl_mem, cl_mem>	_init_square_kernel;
	ClKernel<cl_mem, cl_mem>	_init_sphere_kernel;
	ClKernel<cl_mem, cl_mem>	_init_cube_kernel;
	ClKernel<cl_mem, cl_mem, cl_float4, cl_float>	_update_kernel;
	ClKernel<cl_mem, cl_mem, cl_float4, cl_float>	_explode_kernel;

	GLuint			_render_program;

	GlUniform<glm::mat4>	_uniform_matrix;

	ClGlBuffer<particule::p_vertex,
			attrib<particule::p_vertex, particule::vec4, &particule::p_vertex::pos>,
			attrib<particule::p_vertex, particule::vec4, &particule::p_vertex::color>
		>			_particle_vertices;

	ClBuffer<particule::p_info>	_particle_infos;

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
#include <iomanip>

class	Main final : GlfwWindowProxy, ClContextProxy
{
public:
	typedef typename std::chrono::steady_clock	std_clock;

	Main() :
		GlfwWindowProxy(std::nullopt, "lol"),
		ClContextProxy(true),
		_look_at_controller(),
		_particle_system(get_context(), 200000),
		_cl_fps(),
		_gl_fps(),

		_m_proj(glm::perspective(65.f,
			get_window_width() / (float)get_window_height(), 0.01f, 1000.f)),
		_camera_dist(1.5f)

	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_PROGRAM_POINT_SIZE);
		update_camera();
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

			if (_look_at_controller.update(delta_t.count()))
				update_camera();
			glfwPollEvents();
		}
		_print_stats("\033[33mavg gl", _gl_fps.get_global_fps(), _gl_fps.get_global_stats());
		_print_stats("\033[32mavg cl", _cl_fps.get_global_fps(), _cl_fps.get_global_stats());
	}

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

	void			update_camera()
	{
		glm::mat4 const	m_view = glm::lookAt(
				_look_at_controller.get_look_at() * _camera_dist,
				glm::vec3(0.f, 0.f, 0.f),
				glm::vec3(0.f, 1.f, 0.f));

		_particle_system.set_matrix(_m_proj * m_view);
	}

	virtual void	on_key_press(int key, int, int)
	{
		auto const		f = _keys.find(key);

		if (f != _keys.end())
			_look_at_controller.press(f->second);
		else if (key == 75)
			_particle_system.explode(2.f);
	}

	virtual void	on_key_release(int key, int, int)
	{
		auto const		f = _keys.find(key);

		if (f != _keys.end())
			_look_at_controller.release(f->second);
	}

private:
	LookAtController		_look_at_controller;

	ParticleSystem			_particle_system;

	FpsCounter<std_clock, 1>	_cl_fps;
	FpsCounter<std_clock, 1>	_gl_fps;

	glm::mat4				_m_proj;
	float					_camera_dist;

	static std::map<int, LookAtController::flag> const	_keys;

private:
	Main(Main &&src) = delete;
	Main(Main const &src) = delete;
	Main			&operator=(Main &&rhs) = delete;
	Main			&operator=(Main const &rhs) = delete;
};

std::map<int, LookAtController::flag> const	Main::_keys = {
	{265, LookAtController::UP},
	{262, LookAtController::RIGHT},
	{264, LookAtController::DOWN},
	{263, LookAtController::LEFT},
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
