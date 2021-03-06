/*
 * multi_array_ref_openfpm.hpp
 *
 *  Created on: Jun 29, 2018
 *      Author: i-bird
 */

#ifndef MULTI_ARRAY_REF_OPENFPM_HPP_
#define MULTI_ARRAY_REF_OPENFPM_HPP_

#include "util/cuda_util.hpp"
#include "boost/multi_array/collection_concept.hpp"
#include "boost/multi_array/concept_checks.hpp"
#include "boost/multi_array/algorithm.hpp"
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/multiplies.hpp>
#include <boost/concept_check.hpp>
#include "array_openfpm.hpp"
#include "types.hpp"
#include "multi_array_ref_base_openfpm.hpp"
#include "multi_array_ref_subarray_openfpm.hpp"
#include "storage_order.hpp"
#include "multi_array_iterator_openfpm.hpp"
#include "util/common.hpp"

namespace openfpm {



template <typename T, std::size_t NumDims, typename vector, typename TPtr>
class const_multi_array_ref_openfpm : public detail::multi_array::multi_array_impl_base_openfpm<T,NumDims,vector>
{
	typedef detail::multi_array::multi_array_impl_base_openfpm<T,NumDims,vector> super_type;

	typedef typename boost::mpl::accumulate<vector,
								   typename boost::mpl::int_<1>,
								   typename boost::mpl::multiplies<typename boost::mpl::_2,typename boost::mpl::_1>  >::type size_ct;

public:

/*    typedef typename super_type::value_type value_type;*/
    typedef typename super_type::const_reference const_reference;
    typedef typename super_type::const_iterator const_iterator;
/*    typedef typename super_type::const_reverse_iterator const_reverse_iterator;*/
    typedef T element;
    typedef size_t size_type;
/*    typedef typename super_type::difference_type difference_type;*/
    typedef typename super_type::index index;
/*    typedef typename super_type::extent_range extent_range;*/
    typedef general_storage_order<NumDims> storage_order_type;

    // template typedefs
/*    template <std::size_t NDims>
    struct const_array_view_openfpm
	{
    	typedef boost::detail::multi_array::const_multi_array_view_openfpm<T,NDims> type;
    };

    template <std::size_t NDims>
    struct array_view
	{
      typedef boost::detail::multi_array::multi_array_view<T,NDims> type;
    };

	#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
    	// make const_multi_array_ref a friend of itself
    	template <typename,std::size_t,typename>
    	friend class const_multi_array_ref;
  	#endif*/


    template <typename ExtentType>
    explicit const_multi_array_ref_openfpm(TPtr base, const ExtentType& extents, const general_storage_order<NumDims>& so)
    :base_(base),storage_(so)
    {
    	init_multi_array_ref(extents);
    }


    template <class InputIterator>
    void assign(InputIterator begin, InputIterator end)
    {
    	boost::function_requires<boost::InputIteratorConcept<InputIterator> >();

    	InputIterator in_iter = begin;
    	T* out_iter = base_;
    	std::size_t copy_count=0;
    	while (in_iter != end && copy_count < num_elements_)
    	{
    		*out_iter++ = *in_iter++;
    		copy_count++;
    	}
    }

    size_type num_dimensions() const { return NumDims; }

    size_type size() const { return extent_sz; }

    // given reshaping functionality, this is the max possible size.
    size_type max_size() const { return num_elements(); }

    bool empty() const { return size() == 0; }

    inline __device__ __host__ const index* strides() const {return stride_list_.data();}
    inline __device__ __host__ const element* origin() const { return base_; }
    inline __device__ __host__ const element* data() const { return base_; }

    size_type num_elements() const { return num_elements_; }

/*    template <typename IndexList>
    const element& operator()(IndexList indices) const {
      boost::function_requires<
        boost::CollectionConcept<IndexList> >();
      return super_type::access_element(boost::type<const element&>(),
                                        indices,origin(),
                                        shape(),strides(),index_bases());
    }*/

