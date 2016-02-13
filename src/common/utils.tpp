/*---------------------------------------------------------------------------*\
 *
 *  bitpit
 *
 *  Copyright (C) 2015-2016 OPTIMAD engineering Srl
 *
 *  -------------------------------------------------------------------------
 *  License
 *  This file is part of bitbit.
 *
 *  bitpit is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License v3 (LGPL)
 *  as published by the Free Software Foundation.
 *
 *  bitpit is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with bitpit. If not, see <http://www.gnu.org/licenses/>.
 *
\*---------------------------------------------------------------------------*/

#ifndef __BITPIT_UTILS_TPP__
#define __BITPIT_UTILS_TPP__

/*! \file */

#include <algorithm>
#include <vector>

namespace bitpit {

namespace utils {

/*!
	\ingroup commonUtils

	Adds an id to an ordered list of unique ids.

	\tparam T is the type of elements contained in the list
	\tparam Comparator is the type of the binary function used for the
	comparison of the elements

	\param value is the value to be added
	\param list is the ordered list
	\param comparator is a binary function that accepts two arguments (the first
	of the type pointed by ForwardIterator, and the second, always val), and
	returns a value convertible to bool. The value returned indicates whether
	the first argument is considered to go before the second. The function
	shall not modify any of its arguments. This can either be a function
	pointer or a function object.
	\result Returns true is the id was added to the list, false otherwise.
*/
template <typename T, typename Comparator>
bool addToOrderedVector(const T &value, std::vector<T> &list, Comparator comparator)
{
	if (list.empty()) {
		list.push_back(value);
		return true;
	}

	typename std::vector<T>::iterator itr = std::lower_bound(list.begin(), list.end(), value, comparator);
	if (itr == list.end() || *itr != value) {
		list.insert(itr, value);
		return true;
	} else {
		return false;
	}
};

/*!
	\ingroup commonUtils

	Search a value in an ordered list of unique ids.

	\tparam T is the type of elements contained in the list
	\tparam Comparator is the type of the binary function used for the
	comparison of the elements

	\param value is the value to be searched for
	\param list is the ordered list
	\param comparator is a binary function that accepts two arguments (the first
	of the type pointed by ForwardIterator, and the second, always val), and
	returns a value convertible to bool. The value returned indicates whether
	the first argument is considered to go before the second. The function
	shall not modify any of its arguments. This can either be a function
	pointer or a function object.
	\result Returns true is the value is in the list, false otherwise.
*/
template <typename T, typename Comparator>
typename std::vector<T>::const_iterator findInOrderedVector(const T &value, const std::vector<T> &list, Comparator comparator)
{
	typename std::vector<T>::const_iterator itr = std::lower_bound(list.begin(), list.end(), value, comparator);
	if (itr == list.end() || *itr != value) {
		return list.end();
	}

	return itr;
};

}

}

#endif
