/*
 * Copyright 2016, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * obj_cpp_make_persistent_array.cpp -- cpp make_persistent test for arrays
 */

#include "unittest.h"

#include <libpmemobj/persistent_ptr.hpp>
#include <libpmemobj/p.hpp>
#include <libpmemobj/pool.hpp>
#include <libpmemobj/make_persistent_array.hpp>

#define LAYOUT "cpp"

using namespace nvml::obj;

namespace
{

const int TEST_ARR_SIZE = 10;

class foo
{
      public:
	foo() : bar(1)
	{
		for (int i = 0; i < TEST_ARR_SIZE; ++i)
			this->arr[i] = 1;
	}

	/*
	 * Assert values of foo.
	 */
	void
	check_foo()
	{
		UT_ASSERTeq(1, this->bar);
		for (int i = 0; i < TEST_ARR_SIZE; ++i)
			UT_ASSERTeq(1, this->arr[i]);
	}

	~foo()
	{
		this->bar = 0;
		for (int i = 0; i < TEST_ARR_SIZE; ++i)
			this->arr[i] = 0;
	}

	p<int> bar;
	p<char> arr[TEST_ARR_SIZE];
};

struct root {
	persistent_ptr<foo[]> pfoo;
};

/*
 * test_make_one_d -- (internal) test make_persitent of a 1d array
 */
void
test_make_one_d(pool_base &pop)
{
	try {
		transaction::exec_tx(pop, [&] {
			auto pfoo = make_persistent<foo[]>(5);
			for (int i = 0; i < 5; ++i)
				pfoo[i].check_foo();

			delete_persistent<foo[]>(pfoo, 5);
			UT_ASSERT(pfoo == nullptr);

			auto pfoo2 = make_persistent<foo[]>(6);
			for (int i = 0; i < 6; ++i)
				pfoo2[i].check_foo();

			delete_persistent<foo[]>(pfoo2, 6);
			UT_ASSERT(pfoo2 == nullptr);

			auto pfooN = make_persistent<foo[5]>();
			for (int i = 0; i < 5; ++i)
				pfooN[i].check_foo();

			delete_persistent<foo[5]>(pfooN);
			UT_ASSERT(pfooN == nullptr);
		});
	} catch (...) {
		UT_ASSERT(0);
	}
}

/*
 * test_make_two_d -- (internal) test make_persitent of a 2d array
 */
void
test_make_two_d(pool_base &pop)
{
	try {
		transaction::exec_tx(pop, [&] {
			auto pfoo = make_persistent<foo[][2]>(5);
			for (int i = 0; i < 5; ++i)
				for (int j = 0; j < 2; j++)
					pfoo[i][j].check_foo();

			delete_persistent<foo[][2]>(pfoo, 5);
			UT_ASSERT(pfoo == nullptr);

			auto pfoo2 = make_persistent<foo[][3]>(6);
			for (int i = 0; i < 6; ++i)
				for (int j = 0; j < 3; j++)
					pfoo2[i][j].check_foo();

			delete_persistent<foo[][3]>(pfoo2, 6);
			UT_ASSERT(pfoo2 == nullptr);

			auto pfooN = make_persistent<foo[5][2]>();
			for (int i = 0; i < 5; ++i)
				for (int j = 0; j < 2; j++)
					pfooN[i][j].check_foo();

			delete_persistent<foo[5][2]>(pfooN);
			UT_ASSERT(pfooN == nullptr);
		});
	} catch (...) {
		UT_ASSERT(0);
	}
}

/*
 * test_abort_revert -- (internal) test destruction behavior and revert
 */
void
test_abort_revert(pool_base &pop)
{
	pool<struct root> &root_pop = dynamic_cast<pool<struct root> &>(pop);
	persistent_ptr<root> r = root_pop.get_root();

	try {
		transaction::exec_tx(pop, [&] {
			r->pfoo = make_persistent<foo[]>(5);
			for (int i = 0; i < 5; ++i)
				r->pfoo[i].check_foo();
		});
	} catch (...) {
		UT_ASSERT(0);
	}

	try {
		transaction::exec_tx(pop, [&] {
			UT_ASSERT(r->pfoo != nullptr);
			delete_persistent<foo[]>(r->pfoo, 5);
			UT_ASSERT(r->pfoo == nullptr);

			transaction::abort(EINVAL);
		});
	} catch (...) {
		/* eat the exception */
	}

	UT_ASSERT(r->pfoo != nullptr);
	for (int i = 0; i < 5; ++i)
		r->pfoo[i].check_foo();

	try {
		transaction::exec_tx(
			pop, [&] { delete_persistent<foo[]>(r->pfoo, 5); });
	} catch (...) {
		UT_ASSERT(0);
	}

	UT_ASSERT(r->pfoo == nullptr);
}
}

int
main(int argc, char *argv[])
{
	START(argc, argv, "obj_cpp_make_persistent_array");

	if (argc != 2)
		UT_FATAL("usage: %s file-name", argv[0]);

	const char *path = argv[1];

	pool<struct root> pop;

	try {
		pop = pool<struct root>::create(path, LAYOUT, PMEMOBJ_MIN_POOL,
						S_IWUSR | S_IRUSR);
	} catch (nvml::pool_error &pe) {
		UT_FATAL("!pool::create: %s %s", pe.what(), path);
	}

	test_make_one_d(pop);
	test_make_two_d(pop);
	test_abort_revert(pop);

	pop.close();

	DONE(NULL);
}
