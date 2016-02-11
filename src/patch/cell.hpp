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

#ifndef __BITPIT_CELL_HPP__
#define __BITPIT_CELL_HPP__

#include <memory>

#include "bitpit_containers.hpp"

#include "element.hpp"

namespace bitpit {
	class Cell;
	class Patch;
}

bitpit::IBinaryStream& operator>>(bitpit::IBinaryStream &buf, bitpit::Cell& cell);
bitpit::OBinaryStream& operator<<(bitpit::OBinaryStream &buf, const bitpit::Cell& cell);

namespace bitpit {

class Cell : public Element {

friend class Patch;

friend bitpit::OBinaryStream& (::operator<<) (bitpit::OBinaryStream& buf, const Cell& cell);
friend bitpit::IBinaryStream& (::operator>>) (bitpit::IBinaryStream& buf, Cell& cell);

public:
	Cell();
	Cell(const long &id, ElementInfo::Type type = ElementInfo::UNDEFINED);

	Cell(const Cell &other);
	Cell(Cell&& other) = default;
	Cell& operator = (const Cell &other);
	Cell& operator=(Cell&& other) = default;

	void initialize(ElementInfo::Type type, int nInterfacesPerFace = 0);

	bool isInterior() const;
	
	void initializeInterfaces(std::vector<std::vector<long>> &interfaces);
	void initializeEmptyInterfaces(const std::vector<int> interfaceCount);
	void setInterface(const int &face, const int &index, const long &interface);
	void pushInterface(const int &face, const long &interface);
	void deleteInterface(const int &face, const int &i);
	void unsetInterfaces();
	int getInterfaceCount() const;
	int getInterfaceCount(const int &face) const;
	long getInterface(const int &face, const int &index = 0) const;
	const long * getInterfaces() const;
	const long * getInterfaces(const int &face) const;

	void initializeAdjacencies(std::vector<std::vector<long>> &interfaces);
	void initializeEmptyAdjacencies(const std::vector<int> interfaceCount);
	void setAdjacency(const int &face, const int &index, const long &interface);
	void pushAdjacency(const int &face, const long &interface);
	void deleteAdjacency(const int &face, const int &i);
	void unsetAdjacencies();
	int getAdjacencyCount() const;
	int getAdjacencyCount(const int &face) const;
	long getAdjacency(const int &face, const int &index = 0) const;
	const long * getAdjacencies() const;
	const long * getAdjacencies(const int &face) const;

	void display(std::ostream &out, unsigned short int indent);

	unsigned int getBinarySize( );

protected:
	void setInterior(bool interior);

private:
	bool m_interior;

	bitpit::CollapsedVector2D<long> m_interfaces;
	bitpit::CollapsedVector2D<long> m_adjacencies;

};

extern template class PiercedVector<Cell>;
extern template class PositionalPiercedVector<Cell>;

}

#endif
