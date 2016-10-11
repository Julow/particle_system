// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClGlBuffer.tpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/08 17:53:08 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/11 11:08:43 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef CLGLBUFFER_TPP
# define CLGLBUFFER_TPP

# ifndef __APPLE__
#  include <CL/cl_gl.h>

# endif

template<typename T, typename ...ATT>
ClGlBuffer<T, ATT...>::ClGlBuffer(cl_context c, size_t size, T const *data)
	: GlBuffer<T, ATT...>(size, data)
{
	cl_int			err;

	if ((_handle = clCreateFromGLBuffer(c, CL_MEM_READ_WRITE,
				get_gl_handle(), &err)) == NULL)
		cl_error(err, "clCreateFromGLBuffer");
}

template<typename T, typename ...ATT>
ClGlBuffer<T, ATT...>::~ClGlBuffer()
{
	clReleaseMemObject(_handle);
}

template<typename T, typename ...ATT>
typename ClGlBuffer<T, ATT...>::acquired
				ClGlBuffer<T, ATT...>::cl_acquire(cl_command_queue queue)
{
	return (ClGlBuffer<T, ATT...>::acquired(queue, _handle));
}

template<typename T, typename ...ATT>
GLuint			ClGlBuffer<T, ATT...>::get_gl_handle()
{
	return (GlBuffer<T, ATT...>::get_handle());
}

template<typename T, typename ...ATT>
cl_mem			ClGlBuffer<T, ATT...>::get_cl_handle()
{
	return (_handle);
}

template<typename T, typename ...ATT>
ClGlBuffer<T, ATT...>::acquired::acquired(cl_command_queue queue, cl_mem handle)
	: _queue(queue), _handle(handle)
{
	clEnqueueAcquireGLObjects(queue, 1, &handle, 0, NULL, NULL);
}

template<typename T, typename ...ATT>
ClGlBuffer<T, ATT...>::acquired::acquired(acquired &&src)
	: _queue(src._queue), _handle(src._handle)
{}

template<typename T, typename ...ATT>
ClGlBuffer<T, ATT...>::acquired::~acquired()
{
	clEnqueueReleaseGLObjects(_queue, 1, &_handle, 0, NULL, NULL);
}

template<typename T, typename ...ATT>
cl_mem			ClGlBuffer<T, ATT...>::acquired::get_handle()
{
	return (_handle);
}

#endif
