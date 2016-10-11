// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   gl_utils.hpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/08 19:02:11 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/11 12:41:12 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef GL_UTILS_HPP
# define GL_UTILS_HPP

# include "ft/gl.h"

# include <glm/glm.hpp>
# include <glm/gtc/type_ptr.hpp>

namespace gl_utils
{

template<GLenum TYPE_NAME, size_t SIZE>
struct	_gl_type
{
	constexpr static GLenum const	type_name = TYPE_NAME;
	constexpr static size_t const	size = SIZE;
};

template<typename T>
struct	gl_type {};

template<>
struct	gl_type<float> : _gl_type<GL_FLOAT, 1>
{
	static void		uniform_write(GLint loc, GLsizei count, float const *v)
	{ glUniform1fv(loc, count, v); }
};

template<>
struct	gl_type<glm::vec3> : _gl_type<GL_FLOAT, 3>
{
	static void		uniform_write(GLint loc, GLsizei count, glm::vec3 const *v)
	{ glUniform3fv(loc, count, reinterpret_cast<float const*>(v)); }
};

template<>
struct	gl_type<glm::vec4> : _gl_type<GL_FLOAT, 3>
{
	static void		uniform_write(GLint loc, GLsizei count, glm::vec4 const *v)
	{ glUniform4fv(loc, count, reinterpret_cast<float const*>(v)); }
};

template<>
struct	gl_type<glm::mat4> : _gl_type<GL_FLOAT, 16>
{
	static void		uniform_write(GLint loc, GLsizei count, glm::mat4 const *m)
	{ glUniformMatrix4fv(loc, count, false, reinterpret_cast<float const*>(m)); }
};

};

#endif
