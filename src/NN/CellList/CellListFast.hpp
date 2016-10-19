/*
 * CellListStandard.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: Pietro Incardona
 */

#ifndef CELLLISTSTANDARD_HPP_
#define CELLLISTSTANDARD_HPP_

#include "CellDecomposer.hpp"
#include "Space/SpaceBox.hpp"
#include "util/mathutil.hpp"
#include "CellNNIterator.hpp"
#include "Space/Shape/HyperCube.hpp"
#include "CellListIterator.hpp"
#include "CellListNNIteratorRadius.hpp"
#include <unordered_map>
#include "util/common.hpp"

//! Wrapper of the unordered map
template<typename key,typename val>
class wrap_unordered_map: public std::unordered_map<key,val>
{
};

#ifdef HAVE_LIBQUADMATH

#include <boost/multiprecision/float128.hpp>


//! Wrapper of the unordered map
template<typename val>
class wrap_unordered_map<boost::multiprecision::float128,val>
{
};

#endif

#define STARTING_NSLOT 16

/*! \brief Class for FAST cell list implementation
 *
 * This class implement the FAST cell list, fast but memory
 * expensive. The memory allocation is (M * N_cell_max)*sizeof(ele) + M*8
 *
 * * M = number of cells
 * * N_cell_max = maximum number of elements in a cell
 * * ele = element the structure is storing
 *
 * \note Because N_cell_max >= N/M then M * N_cell_max >= O(N)
 *
 * \warning Not not use for high asymmetric distribution
 *
 * Example of a 2D Cell list 6x6 structure with padding 1 without shift, cell indicated with p are padding cell
 * the origin of the cell or point (0,0) is marked with cell number 9
 *
 * \verbatim
 * +-----------------------+
 * |p |p |p |p |p |p |p |p |
 * +-----------------------+
 * |p |  |  |  |  |  |  |p |
 * +-----------------------+
 * |p |  |  |  |  |  |  |p |
 * +-----------------------+
 * |p |  |  |  |  |  |  |p |
 * +-----------------------+
 * |p |9 |  |  |  |  |  |p |
 * +-----------------------+
 * |p |p |p |p |p |p |p |p |
 * +-----------------------+
 * \endverbatim
 *
 *
 * \tparam dim Dimensionality of the space
 * \tparam T type of the space float, double ...
 * \tparam base Base structure that store the information
 *
 * ### Declaration of a cell list
 * \snippet CellList_test.hpp Declare a cell list
 * ### Usage of cell list [CellS == CellList<3,double,FAST>]
 * \snippet CellList_test.hpp Usage of cell list
 * ### Remove one particle from each cell
 * \snippet CellList_test.hpp remove one particle from each cell
 * ### Usage of the neighborhood iterator
 * \snippet CellList_test.hpp Usage of the neighborhood iterator
 *
 */
template<unsigned int dim, typename T, typename transform, typename base>
class CellList<dim,T,FAST,transform,base> : public CellDecomposer_sm<dim,T,transform>
{
protected:
	//! The array contain the neighborhood of the cell-id in case of asymmetric interaction
	//
	//    * * *
	//    * x *
	//    * * *

	long int NNc_full[openfpm::math::pow(3,dim)];

	//! The array contain the neighborhood of the cell-id in case of symmetric interaction
	//
	//   * * *
	//     x *
	//
	long int NNc_sym[openfpm::math::pow(3,dim)/2+1];

	//! The array contain the neighborhood of the cell-id in case of symmetric interaction (Optimized)
	//
	//   * *
	//   x *
	//
	long int NNc_cr[openfpm::math::pow(2,dim)];

private:

	//! Number of slot for each cell
	size_t slot;

	//! number of particle in each cell list
	openfpm::vector<size_t> cl_n;

	//! elements that each cell store (each cell can store a number
	//! of elements == slot )
	base cl_base;

	//! Caching of r_cutoff radius
	wrap_unordered_map<T,openfpm::vector<long int>> rcache;

