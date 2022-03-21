/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   units.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gsharony <gsharony@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 09:02:47 by gsharony          #+#    #+#             */
/*   Updated: 2021/09/10 08:16:49 by gsharony         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UNITS_HPP
# define UNITS_HPP

# include <iostream>
# include <string>
# include <vector>

/* integers */
int 						toInteger(std::string str);
int 						toInteger(char c);
int 						toInteger(char * str);

/* floats */
float 						toFloat(std::string str);

/* units */
ssize_t						fromSizeUnit(std::string num);
int 						hexToInt(std::string num);;
std::string					intToHex(int num);

#endif