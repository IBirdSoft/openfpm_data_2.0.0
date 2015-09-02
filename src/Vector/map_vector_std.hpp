/*
 * map_vector_std.hpp
 *
 *  Created on: Mar 8, 2015
 *      Author: i-bird
 */

#ifndef MAP_VECTOR_STD_HPP_
#define MAP_VECTOR_STD_HPP_


/*! \brief Implementation of 1-D std::vector like structure
 *
 * this implementation is just a wrapper for the std::vector in the case
 * of data where the members cannot be parsed see openFPM_data wiki for more information
 *
 * ### Create add and access the elements
 * \snippet vector_test_util.hpp Create add and access stl
 *
 * \param T base type
 *
 */
template<typename T>
class vector<T,HeapMemory,grow_policy_double,STD_VECTOR>
{
	//! Actual size of the vector, warning: it is not the space allocated in grid
	//! grid size increase by a fixed amount every time we need a vector bigger than
	//! the actually allocated space
	size_t v_size;

	//! 1-D static grid
	std::vector<T> base;

public:

	//! it define that it is a vector
	typedef int yes_i_am_vector;

	//! iterator for the vector
	typedef vector_key_iterator iterator_key;
	//! Type of the value the vector is storing
	typedef T value_type;

	//! return the size of the vector
	inline size_t size() const
	{
		return base.size();
	}


	/*! \ brief Resize the vector to contain n elements
	 *
	 * \param slot number of elements
	 *
	 */
	inline void resize(size_t slot)
	{
		v_size = slot;

		base.resize(slot);
	}

	/*! \brief Remove all the element from the vector
	 *
	 */
	inline void clear()
	{
		base.clear();
	}

	/*! \brief It insert a new object on the vector, eventually it reallocate the grid
	 *
	 * \param v element to add
	 *
	 * \warning It is not thread safe should not be used in multi-thread environment
	 *          reallocation, work only on cpu
	 *
	 *vector_isel<T>::value
	 */
	inline void add(const T & v)
	{
		base.push_back(v);
	}

	/*! \brief Add an empty object (it call the default constructor () ) at the end of the vector
	 *
	 */

	inline void add()
	{
		base.resize(base.size() + 1);
	}

	/*! \brief Erase the elements from start to end
	 *
	 * \param start element
	 * \param end element
	 *
	 */
	void erase(typename std::vector<T>::iterator start, typename std::vector<T>::iterator end)
	{
		base.erase(start,end);
	}

	/*! \brief Remove one entry from the vector
	 *
	 * \param key element to remove
	 *
	 */
	void remove(size_t key)
	{
#ifdef DEBUG
		if (key >= base.size())
		{
			std::cerr << "Error vector: " << __FILE__ << ":" << __LINE__ << " overflow id: " << key << "\n";
		}
#endif
		base.erase(base.begin() + key);
	}

	/*! \brief Return an std compatible iterator to the first element
	 *
	 * \return an iterator to the first element
	 *
	 */
	inline auto begin() -> decltype(base.begin())
	{
		return base.begin();
	}

	/*! \brief Return an std compatible iterator to the last element
	 *
	 * \return an iterator to the last element
	 *
	 */
	inline auto end() -> decltype(base.begin())
	{
		return base.end();
	}

	/*! \brief Get the last element
	 *
	 * \return the last element as reference
	 *
	 */
	inline T & last()
	{
		return base[base.size()-1];
	}

	/*! \brief Duplicate the vector
	 *
	 * \return the duplicated vector
	 *
	 */
	std::vector<T> duplicate()
	{
		return base;
	}

	/*! \brief swap the memory between the two vector
	 *
	 * \param v vector to swap
	 *
	 */
	void swap(std::vector<T> && v)
	{
		base.swap(v);
	}

	/*! \brief It eliminate double entries
	 *
	 * \note The base object must have an operator== defined
	 *
	 */
	void unique()
	{
		auto it = std::unique(base.begin(),base.end());
		base.resize( std::distance(base.begin(),it) );
	}

	/*! \brief It sort the vector
	 *
	 * \note The base object must have an operator< defined
	 *
	 */
	void sort()
	{
		std::sort(base.begin(), base.end());
	}

