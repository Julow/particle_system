// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   GlBuffer.tpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/08 17:49:46 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/08 19:12:22 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef GLBUFFER_TPP
# define GLBUFFER_TPP

# include "gl_utils.hpp"

template<typename T, typename ...ATT>
GlBuffer<T, ATT...>::GlBuffer(size_t size, T const *data)
{
	unsigned		i;

	glGenBuffers(1, &_handle);
	glBindBuffer(GL_ARRAY_BUFFER, _handle);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(T), data, GL_STATIC_DRAW);
	i = 0;
	(void)(int[]){(_init_attrib<ATT>(i++), 0)...};
}

template<typename T, typename ...ATT>
GlBuffer<T, ATT...>::~GlBuffer()
{
	glDeleteBuffers(1, &_handle);
}

template<typename T, typename ...ATT>
GLuint			GlBuffer<T, ATT...>::get_handle()
{
	return (_handle);
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
