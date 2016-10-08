// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClGlBuffer.hpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/08 17:53:08 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/08 18:04:19 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef CLGLBUFFER_HPP
# define CLGLBUFFER_HPP

# include "ft/cl.h"
# include "GlBuffer.hpp"

/*
** Shared buffer between OpenGL and OpenCL
*/
template<typename T, typename ...ATTRIBS>
class	ClGlBuffer : GlBuffer<T, ATTRIBS...>
{
public:
	class	acquired;

	ClGlBuffer(cl_context c, size_t size, T const *data = nullptr);

	virtual ~ClGlBuffer();

	acquired		cl_acquire(cl_command_queue queue);

	GLuint			get_gl_handle();
	cl_mem			get_cl_handle();

private:
	cl_mem			_handle;

private:
	ClGlBuffer() = delete;
	ClGlBuffer(ClGlBuffer &&src) = delete;
	ClGlBuffer(ClGlBuffer const &src) = delete;
	ClGlBuffer		&operator=(ClGlBuffer &&rhs) = delete;
	ClGlBuffer		&operator=(ClGlBuffer const &rhs) = delete;
};

/*
** Lock-like object to use the buffer with OpenCL
** ClGlBuffer::cl_acquire(queue)
*/
template<typename T, typename ...ATTRIBS>
class	ClGlBuffer<T, ATTRIBS...>::acquired
{
public:
	acquired(cl_command_queue queue, cl_mem handle);
	acquired(acquired &&src);

	virtual ~acquired();

	cl_mem			get_handle();

private:
	cl_command_queue	_queue;
	cl_mem				_handle;

private:
	acquired() = delete;
	acquired(acquired const &src) = delete;
	acquired			&operator=(acquired &&rhs) = delete;
	acquired			&operator=(acquired const &rhs) = delete;
};

# include "ClGlBuffer.tpp"

#endif
