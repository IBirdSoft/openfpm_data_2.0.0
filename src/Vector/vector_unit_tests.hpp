#ifndef VECTOR_UNIT_TESTS_HPP
#define VECTOR_UNIT_TESTS_HPP

#include "map_vector.hpp"
#include "Point_test.hpp"
#include "memory/PreAllocHeapMemory.hpp"
#include "memory/ExtPreAlloc.hpp"
#include "memory/PtrMemory.hpp"
#include <cstring>
#include "Space/Shape/Point.hpp"
#include "util/object_util.hpp"
#include "vector_test_util.hpp"

BOOST_AUTO_TEST_SUITE( vector_test )

// Test vector iterator

BOOST_AUTO_TEST_CASE (vector_iterator_test)
{
	openfpm::vector<Point_test<float>> v_ofp_test = allocate_openfpm(FIRST_PUSH);

	size_t count = 0;

	auto it = v_ofp_test.getIterator();

	while (it.isNext())
	{
		count++;

		++it;
	}

	BOOST_REQUIRE_EQUAL(count,v_ofp_test.size());

	count = 0;
	auto it_f = v_ofp_test.getIteratorFrom( FIRST_PUSH / 2 );

	while (it_f.isNext())
	{
		count++;

		++it_f;
	}

	BOOST_REQUIRE_EQUAL(count, v_ofp_test.size() / 2 );
}

// Test the openfpm vector

BOOST_AUTO_TEST_CASE( vector_use)
{
	std::cout << "Vector unit test start" << "\n";

	std::vector<Point_orig<float>> v_stl_test = allocate_stl();
	openfpm::vector<Point_test<float>> v_ofp_test = allocate_openfpm(FIRST_PUSH);

	// try to duplicate the vector
	openfpm::vector<Point_test<float>> dv_ofp_test = v_ofp_test.duplicate();

	// Check if the STL and openfpm match

	for (size_t i = 0; i < FIRST_PUSH; i++)
	{
		BOOST_REQUIRE_EQUAL(v_stl_test[i].v[0],v_ofp_test.template get<P::v>(i)[0]);
		BOOST_REQUIRE_EQUAL(v_stl_test[i].v[1],v_ofp_test.template get<P::v>(i)[1]);
		BOOST_REQUIRE_EQUAL(v_stl_test[i].v[2],v_ofp_test.template get<P::v>(i)[2]);

		BOOST_REQUIRE_EQUAL(v_stl_test[i].t[0][0],v_ofp_test.template get<P::t>(i)[0][0]);
		BOOST_REQUIRE_EQUAL(v_stl_test[i].t[0][1],v_ofp_test.template get<P::t>(i)[0][1]);
		BOOST_REQUIRE_EQUAL(v_stl_test[i].t[0][2],v_ofp_test.template get<P::t>(i)[0][2]);
		BOOST_REQUIRE_EQUAL(v_stl_test[i].t[1][0],v_ofp_test.template get<P::t>(i)[1][0]);
		BOOST_REQUIRE_EQUAL(v_stl_test[i].t[1][1],v_ofp_test.template get<P::t>(i)[1][1]);
		BOOST_REQUIRE_EQUAL(v_stl_test[i].t[1][2],v_ofp_test.template get<P::t>(i)[1][2]);
		BOOST_REQUIRE_EQUAL(v_stl_test[i].t[2][0],v_ofp_test.template get<P::t>(i)[2][0]);
		BOOST_REQUIRE_EQUAL(v_stl_test[i].t[2][1],v_ofp_test.template get<P::t>(i)[2][1]);
		BOOST_REQUIRE_EQUAL(v_stl_test[i].t[2][2],v_ofp_test.template get<P::t>(i)[2][2]);
	}

	// Check if the duplicated vector match

	for (size_t i = 0 ; i < FIRST_PUSH ; i++)
	{
		BOOST_REQUIRE_EQUAL(dv_ofp_test.template get<P::v>(i)[0],v_ofp_test.template get<P::v>(i)[0]);
		BOOST_REQUIRE_EQUAL(dv_ofp_test.template get<P::v>(i)[1],v_ofp_test.template get<P::v>(i)[1]);
		BOOST_REQUIRE_EQUAL(dv_ofp_test.template get<P::v>(i)[2],v_ofp_test.template get<P::v>(i)[2]);

		BOOST_REQUIRE_EQUAL(dv_ofp_test.template get<P::t>(i)[0][0],v_ofp_test.template get<P::t>(i)[0][0]);
		BOOST_REQUIRE_EQUAL(dv_ofp_test.template get<P::t>(i)[0][1],v_ofp_test.template get<P::t>(i)[0][1]);
		BOOST_REQUIRE_EQUAL(dv_ofp_test.template get<P::t>(i)[0][2],v_ofp_test.template get<P::t>(i)[0][2]);
		BOOST_REQUIRE_EQUAL(dv_ofp_test.template get<P::t>(i)[1][0],v_ofp_test.template get<P::t>(i)[1][0]);
		BOOST_REQUIRE_EQUAL(dv_ofp_test.template get<P::t>(i)[1][1],v_ofp_test.template get<P::t>(i)[1][1]);
		BOOST_REQUIRE_EQUAL(dv_ofp_test.template get<P::t>(i)[1][2],v_ofp_test.template get<P::t>(i)[1][2]);
		BOOST_REQUIRE_EQUAL(dv_ofp_test.template get<P::t>(i)[2][0],v_ofp_test.template get<P::t>(i)[2][0]);
		BOOST_REQUIRE_EQUAL(dv_ofp_test.template get<P::t>(i)[2][1],v_ofp_test.template get<P::t>(i)[2][1]);
		BOOST_REQUIRE_EQUAL(dv_ofp_test.template get<P::t>(i)[2][2],v_ofp_test.template get<P::t>(i)[2][2]);
	}

	std::cout << "Vector unit test end" << "\n";
}

