// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   f.hpp                                              :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/04 14:59:05 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/06 17:20:54 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef F_HPP
# define F_HPP

#include <ostream>
#include <sstream>
#include <string>

namespace ft
{

/*
** ========================================================================== **
** Format util
** -
** `ft::f(format, args...)` or `"format"_f (args...)`
** 		Create a std::string from 'format'
** 			by replacing each '%' by the corresponding arg from 'args'
** 		'args' can be of any types that support ostream's '<<' operator
** `ft::f(out, format, args...)`
** 		Same as above but instead write into 'out'
*/

namespace f_impl
{

static inline constexpr
unsigned		next_fmt(char const *format)
{
	unsigned		next = 0;

	while (format[next] != '\0' && format[next] != '%')
		next++;
	return (next);
}

static inline
void			f_loop(std::ostream &out, char const *format)
{
	out << format;
}

static inline
void			f_loop_end(std::ostream &) {}

template<typename HEAD, typename ...TAIL>
void			f_loop_end(std::ostream &out, HEAD&& arg, TAIL&& ...tail)
{
	out.put(' ');
	out << arg;
	f_loop_end(out, std::forward<TAIL>(tail)...);
}

template<typename HEAD, typename ...TAIL>
void			f_loop(std::ostream &out, char const *format,
						   HEAD&& arg, TAIL&& ...tail)
{
	unsigned const	next = next_fmt(format);

	if (next > 0)
		out.write(format, next);
	if (format[next] != '%')
	{
		f_loop_end(out, std::forward<HEAD>(arg), std::forward<TAIL>(tail)...);
		return ;
	}
	out << arg;
	f_loop(out, format + next + 1, std::forward<TAIL>(tail)...);
}

};

template<typename ...ARGS>
std::string		f(char const *format, ARGS&& ...args)
{
	std::ostringstream	out;

	f_impl::f_loop(out, format, std::forward<ARGS>(args)...);
	return (out.str());
}

template<typename ...ARGS>
void			f(std::ostream &out, char const *format, ARGS&& ...args)
{
	f_impl::f_loop(out, format, std::forward<ARGS>(args)...);
}

/*
** -
*/

namespace f_impl
{

class	fmt_holder
{
public:
	constexpr fmt_holder(char const *fmt) : _fmt(fmt) {}

	template<typename ...ARGS>
	std::string		operator()(ARGS&& ...args)
		{ return (ft::f(_fmt, std::forward<ARGS>(args)...)); }

private:
	char const		*_fmt;
};

};

};

constexpr
ft::f_impl::fmt_holder operator ""_f(char const *fmt, unsigned long)
{
	return (ft::f_impl::fmt_holder(fmt));
}

#endif