	/*! \brief realloc the data structures
	 *
	 *
	 */
	void realloc()
	{
		// we do not have enough slots reallocate the basic structure with more
		// slots
		base cl_base_(2*slot * cl_n.size());

		// copy cl_base
		for (size_t i = 0 ; i < cl_n.size() ; i++)
		{
			for (size_t j = 0 ; j < cl_n.get(i) ; j++)
				cl_base_.get(2*i*slot + j) = cl_base.get(slot * i + j);
		}

		// Double the number of slots
		slot *= 2;

		// swap the memory
		cl_base.swap(cl_base_);
	}

	/*! Calculate the neighborhood cells based on the radius
	 *
	 * \note To the calculated neighborhood cell you have to add the id of the central cell
	 *
		\verbatim
       +-----------------------+
       |p |p |p |p |p |p |p |p |
       +-----------------------+
       |p |  |  |  |  |  |  |p |
       +-----------------------+
       |p |  |  |7 |8 |9 |  |p |
       +-----------------------+
       |p |  |  |-1|0 |1 |  |p |
       +-----------------------+
       |p |9 |  |-9|-8|-7|  |p |
       +-----------------------+
       |p |p |p |p |p |p |p |p |
       +-----------------------+
		\endverbatim
	 *
	 * The number indicate the cell id calculated
	 *
	 * -9,-8,-7,-1,0,1,7,8,9
	 *
	 * The cell 0 has id = 22 in the big cell matrix, so to calculate the
	 * neighborhood cells you have to sum the id of the center cell
	 *
	 * 13,14,15,21,22,23,29,30,31
	 *
	 * \param r_cut Cutoff-radius
	 * \param NNcell vector containing the neighborhood cells ids
	 *
	 */
	void NNcalc(T r_cut, openfpm::vector<long int> & NNcell)
	{
		size_t n_cell[dim];
		size_t n_cell_mid[dim];

		Point<dim,T> spacing = this->getCellBox().getP2();
		const grid_sm<dim,void> & gs = this->getGrid();

		for (size_t i = 0 ; i < dim ; i++)
		{
			n_cell[i] = 2*(std::ceil(r_cut / spacing.get(i)))+1;
			n_cell_mid[i] = n_cell[i] / 2;
		}

		grid_sm<dim,void> gsc(n_cell);
		grid_key_dx_iterator<dim> gkdi(gsc);

		while (gkdi.isNext())
		{
			auto key = gkdi.get();

			for (size_t i = 0 ; i < dim ; i++)
				key.set_d(i,key.get(i) - n_cell_mid[i]);

			NNcell.add(gs.LinId(key));

			++gkdi;
		}
	}

	//! Initialize the structures of the data structure
	void InitializeStructures(const size_t (& div)[dim], size_t tot_n_cell)
	{
		cl_n.resize(tot_n_cell);
		cl_n.fill(0);

		// create the array that store the cell id

		cl_base.resize(tot_n_cell * slot);

		// Calculate the NNc_full array, it is a structure to get the neighborhood array

		// compile-time array {0,0,0,....}  {2,2,2,...} {1,1,1,...}

		typedef typename generate_array<size_t,dim, Fill_zero>::result NNzero;
		typedef typename generate_array<size_t,dim, Fill_two>::result NNtwo;
		typedef typename generate_array<size_t,dim, Fill_one>::result NNone;

		// Generate the sub-grid iterator

		grid_sm<dim,void> gs(div);
		grid_key_dx_iterator_sub<dim> gr_sub3(gs,NNzero::data,NNtwo::data);

		// Calculate the NNc array

		size_t middle = gs.LinId(NNone::data);
		size_t i = 0;
		while (gr_sub3.isNext())
		{
			NNc_full[i] = (long int)gs.LinId(gr_sub3.get()) - middle;

			++gr_sub3;
			i++;
		}

		// Calculate the NNc_sym array

		i = 0;
		gr_sub3.reset();
		while (gr_sub3.isNext())
		{
			auto key = gr_sub3.get();

			size_t lin = gs.LinId(key);

			// Only the first half is considered
			if (lin < middle)
			{
				++gr_sub3;
				continue;
			}

			NNc_sym[i] = lin - middle;

			++gr_sub3;
			i++;
		}

		// Calculate the NNc_cross array

		i = 0;
		grid_key_dx_iterator_sub<dim> gr_sub2(gs,NNzero::data,NNone::data);

		while (gr_sub2.isNext())
		{
			auto key = gr_sub2.get();

			NNc_cr[i] = (long int)gs.LinId(key);

			++gr_sub2;
			i++;
		}
	}

public:

