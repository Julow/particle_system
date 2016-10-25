#version 150 core
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   particle_render.vert                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/10/14 12:55:11 by jaguillo          #+#    #+#             */
/*   Updated: 2016/10/23 20:37:11 by juloo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

in vec4			buff_pos;
in vec4			buff_color;

uniform mat4	u_matrix;

out vec4		p_color;

#define PARTICLE_SIZE		1.f

void		main()
{
	vec4		p_pos = u_matrix * buff_pos;
	float		p_size = PARTICLE_SIZE / p_pos.w;

	p_color = buff_color * min(1.f, p_size);
	gl_Position = p_pos;
	gl_PointSize = max(1.f, p_size);
}