    // Only allow const element access
/*    __device__ __host__ const_reference operator[](index idx) const
    {
      return super_type::access(boost::type<const_reference>(),
                                idx,origin(),
                                shape(),strides(),index_bases());
    }

    // see generate_array_view in base.hpp
    template <int NDims>
    __device__ __host__ typename const_array_view_openfpm<NDims>::type
    operator[](const detail::multi_array::
               index_gen<NumDims,NDims>& indices)
      const {
      typedef typename const_array_view_openfpm<NDims>::type return_type;
      return
        super_type::generate_array_view(boost::type<return_type>(),
                                        indices,
                                        shape(),
                                        strides(),
                                        index_bases(),
                                        origin());
    }*/

    const_iterator begin() const
    {
      return const_iterator(0,origin(),size(),strides());
    }

    const_iterator end() const
    {
      return const_iterator(size(),origin(),size(),strides());
    }

/*    const_reverse_iterator rbegin() const {
      return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const {
      return const_reverse_iterator(begin());
    }


    template <typename OPtr>
    bool operator==(const
                    const_multi_array_ref_openfpm<T,NumDims,OPtr>& rhs)
      const {
      if(std::equal(extent_list_.begin(),
                    extent_list_.end(),
                    rhs.extent_list_.begin()))
        return std::equal(begin(),end(),rhs.begin());
      else return false;
    }

    template <typename OPtr>
    bool operator<(const const_multi_array_ref_openfpm<T,NumDims,OPtr>& rhs) const
    {
      return std::lexicographical_compare(begin(),end(),rhs.begin(),rhs.end());
    }

    template <typename OPtr>
    bool operator!=(const const_multi_array_ref_openfpm<T,NumDims,OPtr>& rhs) const
    {
      return !(*this == rhs);
    }

    template <typename OPtr>
    bool operator>(const const_multi_array_ref_openfpm<T,NumDims,OPtr>& rhs) const
    {
      return rhs < *this;
    }

    template <typename OPtr>
    bool operator<=(const const_multi_array_ref_openfpm<T,NumDims,OPtr>& rhs) const
    {
      return !(*this > rhs);
    }

    template <typename OPtr>
    bool operator>=(const const_multi_array_ref_openfpm<T,NumDims,OPtr>& rhs) const
    {
      return !(*this < rhs);
    }


  #ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
  protected:
  #else
  public:
  #endif

    typedef size_type size_list;*/
    typedef openfpm::array<index,NumDims> index_list;

    // This is used by multi_array, which is a subclass of this
    void set_base_ptr(TPtr new_base) { base_ = new_base; }


    // This constructor supports multi_array's default constructor
    // and constructors from multi_array_ref, subarray, and array_view
/*    explicit
    const_multi_array_ref_openfpm(TPtr base,
                          const storage_order_type& so,
                          const index * index_bases,
                          const size_type* extents) :
      base_(base), storage_(so), origin_offset_(0), directional_offset_(0)
   {
     // If index_bases or extents is null, then initialize the corresponding
     // private data to zeroed lists.
     if(index_bases) {
       boost::detail::multi_array::
         copy_n(index_bases,NumDims,index_base_list_.begin());
     } else {
       std::fill_n(index_base_list_.begin(),NumDims,0);
     }
     if(extents) {
       init_multi_array_ref(extents);
     } else {
       boost::array<index,NumDims> extent_list;
       extent_list.assign(0);
       init_multi_array_ref(extent_list.begin());
     }
   }*/


    TPtr base_;
    storage_order_type storage_;
    size_type extent_sz;
    size_type num_elements_;
    index_list stride_list_;

private:

    // const_multi_array_ref cannot be assigned to (no deep copies!)
    const_multi_array_ref_openfpm& operator=(const const_multi_array_ref_openfpm & other);

