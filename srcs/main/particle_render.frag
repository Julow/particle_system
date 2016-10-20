#version 410 core
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle_render.frag                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/20 15:52:06 by jaguillo          #+#    #+#             */
/*   Updated: 2016/10/20 19:02:30 by jaguillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

in vec4			p_color;

out vec4		color;

// void		main()
// {
// 	vec2		r = gl_PointCoord - vec2(0.5f, 0.5f);

// 	color = p_color;
// 	gl_FragDepth = (dot(r, r) > 0.5f*0.5f) ? 255.f : gl_FragCoord.z;
// }

void		main() { color = p_color; }
