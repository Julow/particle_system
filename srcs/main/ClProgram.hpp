// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   ClProgram.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: juloo <juloo@student.42.fr>                +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/23 22:12:09 by juloo             #+#    #+#             //
//   Updated: 2016/10/26 18:00:43 by juloo            ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef CLPROGRAM_HPP
# define CLPROGRAM_HPP

# include "ft/cl.h"
# include <initializer_list>

class	ClProgram
{
public:
	ClProgram(cl_context context, std::initializer_list<char const*> srcs);
	ClProgram(ClProgram &&src);
	virtual ~ClProgram();

	cl_program			get_handle() const;

private:
	cl_program			_program;

	static cl_program	_get_program(cl_context context,
							std::initializer_list<char const*> srcs);

private:
	ClProgram(ClProgram const &src) = delete;
	ClProgram		&operator=(ClProgram &&rhs) = delete;
	ClProgram		&operator=(ClProgram const &rhs) = delete;
};

#endif