    void init_multi_array_ref(const index sz)
    {
    	// calculate the extents
    	extent_sz = sz;

    	this->compute_strides(stride_list_,extent_sz,storage_);

    	num_elements_ = sz * size_ct::value;
    }
};


template <typename T, int NumDims, typename vector>
class multi_array_ref_openfpm : public const_multi_array_ref_openfpm<T,NumDims,vector,T *>
{
	typedef const_multi_array_ref_openfpm<T,NumDims,vector,T *> super_type;
public:
/*  typedef typename super_type::value_type value_type;*/
  typedef typename super_type::reference reference;
  typedef typename super_type::iterator iterator;
/*  typedef typename super_type::reverse_iterator reverse_iterator;*/
  typedef typename super_type::const_reference const_reference;
  typedef typename super_type::const_iterator const_iterator;
/*  typedef typename super_type::const_reverse_iterator const_reverse_iterator;*/
  typedef typename super_type::element element;
  typedef typename super_type::size_type size_type;
//  typedef typename super_type::difference_type difference_type;
  typedef typename super_type::index index;
/*  typedef typename super_type::extent_range extent_range;*/

  typedef typename super_type::storage_order_type storage_order_type;
  typedef typename super_type::index_list index_list;

  //! indicate that this class is a multi dimensional array
  typedef int yes_is_multi_array;

/*  typedef typename super_type::size_list size_list;*/

/*  template <std::size_t NDims>
  struct const_array_view_openfpm {
    typedef boost::detail::multi_array::const_multi_array_view_openfpm<T,NDims> type;
  };*/

/*  template <std::size_t NDims>
  struct array_view_openfpm {
    typedef boost::detail::multi_array::multi_array_view_openfpm<T,NDims> type;
  };*/

  template <class ExtentType>
  explicit multi_array_ref_openfpm(T* base, const ExtentType r_sz, const general_storage_order<NumDims>& so)
  :super_type(base,r_sz,so)
  {
  }

  // Assignment from other ConstMultiArray types.
  template <typename ConstMultiArray>
  multi_array_ref_openfpm & operator=(const ConstMultiArray& other)
  {
    boost::function_requires<
      boost::multi_array_concepts::
      ConstMultiArrayConcept<ConstMultiArray,NumDims> >();

    // make sure the dimensions agree
    BOOST_ASSERT(other.num_dimensions() == this->num_dimensions());
    BOOST_ASSERT(std::equal(other.shape(),other.shape()+this->num_dimensions(),
                            this->shape()));
    // iterator-based copy
    std::copy(other.begin(),other.end(),this->begin());
    return *this;
  }

  multi_array_ref_openfpm & operator=(const multi_array_ref_openfpm & other)
  {
    if (&other != this)
    {
      // make sure the dimensions agree

      BOOST_ASSERT(other.num_dimensions() == this->num_dimensions());

      // iterator-based copy
      std::copy(other.begin(),other.end(),this->begin());
    }
    return *this;
  }

  multi_array_ref_openfpm & bind_ref(const multi_array_ref_openfpm & other)
  {
    if (&other != this) {

        this->base_ = other.base_;
        this->storage_ = other.storage_;
        this->extent_sz = other.extent_sz;
        this->stride_list_ = other.stride_list_;
        this->num_elements_ = other.num_elements_;
    }
    return *this;
  }

  /* \brief Set the internal pointer
   *
   * \param base internal pointer
   *
   */
  void set_pointer(void * base)
  {
	  this->base_ = static_cast<T *>(base);
  }

  /* \brief Get the internal pointer
   *
   * \return the internal pointer
   *
   */
  void * get_pointer()
  {
	  return this->base_;
  }

  /* \brief Get the internal pointer
   *
   * \return the internal pointer
   *
   */
  const void * get_pointer() const
  {
	  return this->base_;
  }

  multi_array_ref_openfpm & operator=(multi_array_ref_openfpm && other)
  {
	swap(other);

    return *this;
  }

