// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   GlBuffer.tpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/08 17:49:46 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/26 18:50:21 by juloo            ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef GLBUFFER_TPP
# define GLBUFFER_TPP

# include "gl_utils.hpp"

template<typename T, typename ...ATT>
GlBuffer<T, ATT...>::GlBuffer(size_t size, T const *data)
{
	unsigned		i;

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(T), data, GL_STATIC_DRAW);
	i = 0;
	(void)(int[]){(_init_attrib<ATT>(i++), 0)...};
	glBindVertexArray(0);
}

template<typename T, typename ...ATT>
GlBuffer<T, ATT...>::~GlBuffer()
{
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
}

template<typename T, typename ...ATT>
GLuint			GlBuffer<T, ATT...>::get_handle()
{
	return (_vao);
}

template<typename T, typename ...ATT>
void			GlBuffer<T, ATT...>::bind()
{
	glBindVertexArray(_vao);
}

template<typename T, typename ...ATT>
template<typename ATTR>
void			GlBuffer<T, ATT...>::_init_attrib(unsigned index)
{
	using attr_t = gl_utils::gl_type<typename ATTR::v_type>;

	glVertexAttribPointer(index, attr_t::size, attr_t::type_name, GL_FALSE,
			sizeof(T), (GLvoid*)ATTR::offset());
	glEnableVertexAttribArray(index);
}

#endif
