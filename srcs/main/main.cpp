// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   main.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/04 13:50:05 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/10 18:11:07 by jaguillo         ###   ########.fr       //
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

template<typename T, size_t SIZE = 1>
class	GlUniform
{
public:
	GlUniform(GLuint program, char const *name)
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
		gl_utils::gl_type<T>::uniform_write(_loc, 1, &v);
	}

	void			operator=(T const *v)
	{
		gl_utils::gl_type<T>::uniform_write(_loc, SIZE, v);
	}

private:
	GLuint			_loc;

private:
	GlUniform() = delete;
	GlUniform(GlUniform &&src) = delete;
	GlUniform(GlUniform const &src) = delete;
	GlUniform		&operator=(GlUniform &&rhs) = delete;
	GlUniform		&operator=(GlUniform const &rhs) = delete;
};

/*
** ========================================================================== **
*/

#include "ClGlBuffer.hpp"
#include "GlBuffer.hpp"
#include "particule.cl.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern char const *const	cl_program_particle;

class	Main final : GlfwWindowProxy, ClContextProxy
{
public:
	Main() :
		GlfwWindowProxy(500, 500, "lol"),
		ClContextProxy(),

		particule_count(50),

		_particule_program(get_program(get_context(), cl_program_particle)),
		_init_kernel(_particule_program, "init"),
		_update_kernel(_particule_program, "update"),

		_shader_program(get_shaders({
				{GL_VERTEX_SHADER,
					"#version 410 core\n"
					""
					"layout (location = 0) in vec4		buff_pos;"
					"layout (location = 1) in vec4		buff_color;"
					""
					// "uniform mat4	u_m_proj;"
					// "uniform mat4	u_m_view;"
					""
					"out vec4		p_color;"
					""
					"void		main()"
					"{"
					"	p_color = buff_color;"
					"	gl_PointSize = 5;"
					// "	gl_Position = u_m_proj * u_m_view * vec4(buff_pos, 1.f);"
					"	gl_Position = buff_pos;"
					"}"
				},

				{GL_FRAGMENT_SHADER,
					"#version 410 core\n"
					""
					"in vec4		p_color;"
					"out vec4		color;"
					""
					"void		main()"
					"{"
					"	color = p_color;"

					"}"
				},
			})),

		_particules_buffer(get_context(), particule_count, nullptr)//,

		// _uniform_m_proj(_shader_program, "u_m_proj"),
		// _uniform_m_view(_shader_program, "u_m_view")

	{
		glViewport(0, 0, 500, 500);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_PROGRAM_POINT_SIZE);
	}

	virtual ~Main() {}

	unsigned const		particule_count;

	cl_program				_particule_program;
	ClKernel<cl_mem>		_init_kernel;
	ClKernel<cl_mem, cl_float4, cl_float>	_update_kernel;

	GLuint				_shader_program;

	ClGlBuffer<particule::particule,
				attrib<particule::particule, particule::vec4, &particule::particule::pos>,
				attrib<particule::particule, particule::vec4, &particule::particule::color>
			>			_particules_buffer;

	// GlUniform<glm::mat4>	_uniform_m_proj;
	// GlUniform<glm::mat4>	_uniform_m_view;

	void			loop()
	{
		{ // init particules
			auto		p_buffer = _particules_buffer.cl_acquire(get_queue());

			_init_kernel.make_work<1>(particule_count)(get_queue(), p_buffer.get_handle());
		}
		clFinish(get_queue());

		// _uniform_m_proj = glm::perspective(42.f, 1.f, 0.01f, 1000.f);
		// _uniform_m_view = glm::lookAt(
		// 		glm::vec3(0, 0, -1),
		// 		glm::vec3(0, 0, 0),
		// 		glm::vec3(0, 1, 0)
		// 	);

		while (!glfwWindowShouldClose(get_window()))
		{
			{ // update particules
				auto		p_buffer = _particules_buffer.cl_acquire(get_queue());

				_update_kernel.make_work<1>(particule_count)
						(get_queue(), p_buffer.get_handle(),
							{{0.f, 0.f, 0.f}}, 0.0005f);

			}
			clFinish(get_queue());

			glClearColor(0.f, 0.6f, 0.6f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(_shader_program);
			glBindVertexArray(_particules_buffer.get_gl_handle());
			glDrawArrays(GL_POINTS, 0, particule_count);
			glBindVertexArray(0);

			glfwSwapBuffers(get_window());
			glfwPollEvents();
		}

	}

private:

private:
	Main(Main &&src) = delete;
	Main(Main const &src) = delete;
	Main			&operator=(Main &&rhs) = delete;
	Main			&operator=(Main const &rhs) = delete;
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
	m->loop();
	delete m;
	return (0);
}
