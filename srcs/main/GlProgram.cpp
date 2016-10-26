// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   GlProgram.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: juloo <juloo@student.42.fr>                +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/26 18:39:00 by juloo             #+#    #+#             //
//   Updated: 2016/10/26 18:58:13 by juloo            ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "GlProgram.hpp"
#include "f.hpp"

#include <initializer_list>
#include <stdexcept>

static GLuint	get_shader_obj(GlProgram::shader_source const &info)
{
	GLuint			shader;
	char			err_buffer[128];
	GLint			status;

	if ((shader = glCreateShader(info.type)) == 0)
		throw std::runtime_error("Invalid shader type %"_f(info.type));
	glShaderSource(shader, 1, &info.str, NULL);
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

static GLuint	get_shaders(
					std::initializer_list<GlProgram::shader_source> const
						&shader_infos)
{
	GLuint			shaders[shader_infos.size()];
	unsigned		i;

	i = 0;
	try
	{
		for (auto info : shader_infos)
			shaders[i++] = get_shader_obj(info);
		return (link_shaders(shaders, i));
	}
	catch (std::runtime_error const &e)
	{
		while (i > 0)
			glDeleteShader(shaders[--i]);
		throw;
	}
}

GlProgram::GlProgram(std::initializer_list<shader_source> srcs) :
	_handle(get_shaders(srcs))
{}

GlProgram::~GlProgram()
{}

GLuint			GlProgram::get_handle() const
{ return (_handle); }

void			GlProgram::use()
{
	glUseProgram(_handle);
}
