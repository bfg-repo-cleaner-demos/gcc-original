// { dg-do compile }
// { dg-options "-std=c++0x" }

// 2009-06-05  Stephen M. Webb  <stephen.webb@bregmasoft.ca>
//
// Copyright (C) 2009, 2011 Free Software Foundation, Inc.
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

// C++0X [28.8.3] class template basic_regex assign()

#include <regex>
#include <testsuite_hooks.h>

// Tests assign operation from a C-style null-terminated-string.  
void test01()
{
  bool test __attribute__((unused)) = true;

  std::basic_regex<char> re;

  const char* cs = "aab";
  re.assign(cs);
}

int
main()
{ 
  test01();
  return 0;
};