// Pre alloc test

struct pre_test
{
	//! position vector
	openfpm::vector<Point<2,float>,PreAllocHeapMemory<2>,openfpm::grow_policy_identity> pos;
	//! properties vector
	openfpm::vector<Point_test<float>,PreAllocHeapMemory<2>,openfpm::grow_policy_identity> prp;
};

BOOST_AUTO_TEST_CASE( vector_std_utility )
{
	//! [Create add and access stl]

	// Create a vector with 13 element
	openfpm::vector<size_t> pb(13);

	// add at the end some othe element
	pb.add(0);
	pb.add(1);
	pb.add(2);

	// access the vector
	for (size_t i = 0 ;  i < 16 ; i++)
	{
		pb.get(i) = i+1;
	}

	//! [Create add and access stl]

	pb.fill(0);

	// Check is zero
	for (size_t i = 0 ;  i < 16 ; i++)
	{
		BOOST_REQUIRE_EQUAL(pb.get(i),0);
	}

}

size_t alloc[] = {235,345,520};
size_t n_alloc = sizeof(alloc)/sizeof(size_t);

BOOST_AUTO_TEST_CASE ( vector_prealloc_ext )
{
	// Memory for the ghost sending buffer
	HeapMemory mem;

	// sequence of pre-allocation pattern
	std::vector<size_t> pap;

	size_t total = 0;

	// Calculate the total size required for the sending buffer
	for (size_t i = 0 ; i < n_alloc ; i++)
	{
		size_t alloc_ele = openfpm::vector<Point_test<float>>::calculateMem(alloc[i],0);
		pap.push_back(alloc_ele);
		total += alloc_ele;
	}

	// Create an object of preallocated memory
	ExtPreAlloc<HeapMemory> * prAlloc = new ExtPreAlloc<HeapMemory>(pap,mem);

	typedef openfpm::vector<Point_test<float>,ExtPreAlloc<HeapMemory>> send_vector;

	// create a vector of send_vector (ExtPreAlloc warrant that all the created vector are contiguous)
	openfpm::vector<send_vector> g_send;

	// resize
	g_send.resize(n_alloc);

	// Number of allocation
	for (size_t i = 0 ; i < n_alloc ; i++)
	{
		// set the preallocated memory to ensure contiguity
		g_send.get(i).setMemory(*prAlloc);

		// resize the sending vector (No allocation is produced)
		g_send.get(i).resize(alloc[i]);
	}

	// Fill the send buffer with one
	for (size_t i = 0 ; i < n_alloc ; i++)
	{
		auto it = g_send.get(i).getIterator();
		auto & v = g_send.get(i);

		while(it.isNext())
		{
			auto kk = it.get();

			v.template get<P::x>(kk) = 1.0f;
			v.template get<P::y>(kk) = 1.0f;
			v.template get<P::z>(kk) = 1.0f;
			v.template get<P::s>(kk) = 1.0f;

			v.template get<P::v>(kk)[0] = 1.0f;
			v.template get<P::v>(kk)[1] = 1.0f;
			v.template get<P::v>(kk)[2] = 1.0f;

			v.template get<P::t>(kk)[0][0] = 1.0f;
			v.template get<P::t>(kk)[0][1] = 1.0f;
			v.template get<P::t>(kk)[0][2] = 1.0f;
			v.template get<P::t>(kk)[1][0] = 1.0f;
			v.template get<P::t>(kk)[1][1] = 1.0f;
			v.template get<P::t>(kk)[1][2] = 1.0f;
			v.template get<P::t>(kk)[2][0] = 1.0f;
			v.template get<P::t>(kk)[2][1] = 1.0f;
			v.template get<P::t>(kk)[2][2] = 1.0f;

			++it;
		}
	}

	// check that HeapMemory contain ones in the right position
	float * ptr = (float *) mem.getPointer();
	size_t offset = 0;

	for (size_t i = 0 ; i < n_alloc ; i++)
	{
		for (size_t j = 0 ; j < alloc[i] ; j++)
			BOOST_REQUIRE_EQUAL(ptr[j + offset/sizeof(float)],1.0f);

		offset += pap[i];
	}
}

