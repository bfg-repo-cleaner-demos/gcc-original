// { dg-options "-std=gnu++0x" }
// { dg-do compile }

// Copyright (C) 2010-2012 Free Software Foundation
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

// 20.9.11.2 Template class shared_ptr [util.smartptr.shared]

#include <memory>

// incomplete type
struct X;

// get an auto_ptr rvalue
std::auto_ptr<X>&& ap();

void test01()
{
  X* px = 0;
  std::shared_ptr<X> p1(px);   // { dg-error "here" }
  // { dg-error "incomplete" "" { target *-*-* } 771 }

  std::shared_ptr<X> p9(ap());  // { dg-error "here" }
  // { dg-error "incomplete" "" { target *-*-* } 307 }
}
