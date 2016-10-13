// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClGlBuffer.tpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/08 17:53:08 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/13 13:45:39 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef CLGLBUFFER_TPP
# define CLGLBUFFER_TPP

# ifndef __APPLE__
#  include <CL/cl_gl.h>

# endif

template<typename T, typename ...ATT>
ClGlBuffer<T, ATT...>::ClGlBuffer(cl_context c, size_t size, T const *data) :
	GlBuffer<T, ATT...>(size, data),
	ClBuffer<T>(_get_cl_buffer(c, get_gl_handle()))
{}

template<typename T, typename ...ATT>
ClGlBuffer<T, ATT...>::~ClGlBuffer()
{}

template<typename T, typename ...ATT>
typename ClGlBuffer<T, ATT...>::acquired
				ClGlBuffer<T, ATT...>::cl_acquire(cl_command_queue queue)
{
	return (ClGlBuffer<T, ATT...>::acquired(queue, *this));
}

template<typename T, typename ...ATT>
GLuint			ClGlBuffer<T, ATT...>::get_gl_handle()
{
	return (GlBuffer<T, ATT...>::get_handle());
}

template<typename T, typename ...ATT>
cl_mem			ClGlBuffer<T, ATT...>::get_cl_handle()
{
	return (ClBuffer<T>::get_handle());
}

template<typename T, typename ...ATT>
ClGlBuffer<T, ATT...>::acquired::acquired(cl_command_queue queue,
			ClBuffer<T> &buff)
	: _queue(queue), _buff(buff)
{
	cl_mem const	handle = _buff.get_handle();

	clEnqueueAcquireGLObjects(queue, 1, &handle, 0, NULL, NULL);
}

template<typename T, typename ...ATT>
ClGlBuffer<T, ATT...>::acquired::acquired(acquired &&src)
	: _queue(src._queue), _buff(src._buff)
{}

template<typename T, typename ...ATT>
ClGlBuffer<T, ATT...>::acquired::~acquired()
{
	cl_mem const	handle = _buff.get_handle();

	clEnqueueReleaseGLObjects(_queue, 1, &handle, 0, NULL, NULL);
}

template<typename T, typename ...ATT>
ClBuffer<T>		&ClGlBuffer<T, ATT...>::acquired::operator*()
{
	return (_buff);
}

template<typename T, typename ...ATT>
ClBuffer<T>		*ClGlBuffer<T, ATT...>::acquired::operator->()
{
	return (&_buff);
}

template<typename T, typename ...ATT>
cl_mem			ClGlBuffer<T, ATT...>::_get_cl_buffer(cl_context c,
					GLuint gl_handle)
{
	cl_int			err;
	cl_mem			buff;

	buff = clCreateFromGLBuffer(c, CL_MEM_READ_WRITE, gl_handle, &err);
	if (buff == NULL)
		throw std::runtime_error("clCreateFromGLBuffer: %"_f
						(ClContextProxy::strerror(err)));
	return (buff);
}

#endif