BOOST_AUTO_TEST_CASE( vector_prealloc )
{
	openfpm::vector<pre_test> pb(3);

	for (size_t i = 0 ;  i < 3 ; i++)
	{
		// Create the size required to store the particles position and properties to communicate
		size_t s1 = openfpm::vector<Point<2,float>>::calculateMem(1024,0);
		size_t s2 = openfpm::vector<Point_test<float>>::calculateMem(1024,0);

		// Preallocate the memory
		size_t sz[2] = {s1,s2};
		PreAllocHeapMemory<2> * mem = new PreAllocHeapMemory<2>(sz);

		// Set the memory allocator
		pb.get(i).pos.setMemory(*mem);
		pb.get(i).prp.setMemory(*mem);

		// set the size and allocate, using mem warrant that pos and prp is contiguous
		pb.get(i).pos.resize(1024);
		pb.get(i).prp.resize(1024);
	}
}


BOOST_AUTO_TEST_CASE( object_test_creator )
{
	bool tst = std::is_same< typename object_creator<Point_test<float>::type,0,1,5>::type, typename boost::fusion::vector3<float,float,float[3][3]> >::value;

	BOOST_REQUIRE_EQUAL(tst , true);
}

#define V_REM_PUSH 1024

BOOST_AUTO_TEST_CASE(vector_remove )
{
	typedef Point_test<float> p;

	//! [Create push and multiple remove]

	openfpm::vector<Point_test<float>> v1;

	for (size_t i = 0 ; i < V_REM_PUSH ; i++)
	{
		// Point
		Point_test<float> p;
		p.setx(i);

		v1.add(p);
	}

	{
	openfpm::vector<size_t> rem;
	rem.add(0);
	rem.add(1);
	rem.add(2);
	rem.add(3);

	v1.remove(rem);
	}

	//! [Create push and multiple remove]

	BOOST_REQUIRE_EQUAL(v1.size(),1020);
	BOOST_REQUIRE_EQUAL(v1.template get<p::x>(0),4);

	{
	openfpm::vector<size_t> rem;
	rem.add(v1.size()-3);
	rem.add(v1.size()-2);
	rem.add(v1.size()-1);
	rem.add(v1.size());

	v1.remove(rem);
	}

	BOOST_REQUIRE_EQUAL(v1.size(),1016);
	BOOST_REQUIRE_EQUAL(v1.template get<p::x>(v1.size()-1),1019);

	{
	openfpm::vector<size_t> rem;
	for (size_t i = 0 ; i < (V_REM_PUSH - 8) / 2 ; i++)
		rem.add(i * 2);

	// remove all the even number
	v1.remove(rem);
	}

	BOOST_REQUIRE_EQUAL(v1.size(),508);

	// Check only odd
	for (size_t i = 0 ; i < v1.size() ; i++)
	{
		BOOST_REQUIRE_EQUAL((size_t)v1.template get<p::x>(v1.size()-1) % 2, 1);
	}
}

