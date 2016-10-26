// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   GlProgram.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: juloo <juloo@student.42.fr>                +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/26 18:25:53 by juloo             #+#    #+#             //
//   Updated: 2016/10/26 18:55:41 by juloo            ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef GLPROGRAM_HPP
# define GLPROGRAM_HPP

# include "ft/gl.h"
# include <initializer_list>

class	GlProgram
{
public:
	struct	shader_source
	{
		GLenum		type;
		char const	*str;
	};

	GlProgram(std::initializer_list<shader_source> srcs);
	virtual ~GlProgram();

	GLuint			get_handle() const;

	void			use();

private:
	GLuint			_handle;

private:
	GlProgram() = delete;
	GlProgram(GlProgram &&src) = delete;
	GlProgram(GlProgram const &src) = delete;
	GlProgram		&operator=(GlProgram &&rhs) = delete;
	GlProgram		&operator=(GlProgram const &rhs) = delete;
};

#endif