	//! Object type that the structure store
	typedef typename base::value_type value_type;

	//! Type of the coordinate space (double float)
	typedef T stype;

	/*! \brief Return the underlying grid information of the cell list
	 *
	 * \return the grid infos
	 *
	 */
	const grid_sm<dim,void> & getGrid()
	{
		return CellDecomposer_sm<dim,T,transform>::getGrid();
	}

	/*! Initialize the cell list from a well-define Cell-decomposer
	 *
	 * In some cases is needed to have a Cell-list with Cells consistent
	 * with a well predefined CellDecomposer. In this case we use this function.
	 * Using this initialization the Cell-list maintain the Cells defined by this
	 * Cell-decomposer consistently
	 *
	 * \param cd_sm Cell-Decomposer
	 * \param dom_box domain box (carefully this is going to be adjusted)
	 * \param pad cell-list padding
	 * \param slot slots for each cell
	 *
	 */
	void Initialize(CellDecomposer_sm<dim,T,transform> & cd_sm, const Box<dim,T> & dom_box,const size_t pad = 1, size_t slot=STARTING_NSLOT)
	{
		size_t bc[dim];
		for (size_t i = 0 ; i < dim ; i++)	{bc[i] = NON_PERIODIC;}

		Box<dim,long int> bx = cd_sm.convertDomainSpaceIntoCellUnits(dom_box,bc);
//		Box<dim,T> bxd = cd_sm.convertCellUnitsIntoDomainSpace(bx);

		size_t div[dim];
		size_t div_big[dim];
		size_t div_w_pad[dim];
		size_t tot_cell = 1;

		for (size_t i = 0 ; i < dim ; i++)
		{
			div[i] = bx.getHigh(i) - bx.getLow(i);
			div_w_pad[i] = bx.getHigh(i) - bx.getLow(i) + 2*pad;
			tot_cell *= div_w_pad[i];
			div_big[i] = cd_sm.getDiv()[i] - 2*cd_sm.getPadding(i);
		}

		CellDecomposer_sm<dim,T,transform>::setDimensions(cd_sm.getDomain(),div_big,div, pad, bx.getP1());

		// here we set the cell-shift for the CellDecomposer
//		Initialize(cd_sm.getDomain(),div,bx.getP1(),pad,slot);

		InitializeStructures(div_w_pad,tot_cell);
	}

	/*! Initialize the cell list
	 *
	 * \param box Domain where this cell list is living
	 * \param div grid size on each dimension
	 * \param pad padding cell
	 * \param slot maximum number of slot
	 *
	 */
	void Initialize(const Box<dim,T> & box, const size_t (&div)[dim], const size_t pad = 1, size_t slot=STARTING_NSLOT)
	{
		SpaceBox<dim,T> sbox(box);

		// Initialize point transformation

		Initialize(sbox,div,pad,slot);
	}

	/*! Initialize the cell list constructor
	 *
	 * \param box Domain where this cell list is living
	 * \param div grid size on each dimension
	 * \param pad padding cell
	 * \param slot maximum number of slot
	 *
	 */
	void Initialize(const SpaceBox<dim,T> & box, const size_t (&div)[dim], const size_t pad = 1, size_t slot=STARTING_NSLOT)
	{

		Matrix<dim,T> mat;

		CellDecomposer_sm<dim,T,transform>::setDimensions(box,div, mat, pad);
		this->slot = slot;

		// create the array that store the number of particle on each cell and se it to 0
		InitializeStructures(this->gr_cell.getSize(),this->gr_cell.size());

	}