BOOST_AUTO_TEST_CASE(vector_clear )
{
	typedef Point_test<float> p;

	openfpm::vector<Point_test<float>> v1;

	for (size_t i = 0 ; i < V_REM_PUSH ; i++)
	{
		// Point
		Point_test<float> p;
		p.setx(i);

		v1.add(p);
	}

	v1.clear();

	BOOST_REQUIRE_EQUAL(v1.size(),0);

	for (size_t i = 0 ; i < V_REM_PUSH ; i++)
	{
		// Point
		Point_test<float> p;
		p.setx(i);

		v1.add(p);
	}

	BOOST_REQUIRE_EQUAL(v1.size(),V_REM_PUSH);
}

BOOST_AUTO_TEST_CASE( vector_memory_repr )
{
	// create a vector
	openfpm::vector<Point_test<float>> v1;

	// Point
	Point_test<float> p;
	p.setx(1.0);
	p.sety(2.0);
	p.setz(3.0);
	p.sets(4.0);

	// push objects

	for (size_t i = 0 ; i < FIRST_PUSH ; i++)
	{
		// Modify p

		p.get<P::v>()[0] = 1.0 + i;
		p.get<P::v>()[1] = 2.0 + i;
		p.get<P::v>()[2] = 7.0 + i;

		p.get<P::t>()[0][0] = 10.0 + i;
		p.get<P::t>()[0][1] = 13.0 + i;
		p.get<P::t>()[0][2] = 8.0 + i;
		p.get<P::t>()[1][0] = 19.0 + i;
		p.get<P::t>()[1][1] = 23.0 + i;
		p.get<P::t>()[1][2] = 5.0 + i;
		p.get<P::t>()[2][0] = 4.0 + i;
		p.get<P::t>()[2][1] = 3.0 + i;
		p.get<P::t>()[2][2] = 11.0 + i;

		// add p

		v1.add(p);
	}

	PtrMemory * ptr1 = new PtrMemory(v1.getPointer(),sizeof(Point_test<float>)*FIRST_PUSH);

	// create vector representation to a piece of memory already allocated

	openfpm::vector<Point_test<float>,PtrMemory,openfpm::grow_policy_identity> v2;

	v2.setMemory(*ptr1);

	v2.resize(FIRST_PUSH);

	// check

	// Check if the duplicated vector match

	for (size_t i = 0 ; i < FIRST_PUSH ; i++)
	{
		BOOST_REQUIRE_EQUAL(v1.template get<P::v>(i)[0],v2.template get<P::v>(i)[0]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::v>(i)[1],v2.template get<P::v>(i)[1]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::v>(i)[2],v2.template get<P::v>(i)[2]);

		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[0][0],v2.template get<P::t>(i)[0][0]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[0][1],v2.template get<P::t>(i)[0][1]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[0][2],v2.template get<P::t>(i)[0][2]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[1][0],v2.template get<P::t>(i)[1][0]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[1][1],v2.template get<P::t>(i)[1][1]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[1][2],v2.template get<P::t>(i)[1][2]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[2][0],v2.template get<P::t>(i)[2][0]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[2][1],v2.template get<P::t>(i)[2][1]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[2][2],v2.template get<P::t>(i)[2][2]);
	}
}