  void swap(multi_array_ref_openfpm & other)
  {
	T* base_tmp = this->base_;
	this->base_ = other.base_;
	other.base_ = base_tmp;

    storage_order_type storage_tmp = this->storage_;
    this->storage_ = other.storage_;
    other.storage_ = storage_tmp;

    size_type extent_tmp = this->extent_sz;
    this->extent_sz = other.extent_sz;
    other.extent_sz = extent_tmp;

    index_list stride_list_tmp = this->stride_list_;
    this->stride_list_ = other.stride_list_;
    other.stride_list_ = stride_list_tmp;

    size_type num_elements_tmp = this->num_elements_;
    this->num_elements_ = other.num_elements_;
    other.num_elements_ = num_elements_tmp;
  }

  __device__ __host__ element* origin() { return super_type::base_; }

  __device__ __host__ const element* origin() const { return super_type::origin(); }

/*  element* data() { return super_type::base_; }

  template <class IndexList>
  element& operator()(const IndexList& indices) {
    boost::function_requires<
      CollectionConcept<IndexList> >();
    return super_type::access_element(boost::type<element&>(),
                                      indices,origin(),
                                      this->shape(),this->strides(),
                                      this->index_bases());
  }*/


  __device__ __host__ reference operator[](index idx)
  {
    return super_type::access(boost::type<reference>(),
                              idx,
                              this->strides(),
                              this->origin());
  }


  // See note attached to generate_array_view in base.hpp
/*  template <int NDims>
  typename array_view_openfpm<NDims>::type
  operator[](const detail::multi_array::
             index_gen<NumDims,NDims>& indices) {
    typedef typename array_view_openfpm<NDims>::type return_type;
    return
      super_type::generate_array_view(boost::type<return_type>(),
                                      indices,
                                      this->shape(),
                                      this->strides(),
                                      this->index_bases(),
                                      origin());
  }*/


  iterator begin()
  {return iterator(0,origin(),this->size(),this->strides());}

  iterator end()
  {return iterator(this->size(),origin(),this->size(),this->strides());}

  // rbegin() and rend() written naively to thwart MSVC ICE.
/*  reverse_iterator rbegin() {
    reverse_iterator ri(end());
    return ri;
  }

  reverse_iterator rend() {
    reverse_iterator ri(begin());
    return ri;
  }

  // Using declarations don't seem to work for g++
  // These are the proxies to work around this.

  const element* origin() const { return super_type::origin(); }
  const element* data() const { return super_type::data(); }

  template <class IndexList>
  const element& operator()(const IndexList& indices) const {
    boost::function_requires<
      CollectionConcept<IndexList> >();
    return super_type::operator()(indices);
  }*/

  __inline__ const_reference operator[](index idx) const
  {
	  return super_type::access(boost::type<const_reference>(),
                              idx,
                              this->strides(),
                              this->origin());
  }

  // See note attached to generate_array_view in base.hpp
/*  template <int NDims>
  typename const_array_view_openfpm<NDims>::type
  operator[](const detail::multi_array::
             index_gen<NumDims,NDims>& indices)
    const {
    return super_type::operator[](indices);
  }*/

  const_iterator begin() const
  {return super_type::begin();}

  const_iterator end() const
  {return super_type::end();}

/*  const_reverse_iterator rbegin() const {
    return super_type::rbegin();
  }

  const_reverse_iterator rend() const {
    return super_type::rend();
  }

protected:
  // This is only supplied to support multi_array's default constructor
  explicit multi_array_ref_openfpm(T* base,
                           const storage_order_type& so,
                           const index* index_bases,
                           const size_type* extents) :
    super_type(base,so,index_bases,extents) { }*/

};

template<typename T, typename Sfinae = void>
struct is_multi_array: std::false_type {};


/*! \brief has_noPointers check if a type has defined a
 * method called noPointers
 *
 * ### Example
 *
 * \snippet util_test.hpp Check no pointers
 *
 * return true if T::noPointers() is a valid expression (function pointers)
 * and produce a defined type
 *
 */
template<typename T>
struct is_multi_array<T, typename Void<typename T::yes_is_multi_array >::type> : std::true_type
{};

} // namespace openfpm


#endif /* MULTI_ARRAY_REF_OPENFPM_HPP_ */
