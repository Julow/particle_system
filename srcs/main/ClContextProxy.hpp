// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClContextProxy.hpp                                 :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/05 15:57:05 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/11 11:19:35 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef CLCONTEXTPROXY_HPP
# define CLCONTEXTPROXY_HPP

# include "ft/cl.h"

/*
** Proxy to cl_context object initialized with a single GPU device
**  and it's associated command queue
** -
** Support cl/gl interop
*/
class	ClContextProxy
{
public:
	ClContextProxy(bool gl_interop = false);

	virtual ~ClContextProxy();

	cl_context			get_context();
	cl_command_queue	get_queue();

	static char const	*strerror(cl_int err);

private:
	cl_context			_context;
	cl_command_queue	_queue;

private:
	ClContextProxy(ClContextProxy &&src) = delete;
	ClContextProxy(ClContextProxy const &src) = delete;
	ClContextProxy		&operator=(ClContextProxy &&rhs) = delete;
	ClContextProxy		&operator=(ClContextProxy const &rhs) = delete;
};

#endif