BOOST_AUTO_TEST_CASE( vector_add_test_case )
{
	// create two vector
	openfpm::vector<Point_test<float>> v1;

	// Point
	Point_test<float> p;
	p.setx(1.0);
	p.sety(2.0);
	p.setz(3.0);
	p.sets(4.0);

	// push objects

	for (size_t i = 0 ; i < FIRST_PUSH ; i++)
	{
		// Modify p

		p.get<P::v>()[0] = 1.0 + i;
		p.get<P::v>()[1] = 2.0 + i;
		p.get<P::v>()[2] = 7.0 + i;

		p.get<P::t>()[0][0] = 10.0 + i;
		p.get<P::t>()[0][1] = 13.0 + i;
		p.get<P::t>()[0][2] = 8.0 + i;
		p.get<P::t>()[1][0] = 19.0 + i;
		p.get<P::t>()[1][1] = 23.0 + i;
		p.get<P::t>()[1][2] = 5.0 + i;
		p.get<P::t>()[2][0] = 4.0 + i;
		p.get<P::t>()[2][1] = 3.0 + i;
		p.get<P::t>()[2][2] = 11.0 + i;

		// add p

		v1.add(p);
	}

	// Duplicate the vector
	openfpm::vector<Point_test<float>> v2 = v1.duplicate();

	v1.template add_prp<Point_test<float>,HeapMemory,typename openfpm::grow_policy_double,P::x,P::y,P::z,P::s,P::v,P::t>(v2);

	for (size_t i = 0 ; i < FIRST_PUSH ; i++)
	{
		BOOST_REQUIRE_EQUAL(v1.template get<P::v>(i)[0], v1.template get<P::v>(i+v2.size())[0]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::v>(i)[1], v1.template get<P::v>(i+v2.size())[1]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::v>(i)[2], v1.template get<P::v>(i+v2.size())[2]);

		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[0][0], v1.template get<P::t>(i+v2.size())[0][0]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[0][1], v1.template get<P::t>(i+v2.size())[0][1]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[0][2], v1.template get<P::t>(i+v2.size())[0][2]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[1][0], v1.template get<P::t>(i+v2.size())[1][0]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[1][1], v1.template get<P::t>(i+v2.size())[1][1]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[1][2], v1.template get<P::t>(i+v2.size())[1][2]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[2][0], v1.template get<P::t>(i+v2.size())[2][0]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[2][1], v1.template get<P::t>(i+v2.size())[2][1]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[2][2], v1.template get<P::t>(i+v2.size())[2][2]);
	}

	// add homogeneous

	v1.template add(v2);

	for (size_t i = 0 ; i < FIRST_PUSH ; i++)
	{
		BOOST_REQUIRE_EQUAL(v1.template get<P::v>(i)[0], v1.template get<P::v>(i+2*v2.size())[0]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::v>(i)[1], v1.template get<P::v>(i+2*v2.size())[1]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::v>(i)[2], v1.template get<P::v>(i+2*v2.size())[2]);

		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[0][0], v1.template get<P::t>(i+2*v2.size())[0][0]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[0][1], v1.template get<P::t>(i+2*v2.size())[0][1]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[0][2], v1.template get<P::t>(i+2*v2.size())[0][2]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[1][0], v1.template get<P::t>(i+2*v2.size())[1][0]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[1][1], v1.template get<P::t>(i+2*v2.size())[1][1]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[1][2], v1.template get<P::t>(i+2*v2.size())[1][2]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[2][0], v1.template get<P::t>(i+2*v2.size())[2][0]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[2][1], v1.template get<P::t>(i+2*v2.size())[2][1]);
		BOOST_REQUIRE_EQUAL(v1.template get<P::t>(i)[2][2], v1.template get<P::t>(i+2*v2.size())[2][2]);
	}

}

