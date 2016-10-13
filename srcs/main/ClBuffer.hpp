// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClBuffer.hpp                                       :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/13 12:02:40 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/13 13:49:33 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef CLBUFFER_HPP
# define CLBUFFER_HPP

# include "ft/cl.h"

/*
** OpenCL buffer
*/
template<typename T>
class	ClBuffer
{
public:
	ClBuffer(cl_context context, size_t size, bool read_only=false);

	ClBuffer(cl_mem handle);

	ClBuffer(ClBuffer &&src);

	virtual ~ClBuffer();

	cl_mem			get_handle();

private:
	cl_mem			_handle;

private:
	ClBuffer() = delete;
	ClBuffer(ClBuffer const &src) = delete;
	ClBuffer		&operator=(ClBuffer &&rhs) = delete;
	ClBuffer		&operator=(ClBuffer const &rhs) = delete;
};

# include "ClBuffer.tpp"

#endif
