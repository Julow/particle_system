// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClGlBuffer.hpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/08 17:53:08 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/13 13:45:50 by jaguillo         ###   ########.fr       //
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
class	ClGlBuffer : GlBuffer<T, ATTRIBS...>, ClBuffer<T>
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

	static cl_mem	_get_cl_buffer(cl_context c, GLuint gl_handle);

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
	acquired(cl_command_queue queue, ClBuffer<T> &buff);
	acquired(acquired &&src);

	virtual ~acquired();

	ClBuffer<T>		&operator*();
	ClBuffer<T>		*operator->();

private:
	cl_command_queue	_queue;
	ClBuffer<T>			&_buff;

private:
	acquired() = delete;
	acquired(acquired const &src) = delete;
	acquired		&operator=(acquired &&rhs) = delete;
	acquired		&operator=(acquired const &rhs) = delete;
};

# include "ClGlBuffer.tpp"

#endif
