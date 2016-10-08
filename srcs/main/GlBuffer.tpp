// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   GlBuffer.tpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/08 17:49:46 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/08 18:03:44 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef GLBUFFER_TPP
# define GLBUFFER_TPP

// -
// TODO: move

template<GLenum TYPE_NAME, size_t SIZE>
struct	_gl_type
{
	constexpr static GLenum const	type_name = TYPE_NAME;
	constexpr static size_t const	size = SIZE;
};

template<typename T>
struct	gl_type {};

template<>
struct	gl_type<float> : _gl_type<GL_FLOAT, 1> {};

template<>
struct	gl_type<glm::vec3> : _gl_type<GL_FLOAT, 3> {};

// -

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
	using attr_t = gl_type<typename ATTR::v_type>;

	glVertexAttribPointer(index, attr_t::size, attr_t::type_name, GL_FALSE,
			sizeof(T), (GLvoid*)ATTR::offset());
	glEnableVertexAttribArray(index);
}

#endif
