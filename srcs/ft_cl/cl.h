/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cl.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/04 13:58:48 by jaguillo          #+#    #+#             */
/*   Updated: 2016/10/04 14:00:53 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT__CL_H
# define FT__CL_H

# ifdef FT_CL_MAC_OS
#  include <OpenCL/opencl.h>

# else
#  include <CL/cl.h>

# endif

#endif
