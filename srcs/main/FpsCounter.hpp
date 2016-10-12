// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   FpsCounter.hpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: jaguillo <jaguillo@student.42.fr>          +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2016/10/12 17:36:07 by jaguillo          #+#    #+#             //
//   Updated: 2016/10/12 17:51:02 by jaguillo         ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef FPSCOUNTER_HPP
# define FPSCOUNTER_HPP

# include <chrono>

/*
** Track FPS (frames per second)
**  and min/max/average duration of frames
** RES_RATIO is the number of update per second
*/
template<typename CLOCK, unsigned RES_RATIO = 10>
class	FpsCounter
{
public:
	typedef typename CLOCK::duration	duration_t;
	typedef typename CLOCK::time_point	time_point_t;

	FpsCounter() : _frames{}, _i(0), _frame_begin(), _last_update() {}

	virtual ~FpsCounter() {}

	/*
	** Begin of a frame
	*/
	void			begin(time_point_t const &now = CLOCK::now());

	/*
	** End of a frame
	** Return true when a RES_INTERVAL has been hit (RES_RATIO times per second)
	*/
	bool			end(time_point_t const &now = CLOCK::now());

	/*
	** Current FPS
	*/
	float			get_fps() const;

	/*
	** Min/max/average duration of the last second's frames
	**  (in seconds)
	*/
	std::tuple<float, float, float>	get_stats() const;

private:
	struct	frame
	{
		time_point_t	begin;
		unsigned		count;
		duration_t		min_t;
		duration_t		max_t;
		duration_t		total_t;
	};

	constexpr static const unsigned		BUFF_SIZE = RES_RATIO + 1;

	frame			_frames[BUFF_SIZE];
	unsigned		_i;

	time_point_t	_frame_begin;
	time_point_t	_last_update;

private:
	FpsCounter(FpsCounter &&src) = delete;
	FpsCounter(FpsCounter const &src) = delete;
	FpsCounter		&operator=(FpsCounter &&rhs) = delete;
	FpsCounter		&operator=(FpsCounter const &rhs) = delete;
};

# include "FpsCounter.tpp"

#endif