////////// Test function ///////////

openfpm::vector<scalar<float>> & test_error_v()
{
	openfpm::vector<scalar<float>> v(16);

	return v;
}

/////////////////////////////////////

BOOST_AUTO_TEST_CASE( vector_safety_check )
{
#if defined(SE_CLASS1) && defined (THROW_ON_ERROR)

	bool error = false;

	typedef Point_test<float> p;

	// Create a vector

	openfpm::vector<Point_test<float>> v(16);
	openfpm::vector<Point_test<float>> v2(16);

	error = false;
	try
	{v.template get<p::x>(23);}
	catch (size_t e)
	{
		error = true;
		BOOST_REQUIRE_EQUAL(e,VECTOR_ERROR);
		BOOST_REQUIRE_EQUAL(v.getLastError(),2001);
	}
	BOOST_REQUIRE_EQUAL(error,true);

	error = false;
	Point_test<float> t;
	try
	{v.set(23,t);}
	catch (size_t e)
	{
		error = true;
		BOOST_REQUIRE_EQUAL(e,VECTOR_ERROR);
		BOOST_REQUIRE_EQUAL(v.getLastError(),2001);
	}
	BOOST_REQUIRE_EQUAL(error,true);

	error = false;
	try
	{v.set(6,v2,23);}
	catch (size_t e)
	{
		error = true;
		BOOST_REQUIRE_EQUAL(e,GRID_ERROR);
	}
	BOOST_REQUIRE_EQUAL(error,true);

	//// Negative key

	error = false;
	try
	{v.template get<p::x>(-1);}
	catch (size_t e)
	{
		error = true;
		BOOST_REQUIRE_EQUAL(e,VECTOR_ERROR);
		BOOST_REQUIRE_EQUAL(v.getLastError(),2001);
	}
	BOOST_REQUIRE_EQUAL(error,true);

	error = false;
	Point_test<float> t2;
	try
	{v.set(-1,t2);}
	catch (size_t e)
	{
		error = true;
		BOOST_REQUIRE_EQUAL(e,VECTOR_ERROR);
		BOOST_REQUIRE_EQUAL(v.getLastError(),2001);
	}
	BOOST_REQUIRE_EQUAL(error,true);

	error = false;
	try
	{v.set(12,v2,-1);}
	catch (size_t e)
	{
		error = true;
		BOOST_REQUIRE_EQUAL(e,GRID_ERROR);
	}
	BOOST_REQUIRE_EQUAL(error,true);

	#if defined(SE_CLASS2) && defined (THROW_ON_ERROR)

	error = false;

	// Create a grid

	openfpm::vector<scalar<float>> * v3 = new openfpm::vector<scalar<float>>(16);
	delete v3;

	// Try to access the class

	try
	{v3->size();}
	catch (size_t e)
	{
		error = true;
		BOOST_REQUIRE_EQUAL(e,MEM_ERROR);
	}
	BOOST_REQUIRE_EQUAL(error,true);

	try
	{
		openfpm::vector<scalar<float>> vr = test_error_v();
	}
	catch (size_t e)
	{
		error = true;
		BOOST_REQUIRE_EQUAL(e,MEM_ERROR);
	}
	BOOST_REQUIRE_EQUAL(error,true);

	#endif

#endif
}

BOOST_AUTO_TEST_SUITE_END()

#endif