	/*! \brief Get an element of the vector
	 *
	 * \tparam p must be 0
	 *
	 * \param id element to get
	 *
	 * \return the reference to the element
	 *
	 */
	template <unsigned int p>inline T& get(size_t id)
	{
#ifdef DEBUG
		if (p != 0)
		{std::cerr << "Error the property does not exist" << "\n";}

		if (id >= base.size())
		{
			std::cerr << "Error vector: " << __FILE__ << ":" << __LINE__ << " overflow id: " << id << "\n";
		}
#endif

		return base[id];
	}

	/*! \brief Get an element of the vector
	 *
	 * \tparam p must be 0
	 *
	 * \param id element to get
	 *
	 * \return the reference to the element
	 *
	 */
	template <unsigned int p>inline const T& get(size_t id) const
	{
#ifdef DEBUG
		if (p != 0)
		{std::cerr << "Error the property does not exist" << "\n";}

		if (id >= base.size())
		{
			std::cerr << "Error vector: " << __FILE__ << ":" << __LINE__ << " overflow id: " << id << "\n";
		}
#endif

		return base[id];
	}

	/*! \brief Get an element of the vector
	 *
	 * \param id element to get
	 *
	 * \return the element reference
	 *
	 */
	inline T & get(size_t id)
	{
#ifdef DEBUG
		if (id >= base.size())
		{
			std::cerr << "Error vector: " << __FILE__ << ":" << __LINE__ << " overflow id: " << id << "\n";
		}
#endif
		return base[id];
	}

	/*! \brief Get an element of the vector
	 *
	 * \param id element to get
	 *
	 * \return the element value
	 *
	 */
	inline const T & get(size_t id) const
	{
#ifdef DEBUG
		if (id >= base.size())
		{
			std::cerr << "Error vector: " << __FILE__ << ":" << __LINE__ << " overflow id: " << id << "\n";
		}
#endif
		return base[id];
	}

	/*! \brief it fill all the memory of fl patterns
	 *
	 * WARNING does not assign a value to each element but it fill the memory
	 * Useful to fast set the memory to zero
	 *
	 * \param fl byte to fill
	 *
	 */

	inline void fill(unsigned char fl)
	{
		memset(&base[0],fl,base.size() * sizeof(T));
	}

	/*! \brief reserve a memory space in advance to avoid reallocation
	 *
	 * \param ns number of element the memory has to store
	 *
	 */

	inline void reserve(size_t ns)
	{
		base.reserve(ns);
	}

	//! Constructor, vector of size 0
	vector() {}

	//! Constructor, vector of size sz
	vector(size_t sz):base(sz) {}

	/*! swap the content of the vector
	 *
	 * \param v vector to be swapped with
	 *
	 */
	void swap(openfpm::vector<T,HeapMemory,grow_policy_double,STD_VECTOR> & v)
	{
		base.swap(v.base);
	}

	/*! \brief Get iterator
	 *
	 * \return an iterator
	 *
	 */

	vector_key_iterator getIterator() const
	{
		return vector_key_iterator(base.size());
	}

	/*! \brief Calculate the memory size required to allocate n elements
	 *
	 * Calculate the total size required to store n-elements in a vector
	 *
	 * \param n number of elements
	 * \param e unused
	 *
	 * \return the size of the allocation number e
	 *
	 */
	inline static size_t calculateMem(size_t n, size_t e)
	{
		return n*sizeof(T);
	}

	/*! \brief How many allocation are required to create n-elements
	 *
	 * \param n number of elements
	 *
	 * \return the number of allocations
	 *
	 */
	inline static size_t calculateNMem(size_t n)
	{
		return 1;
	}

	/*! \brief Return the pointer to the chunk of memory
	 *
	 * \return the pointer to the chunk of memory
	 *
	 */
	void * getPointer()
	{
		return &base[0];
	}

	/*! \brief This class has pointer inside
	 *
	 * \return false
	 *
	 */
	static bool noPointers()
	{
		return false;
	}
};


#endif /* MAP_VECTOR_STD_HPP_ */