	//! Default Constructor
	CellList()
	:slot(STARTING_NSLOT)
	{};

	//! Copy constructor
	CellList(const CellList<dim,T,FAST,transform,base> & cell)
	:slot(STARTING_NSLOT)
	{
		this->operator=(cell);
	}

	//! Copy constructor
	CellList(CellList<dim,T,FAST,transform,base> && cell)
	:slot(STARTING_NSLOT)
	{
		this->operator=(cell);
	}


	/*! \brief Cell list constructor
	 *
	 * \param box Domain where this cell list is living
	 * \param div grid size on each dimension
	 * \param mat Matrix transformation
	 * \param pad Cell padding
	 * \param slot maximum number of slot
	 *
	 */
	CellList(Box<dim,T> & box, const size_t (&div)[dim], Matrix<dim,T> mat, const size_t pad = 1, size_t slot=STARTING_NSLOT)
	:slot(slot),CellDecomposer_sm<dim,T,transform>(box,div,mat,box.getP1(),pad)
	{
		SpaceBox<dim,T> sbox(box);
		Initialize(sbox,div,pad,slot);
	}

	/*! \brief Cell list constructor
	 *
	 * \param box Domain where this cell list is living
	 * \param div grid size on each dimension
	 * \param pad Cell padding
	 * \param slot maximum number of slot
	 *
	 */
	CellList(Box<dim,T> & box, const size_t (&div)[dim], const size_t pad = 1, size_t slot=STARTING_NSLOT)
	:slot(slot)
	{
		SpaceBox<dim,T> sbox(box);
		Initialize(sbox,div,pad,slot);
	}

	/*! \brief Cell list constructor
	 *
	 * \param box Domain where this cell list is living
	 * \param div grid size on each dimension
	 * \param pad Cell padding
	 * \param slot maximum number of slot
	 *
	 */
	CellList(SpaceBox<dim,T> & box, const size_t (&div)[dim], const size_t pad = 1, size_t slot=STARTING_NSLOT)
	:slot(slot)
	{
		Initialize(box,div,pad,slot);
	}

	/*! \brief Cell list constructor from a cell decomposer
	 *
	 * \see Initialize
	 *
	 * \param cd_sm Cell-Decomposer
	 * \param box domain box (carefully this is going to be adjusted)
	 * \param pad Cell list padding
	 * \param slot number of slot for each cell
	 *
	 */
	CellList(CellDecomposer_sm<dim,T,transform> & cd_sm, const Box<dim,T> & box, const size_t pad = 1, size_t slot=STARTING_NSLOT)
	:slot(slot)
	{
		Initialize(cd_sm,box,pad,slot);
	}


	/*! \brief Destructor
	 *
	 *
	 */
	~CellList()
	{}

	/*! \brief Constructor from a temporal object
	 *
	 * \param cell Cell list structure
	 *
	 * \return itself
	 *
	 */
	CellList<dim,T,FAST,transform,base> & operator=(CellList<dim,T,FAST,transform,base> && cell)
	{
		std::copy(&cell.NNc_full[0],&cell.NNc_full[openfpm::math::pow(3,dim)],&NNc_full[0]);
		std::copy(&cell.NNc_sym[0],&cell.NNc_sym[openfpm::math::pow(3,dim)/2+1],&NNc_sym[0]);
		std::copy(&cell.NNc_cr[0],&cell.NNc_cr[openfpm::math::pow(2,dim)],&NNc_cr[0]);

		slot = cell.slot;

		cl_n.swap(cell.cl_n);
		cl_base.swap(cell.cl_base);

		static_cast<CellDecomposer_sm<dim,T,transform> &>(*this).swap(cell);

		return *this;
	}

