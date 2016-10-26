// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   GlUniform.hpp                                      :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: juloo <juloo@student.42.fr>                +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/26 19:02:54 by juloo             #+#    #+#             //
//   Updated: 2016/10/26 19:07:12 by juloo            ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef GLUNIFORM_HPP
# define GLUNIFORM_HPP

# include "ft/gl.h"
# include "GlProgram.hpp"

template<typename T, size_t SIZE = 1>
class	GlUniform
{
public:
	GlUniform(GLuint program, char const *name);
	GlUniform(GlProgram const &program, char const *name);

	virtual ~GlUniform();

	GLuint			get_location();

	template<bool DUMMY = true,
		typename = std::enable_if_t<SIZE == 1 && DUMMY>>
	void			operator=(T const &v);
	void			operator=(T const *v);

private:
	GLuint			_loc;
	GLuint			_program;

private:
	GlUniform() = delete;
	GlUniform(GlUniform &&src) = delete;
	GlUniform(GlUniform const &src) = delete;
	GlUniform		&operator=(GlUniform &&rhs) = delete;
	GlUniform		&operator=(GlUniform const &rhs) = delete;
};

# include "GlUniform.tpp"

#endif
