// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   GlBuffer.hpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/08 17:48:05 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/08 18:01:37 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef GLBUFFER_HPP
# define GLBUFFER_HPP

# include "ft/gl.h"
# include <glm/glm.hpp>

template<typename T, typename V_TYPE, V_TYPE T::* PTR>
struct	attrib // TODO: move
{
	typedef V_TYPE						v_type;

	static size_t		offset()
	{
		return (reinterpret_cast<size_t>(&(reinterpret_cast<T const*>(0)->*PTR)));
	}
};

/*
** OpenGL buffer
*/
template<typename T, typename ...ATTRIBS>
class	GlBuffer
{
public:
	GlBuffer(size_t size, T const *data = nullptr);

	virtual ~GlBuffer();

	GLuint			get_handle();

private:
	GLuint			_handle;

	template<typename ATTR>
	void			_init_attrib(unsigned index);

private:
	GlBuffer() = delete;
	GlBuffer(GlBuffer &&src) = delete;
	GlBuffer(GlBuffer const &src) = delete;
	GlBuffer		&operator=(GlBuffer &&rhs) = delete;
	GlBuffer		&operator=(GlBuffer const &rhs) = delete;
};

# include "GlBuffer.tpp"

#endif