	/*! \brief Constructor from a temporal object
	 *
	 * \param cell Cell list structure
	 *
	 * \return itself
	 *
	 */
	CellList<dim,T,FAST,transform,base> & operator=(const CellList<dim,T,FAST,transform,base> & cell)
	{
		std::copy(&cell.NNc_full[0],&cell.NNc_full[openfpm::math::pow(3,dim)],&NNc_full[0]);
		std::copy(&cell.NNc_sym[0],&cell.NNc_sym[openfpm::math::pow(3,dim)/2+1],&NNc_sym[0]);
		std::copy(&cell.NNc_cr[0],&cell.NNc_cr[openfpm::math::pow(2,dim)],&NNc_cr[0]);

		slot = cell.slot;

		cl_n = cell.cl_n;
		cl_base = cell.cl_base;

		static_cast<CellDecomposer_sm<dim,T,transform> &>(*this) = static_cast<const CellDecomposer_sm<dim,T,transform> &>(cell);

		return *this;
	}

	/*! \brief Add to the cell
	 *
	 * \param cell_id Cell id where to add
	 * \param ele element to add
	 *
	 */
	inline void addCell(size_t cell_id, typename base::value_type ele)
	{
		// Get the number of element the cell is storing

		size_t nl = getNelements(cell_id);

		if (nl + 1 >= slot)
		{
			realloc();
		}

		// we have enough slot to store another neighbor element

		cl_base.get(slot * cell_id + cl_n.get(cell_id)) = ele;
		cl_n.get(cell_id)++;
	}

	/*! \brief Add an element in the cell list
	 *
	 * \param pos array that contain the coordinate
	 * \param ele element to store
	 *
	 */
	inline void add(const T (& pos)[dim], typename base::value_type ele)
	{
		// calculate the Cell id

		size_t cell_id = this->getCell(pos);

		// add the element to the cell

		addCell(cell_id,ele);
	}

	/*! \brief Add an element in the cell list
	 *
	 * \param pos array that contain the coordinate
	 * \param ele element to store
	 *
	 */
	inline void add(const Point<dim,T> & pos, typename base::value_type ele)
	{
		// calculate the Cell id

		size_t cell_id = this->getCell(pos);

		// add the element to the cell

		addCell(cell_id,ele);
	}

	/*! \brief remove an element from the cell
	 *
	 * \param cell cell id
	 * \param ele element id
	 *
	 */
	inline void remove(size_t cell, size_t ele)
	{
		cl_n.get(cell)--;
	}

	/*! \brief Return the number of elements in the cell
	 *
	 * \param cell_id id of the cell
	 *
	 * \return number of elements in the cell
	 *
	 */
	inline size_t getNelements(const size_t cell_id) const
	{
		return cl_n.get(cell_id);
	}

	/*! \brief Get an element in the cell
	 *
	 * \tparam i property to get
	 *
	 * \param cell cell id
	 * \param ele element id
	 *
	 * \return The element value
	 *
	 */
	inline auto get(size_t cell, size_t ele) -> decltype(cl_base.get(cell * slot + ele)) &
	{
		return cl_base.get(cell * slot + ele);
	}

	///////////////////////////////////// Temporal workaround  for GCC bug ////////////////////////
#if !defined(__APPLE_CC__) &&  GCC_VERSION <= 40902

	/*! \brief Get an element in the cell
	 *
	 * \tparam i property to get
	 *
	 * \param cell cell id
	 * \param ele element id
	 *
	 * \return The element value
	 *
	 */
	inline auto get(size_t cell, size_t ele) const -> decltype(cl_base.get(cell * slot + ele)) &
	{
		return cl_base.get(cell * slot + ele);
	}

	/*! \brief Get an element in the cell
	 *
	 * \tparam i property to get
	 *
	 * \param cell cell id
	 * \param ele element id
	 *
	 * \return The element value
	 *
	 */
	template<unsigned int i> inline auto get(size_t cell, size_t ele) const -> decltype(cl_base.get(cell * slot + ele)) &
	{
		return cl_base.template get<i>(cell * slot + ele);
	}

#else

	/*! \brief Get an element in the cell
	 *
	 * \tparam i property to get
	 *
	 * \param cell cell id
	 * \param ele element id
	 *
	 * \return The element value
	 *
	 */
	inline auto get(size_t cell, size_t ele) const -> decltype(cl_base.get(cell * slot + ele)) &
	{
		return cl_base.get(cell * slot + ele);
	}

