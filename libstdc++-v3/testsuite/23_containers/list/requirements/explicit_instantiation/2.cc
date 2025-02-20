// Copyright (C) 2004, 2009, 2012 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.


// This file tests explicit instantiation of library containers

#include <list>
#include <testsuite_api.h>

// { dg-do compile }

// N.B. In C++0x mode we cannot instantiate with T == NonDefaultConstructible
// because of 23.3.4.1.4
#if __cplusplus < 201103L
template class std::list<__gnu_test::NonDefaultConstructible>;
#endif
