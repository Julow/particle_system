// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   gl_utils.hpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/08 19:02:11 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/08 19:42:05 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef GL_UTILS_HPP
# define GL_UTILS_HPP

# include "ft/gl.h"
# include <glm/vec3.hpp>

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
	constexpr static void		(*uniform_write)(GLint, GLsizei, GLfloat const*) = glUniform1fv;
};

template<>
struct	gl_type<glm::vec3> : _gl_type<GL_FLOAT, 3>
{
	constexpr static void		(*uniform_write)(GLint, GLsizei, GLfloat const*) = glUniform3fv;
};

};

#endif