	/*! \brief Get an element in the cell
	 *
	 * \tparam i property to get
	 *
	 * \param cell cell id
	 * \param ele element id
	 *
	 * \return The element value
	 *
	 */
	template<unsigned int i> inline auto get(size_t cell, size_t ele) const -> decltype(cl_base.get(cell * slot + ele)) &
	{
		return cl_base.template get<i>(cell * slot + ele);
	}

#endif


	/*! \brief Swap the memory
	 *
	 * \param cl Cell list with witch you swap the memory
	 *
	 */
	inline void swap(CellList<dim,T,FAST,transform,base> & cl)
	{
		cl_n.swap(cl.cl_n);
		cl_base.swap(cl.cl_base);

		long int NNc_full_tmp[openfpm::math::pow(3,dim)];
		long int NNc_sym_tmp[openfpm::math::pow(3,dim)];
		long int NNc_cr_tmp[openfpm::math::pow(3,dim)];

		std::copy(&cl.NNc_full[0],&cl.NNc_full[openfpm::math::pow(3,dim)],&NNc_full_tmp[0]);
		std::copy(&cl.NNc_sym[0],&cl.NNc_sym[openfpm::math::pow(3,dim)/2+1],&NNc_sym_tmp[0]);
		std::copy(&cl.NNc_cr[0],&cl.NNc_cr[openfpm::math::pow(2,dim)],&NNc_cr_tmp[0]);

		std::copy(&NNc_full[0],&NNc_full[openfpm::math::pow(3,dim)],&cl.NNc_full[0]);
		std::copy(&NNc_sym[0],&NNc_sym[openfpm::math::pow(3,dim)/2+1],&cl.NNc_sym[0]);
		std::copy(&NNc_cr[0],&NNc_cr[openfpm::math::pow(2,dim)],&cl.NNc_cr[0]);

		std::copy(&NNc_full_tmp[0],&NNc_full_tmp[openfpm::math::pow(3,dim)],&NNc_full[0]);
		std::copy(&NNc_sym_tmp[0],&NNc_sym_tmp[openfpm::math::pow(3,dim)/2+1],&NNc_sym[0]);
		std::copy(&NNc_cr_tmp[0],&NNc_cr_tmp[openfpm::math::pow(2,dim)],&NNc_cr[0]);

		size_t cl_slot_tmp = cl.slot;
		cl.slot = slot;
		slot = cl_slot_tmp;

		static_cast<CellDecomposer_sm<dim,T,transform> &>(*this) = static_cast<const CellDecomposer_sm<dim,T,transform> &>(cl);
	}

	/*! \brief Get the Cell iterator
	 *
	 * \param cell
	 *
	 * \return the iterator to the elements inside cell
	 *
	 */
	CellIterator<CellList<dim,T,FAST,transform,base>> getCellIterator(size_t cell)
	{
		return CellIterator<CellList<dim,T,FAST,transform,base>>(cell,*this);
	}

	/*! \brief Get the Neighborhood iterator
	 *
	 * It iterate across all the element of the selected cell and the near cells
	 *
	 *  \verbatim

	     * * *
	     * x *
	     * * *

	   \endverbatim
	 *
	 * * x is the selected cell
	 * * * are the near cell
	 *
	 * \param cell cell id
	 *
	 * \return An iterator across the neighhood particles
	 *
	 */
	template<unsigned int impl=NO_CHECK> inline CellNNIterator<dim,CellList<dim,T,FAST,transform,base>,FULL,impl> getNNIterator(size_t cell)
	{
		CellNNIterator<dim,CellList<dim,T,FAST,transform,base>,FULL,impl> cln(cell,NNc_full,*this);

		return cln;
	}

