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

#include <unordered_map>

#include "patch.hpp"

namespace bitpit {

/*!
	\ingroup patch
	@{
*/

/*!
	\struct Adaption

	\brief The Adaption struct defines the information associated to an
	adaption.
*/


/*!
	\enum Adaption::Type

	\brief The Type enum defines the type of adaption that has been
	performed.
*/

/*!
	\enum Adaption::Entity

	\brief The Entity enum defines the type of entities on which the
	adaption has been performed.
*/

/*!
	\struct Info

	\brief The Info struct defines the information associated to an
	adaption.
*/

/*!
	@}
*/

/*!
	\ingroup patch
	@{
*/

/*!
	\class FlatMapping

	\brief The FlatMapping class allows to generate a mapping between an
	id-base numeration to a continuous-index numeration.
*/

/*!
	Default constructor.
*/
FlatMapping::FlatMapping()
	: m_patch(nullptr)
{
}

/*!
	Creates a new flat mapping.

	\param patch is the patch from witch the flat numbering will be built
*/
FlatMapping::FlatMapping(Patch *patch)
	: m_patch(patch)
{
}

/*!
	Default destructor.
*/
FlatMapping::~FlatMapping()
{
}

/*!
	Gets the numbering associated to the flat mapping.

	\result The numbering associated to the flat mapping.
*/
const std::vector<long> & FlatMapping::getNumbering() const
{
	return m_numbering;
}

/*!
	Gets the mapping associated to the flat mapping.

	\result The mapping associated to the flat mapping.
*/
const std::vector<long> & FlatMapping::getMapping() const
{
	return m_mapping;
}

/*!
	@}
*/

/*!
	\ingroup patch
	@{
*/

/*!
	\class CellFlatMapping

	\brief The CellFlatMapping class allows to generate a cell mapping
	between an id-base numeration to a continuous-index numeration.
*/

/*!
	Default constructor.
*/
CellFlatMapping::CellFlatMapping()
	: FlatMapping()
{
}

/*!
	Creates a new cell flat mapping.

	\param patch is the patch from witch the flat numbering will be built
*/
CellFlatMapping::CellFlatMapping(Patch *patch)
	: FlatMapping(patch)
{
	m_numbering.reserve(m_patch->getCellCount());
	m_mapping.reserve(m_patch->getCellCount());

	long flatId = -1;
	for (const auto &cell : m_patch->cells()) {
		flatId++;

		m_numbering.emplace_back();
		long &cellId = m_numbering.back();
		cellId = cell.get_id();

		m_mapping.emplace_back();
		long &cellMapping = m_mapping.back();
		cellMapping = flatId;
	}
}

/*!
	Default destructor.
*/
CellFlatMapping::~CellFlatMapping()
{
}

/*!
	Updates the cell flat mapping.

	\param adaptionData is adaption data that will be used to update
	the mapping
*/
void CellFlatMapping::update(const std::vector<Adaption::Info> adaptionData)
{
	// Previous number of cells
	long nPreviousCells = m_numbering.size();

	// Current number of cells
	long nCurrentCells = m_patch->getCellCount();

	// Map for renumbering the elements
	std::unordered_map<long, long> backwardMap;
	for (auto &adaptionInfo : adaptionData) {
		if (adaptionInfo.entity != Adaption::ENTITY_CELL) {
			continue;
		}

		if (adaptionInfo.type == Adaption::TYPE_REFINEMENT) {
			long previousId = adaptionInfo.previous[0];
			for (const auto &currentId : adaptionInfo.current) {
				backwardMap.insert({{currentId, previousId}});
			}
		} else if (adaptionInfo.type == Adaption::TYPE_COARSENING) {
			long previousId = adaptionInfo.previous[0];
			long currentId  = adaptionInfo.current[0];
			backwardMap.insert({{currentId, previousId}});
		} else if (adaptionInfo.type == Adaption::TYPE_CREATION) {
			long previousId = nPreviousCells;
			long currentId  = adaptionInfo.current[0];
			backwardMap.insert({{currentId, previousId}});
		}
	}

	// Update the mapping up to the first change
	m_mapping.resize(nCurrentCells);
	m_numbering.resize(nCurrentCells);

	long flatId = -1;
	long firstChangedFlatId = -1;
	auto cellIterator = m_patch->cells().cbegin();
	while (cellIterator != m_patch->cells().cend()) {
		flatId++;
		if (flatId == nCurrentCells) {
			break;
		}

		long cellId = cellIterator->get_id();
		if (cellId == m_numbering[flatId]) {
			m_mapping[flatId] = flatId;
			cellIterator++;
			continue;
		}

		firstChangedFlatId = flatId;
		break;
	}

	// If there are no changes the mapping is updated
	if (firstChangedFlatId < 0 && nCurrentCells <= nPreviousCells) {
		return;
	}

	// Build a map for the previous numbering
	//
	// We only need the flat ids after the flat id with the first change.
	std::unordered_map<long, long> previousNumberingMap;
	for (long n = firstChangedFlatId; n < nPreviousCells; ++n) {
		previousNumberingMap.insert({{m_numbering[n], n}});
	}

	// Continue the update of the mapping
	flatId = firstChangedFlatId - 1;
	while (cellIterator != m_patch->cells().cend()) {
		flatId++;
		long currentId = cellIterator->get_id();

		long previousId;
		if (backwardMap.count(currentId) != 0) {
			previousId = backwardMap.at(currentId);
		} else {
			previousId = currentId;
		}

		long previousFlatId;
		if (previousId == m_numbering[flatId]) {
			previousFlatId = flatId;
		} else {
			long previousId;
			if (backwardMap.count(currentId) != 0) {
				previousId = backwardMap.at(currentId);
			} else {
				previousId = currentId;
			}

			if (previousNumberingMap.count(previousId) != 0) {
				previousFlatId = previousNumberingMap.at(previousId);
			} else {
				previousFlatId = Element::NULL_ID;
			}
		}

		m_numbering[flatId] = currentId;
		m_mapping[flatId]   = previousFlatId;

		cellIterator++;
	}
}

}
