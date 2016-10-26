// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   GlUniform.tpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: juloo <juloo@student.42.fr>                +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/26 19:03:55 by juloo             #+#    #+#             //
//   Updated: 2016/10/26 19:21:40 by juloo            ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef GLUNIFORM_TPP
# define GLUNIFORM_TPP

# include "ClContextProxy.hpp" // TODO: remove
# include "gl_utils.hpp"

template<typename T, size_t SIZE>
GlUniform<T, SIZE>::GlUniform(GLuint program, char const *name) :
	_program(program)
{
	if ((_loc = glGetUniformLocation(program, name)) < 0)
		throw std::runtime_error("Unknown uniform %"_f(name));
}

template<typename T, size_t SIZE>
GlUniform<T, SIZE>::GlUniform(GlProgram const &program, char const *name) :
	GlUniform(program.get_handle(), name)
{}

template<typename T, size_t SIZE>
GlUniform<T, SIZE>::~GlUniform()
{}

template<typename T, size_t SIZE>
GLuint			GlUniform<T, SIZE>::get_location()
{
	return (_loc);
}

template<typename T, size_t SIZE>
template<bool, typename>
void			GlUniform<T, SIZE>::operator=(T const &v)
{
	glUseProgram(_program);
	gl_utils::gl_type<T>::uniform_write(_loc, 1, &v);
}

template<typename T, size_t SIZE>
void			GlUniform<T, SIZE>::operator=(T const *v)
{
	glUseProgram(_program);
	gl_utils::gl_type<T>::uniform_write(_loc, SIZE, v);
}

#endif
