// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClProgram.cpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: juloo <juloo@student.42.fr>                +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/23 22:18:00 by juloo             #+#    #+#             //
//   Updated: 2016/10/26 18:06:06 by juloo            ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include "ClContextProxy.hpp"
#include "ClProgram.hpp"
#include "f.hpp"

#include <stdexcept>

ClProgram::ClProgram(cl_context context,
		std::initializer_list<char const*> srcs) :
	_program(_get_program(context, srcs))
{}

ClProgram::ClProgram(ClProgram &&src) :
	_program(src._program)
{}

ClProgram::~ClProgram()
{
	clReleaseProgram(_program);
}

cl_program		ClProgram::get_handle() const
{
	return (_program);
}

static uint32_t	get_log_size(cl_program program, cl_device_id device)
{
	size_t			size;

	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
	return (size);
}

__attribute__ ((noreturn))
static void		program_build_error(cl_program program, cl_device_id device)
{
	uint32_t const	log_size = get_log_size(program, device);
	char			log[log_size];

	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
			log_size, log, NULL);
	clReleaseProgram(program);
	throw std::runtime_error("Failed to build program: %"_f
					(std::string(log, log_size)));
}

cl_program		ClProgram::_get_program(cl_context context,
					std::initializer_list<char const*> srcs)
{
	cl_program		program;
	cl_int			err;
	cl_device_id	device;

	if ((err = clGetContextInfo(context, CL_CONTEXT_DEVICES,
			sizeof(device), &device, NULL)) != CL_SUCCESS)
		throw std::runtime_error("clGetContextInfo: %"_f
				(ClContextProxy::strerror(err)));
	program = clCreateProgramWithSource(context, srcs.size(),
				const_cast<char const**>(srcs.begin()),
				NULL, &err);
	if (err != CL_SUCCESS)
		throw std::runtime_error("clCreateProgramWithSource: %"_f
				(ClContextProxy::strerror(err)));
	err = clBuildProgram(program, 1, &device, "", NULL, NULL);
	if (err == CL_BUILD_PROGRAM_FAILURE)
		program_build_error(program, device);
	else if (err != CL_SUCCESS)
	{
		clReleaseProgram(program);
		throw std::runtime_error("clBuildProgram: %"_f
				(ClContextProxy::strerror(err)));
	}
	return (program);
}
