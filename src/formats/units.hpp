/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   units.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gsharony <gsharony@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 09:02:47 by gsharony          #+#    #+#             */
/*   Updated: 2022/04/09 13:53:58 by gsharony         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UNITS_HPP
# define UNITS_HPP

# include <iostream>
# include <string>
# include <vector>
# include <cstdlib>

#if __linux__
	# include <sys/types.h>
#endif

/* integers */
int				toInteger(std::string str);
int				toInteger(char c);
int				toInteger(char * str);
bool				isPositiveBase10(std::string num);
bool				isPositiveBase16(std::string num);

/* floats */
float 			toFloat(std::string str);

/* units */
ssize_t			fromSizeUnit(std::string num);
int				hexToInt(std::string num);;
std::string		intToHex(int num);
std::string		toBase62(int num);

#endif