	/*! \brief Get the Neighborhood iterator
	 *
	 * It iterate across all the element of the selected cell and the near cells up to some selected radius
	 *
	 * \param cell cell id
	 * \param r_cut radius
	 *
	 * \return An iterator across the neighborhood particles
	 *
	 */
	template<unsigned int impl=NO_CHECK> inline CellNNIteratorRadius<dim,CellList<dim,T,FAST,transform,base>,impl> getNNIteratorRadius(size_t cell, T r_cut)
	{
		openfpm::vector<long int> & NNc = rcache[r_cut];

		if (NNc.size() == 0)
			NNcalc(r_cut,NNc);

		CellNNIteratorRadius<dim,CellList<dim,T,FAST,transform,base>,impl> cln(cell,NNc,*this);

		return cln;
	}

	/*! \brief Get the Neighborhood iterator
	 *
	 * It iterate across all the element of the selected cell and the near cells
	 *
	 *  \verbatim

	   * * *
	     x *

	   \endverbatim
	 *
	 * * x is the selected cell
	 * * * are the near cell
	 *
	 * \param cell cell id
	 * \param p particle id
	 * \param v vector of position of the particles
	 *
	 * \return An aiterator across the neighborhood particles
	 *
	 */
	template<unsigned int impl> inline CellNNIteratorSym<dim,CellList<dim,T,FAST,transform,base>,SYM,impl> getNNIteratorSym(size_t cell, size_t p, const openfpm::vector<Point<dim,T>> & v)
	{
		CellNNIteratorSym<dim,CellList<dim,T,FAST,transform,base>,SYM,impl> cln(cell,p,NNc_sym,*this,v);

		return cln;
	}


	/*! \brief Get the Neighborhood iterator
	 *
	 * It iterate across all the element of the selected cell and the near cells
	 *
	 *  \verbatim

	   * *
	   x *

	   \endverbatim
	 *
	 * * x is the selected cell
	 * * * are the near cell
	 *
	 * \param cell cell id
	 *
	 * \return an iterator across the neighborhood params
	 *
	 */
	template<unsigned int impl> inline CellNNIterator<dim,CellList<dim,T,FAST,transform,base>,CRS,impl> getNNIteratorCross(size_t cell)
	{
		CellNNIterator<dim,CellList<dim,T,FAST,transform,base>,CRS,impl> cln(cell,NNc_cr,*this);

		return cln;
	}

	/*! \brief Return the number of padding cells of the Cell decomposer
	 *
	 * \param i dimension
	 *
	 * \return the number of padding cells
	 *
	 */
	size_t getPadding(size_t i)
	{
		return CellDecomposer_sm<dim,T,transform>::getPadding(i);
	}

	/*! \brief Clear the cell list
	 *
	 */
	void clear()
	{
		for (size_t i = 0 ; i < cl_n.size() ; i++)
			cl_n.get(i) = 0;
	}

	/*! \brief Given a cell it return the starting point of the cell
	 *
	 * \param cell_id cell id
	 *
	 * \return the start index
	 *
	 */
	inline size_t getStartId(size_t cell_id)
	{
		return cell_id*slot;
	}

	/*! \brief Given a cell it return the end point of the cell
	 *
	 * \param cell_id cell id
	 *
	 * \return the stop index
	 *
	 */
	inline size_t getStopId(size_t cell_id)
	{
		return cell_id*slot+cl_n.get(cell_id);
	}

	/*! \brief Return the neighborhood id
	 *
	 * \param part_id particle id
	 *
	 * \return the neighborhood id
	 *
	 */
	inline size_t & get_lin(size_t part_id)
	{
		return cl_base.get(part_id);
	}

//////////////////////////////// POINTLESS BUT REQUIRED TO RESPECT THE INTERFACE //////////////////

	//! Ghost marker
	size_t g_m = 0;

	/*! \brief return the ghost marker
	 *
	 * \return ghost marker
	 *
	 */
	inline size_t get_gm()
	{
		return g_m;
	}

	/*! \brief Set the ghost marker
	 *
	 * \param g_m marker
	 *
	 */
	inline void set_gm(size_t g_m)
	{
		this->g_m = g_m;
	}

/////////////////////////////////////
};


#endif /* CELLLISTSTANDARD_HPP_ */
