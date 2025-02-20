// std::numpunct implementation details, GNU version -*- C++ -*-

// Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2011
// Free Software Foundation, Inc.
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

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

//
// ISO C++ 14882: 22.2.3.1.2  numpunct virtual functions
//

// Written by Benjamin Kosnik <bkoz@redhat.com>

#include <locale>
#include <bits/c++locale_internal.h>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  template<> 
    void
    numpunct<char>::_M_initialize_numpunct(__c_locale __cloc)
    {
      if (!_M_data)
	_M_data = new __numpunct_cache<char>;

      if (!__cloc)
	{
	  // "C" locale
	  _M_data->_M_grouping = "";
	  _M_data->_M_grouping_size = 0;
	  _M_data->_M_use_grouping = false;

	  _M_data->_M_decimal_point = '.';
	  _M_data->_M_thousands_sep = ',';

	  for (size_t __i = 0; __i < __num_base::_S_oend; ++__i)
	    _M_data->_M_atoms_out[__i] = __num_base::_S_atoms_out[__i];

	  for (size_t __j = 0; __j < __num_base::_S_iend; ++__j)
	    _M_data->_M_atoms_in[__j] = __num_base::_S_atoms_in[__j];
	}
      else
	{
	  // Named locale.
	  _M_data->_M_decimal_point = *(__nl_langinfo_l(DECIMAL_POINT, 
							__cloc));
	  _M_data->_M_thousands_sep = *(__nl_langinfo_l(THOUSANDS_SEP, 
							__cloc));

	  // Check for NULL, which implies no grouping.
	  if (_M_data->_M_thousands_sep == '\0')
	    {
	      // Like in "C" locale.
	      _M_data->_M_grouping = "";
	      _M_data->_M_grouping_size = 0;
	      _M_data->_M_use_grouping = false;
	      _M_data->_M_thousands_sep = ',';
	    }
	  else
	    {
	      const char* __src = __nl_langinfo_l(GROUPING, __cloc);
	      const size_t __len = strlen(__src);
	      if (__len)
		{
		  __try
		    {
		      char* __dst = new char[__len + 1];
		      memcpy(__dst, __src, __len + 1);
		      _M_data->_M_grouping = __dst;
		    }
		  __catch(...)
		    {
		      delete _M_data;
		      _M_data = 0;
		      __throw_exception_again;
		    }
		}
	      else
		{
		  _M_data->_M_grouping = "";
		  _M_data->_M_use_grouping = false;
		}
	      _M_data->_M_grouping_size = __len;
	    }
	}

      // NB: There is no way to extact this info from posix locales.
      // _M_truename = __nl_langinfo_l(YESSTR, __cloc);
      _M_data->_M_truename = "true";
      _M_data->_M_truename_size = 4;
      // _M_falsename = __nl_langinfo_l(NOSTR, __cloc);
      _M_data->_M_falsename = "false";
      _M_data->_M_falsename_size = 5;
    }
 
  template<> 
    numpunct<char>::~numpunct()
    {
      if (_M_data->_M_grouping_size)
	delete [] _M_data->_M_grouping;
      delete _M_data;
    }
   
#ifdef _GLIBCXX_USE_WCHAR_T
  template<> 
    void
    numpunct<wchar_t>::_M_initialize_numpunct(__c_locale __cloc)
    {
      if (!_M_data)
	_M_data = new __numpunct_cache<wchar_t>;

      if (!__cloc)
	{
	  // "C" locale
	  _M_data->_M_grouping = "";
	  _M_data->_M_grouping_size = 0;
	  _M_data->_M_use_grouping = false;

	  _M_data->_M_decimal_point = L'.';
	  _M_data->_M_thousands_sep = L',';

	  // Use ctype::widen code without the facet...
	  for (size_t __i = 0; __i < __num_base::_S_oend; ++__i)
	    _M_data->_M_atoms_out[__i] =
	      static_cast<wchar_t>(__num_base::_S_atoms_out[__i]);

	  for (size_t __j = 0; __j < __num_base::_S_iend; ++__j)
	    _M_data->_M_atoms_in[__j] =
	      static_cast<wchar_t>(__num_base::_S_atoms_in[__j]);
	}
      else
	{
	  // Named locale.
	  // NB: In the GNU model wchar_t is always 32 bit wide.
	  union { char *__s; wchar_t __w; } __u;
	  __u.__s = __nl_langinfo_l(_NL_NUMERIC_DECIMAL_POINT_WC, __cloc);
	  _M_data->_M_decimal_point = __u.__w;

	  __u.__s = __nl_langinfo_l(_NL_NUMERIC_THOUSANDS_SEP_WC, __cloc);
	  _M_data->_M_thousands_sep = __u.__w;

	  // Check for NULL, which implies no grouping.
	  if (_M_data->_M_thousands_sep == L'\0')
	    {
	      // Like in "C" locale.
	      _M_data->_M_grouping = "";
	      _M_data->_M_grouping_size = 0;
	      _M_data->_M_use_grouping = false;
	      _M_data->_M_thousands_sep = L',';
	    }
	  else
	    {
	      const char* __src = __nl_langinfo_l(GROUPING, __cloc);
	      const size_t __len = strlen(__src);
	      if (__len)
		{
		  __try
		    {
		      char* __dst = new char[__len + 1];
		      memcpy(__dst, __src, __len + 1);
		      _M_data->_M_grouping = __dst;
		    }
		  __catch(...)
		    {
		      delete _M_data;
		      _M_data = 0;
		      __throw_exception_again;
		    }
		}
	      else
		{
		  _M_data->_M_grouping = "";
		  _M_data->_M_use_grouping = false;
		}
	      _M_data->_M_grouping_size = __len;
	    }
	}

      // NB: There is no way to extact this info from posix locales.
      // _M_truename = __nl_langinfo_l(YESSTR, __cloc);
      _M_data->_M_truename = L"true";
      _M_data->_M_truename_size = 4;
      // _M_falsename = __nl_langinfo_l(NOSTR, __cloc);
      _M_data->_M_falsename = L"false";
      _M_data->_M_falsename_size = 5;
    }

  template<> 
    numpunct<wchar_t>::~numpunct()
    {
      if (_M_data->_M_grouping_size)
	delete [] _M_data->_M_grouping;
      delete _M_data;
    }
 #endif

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace
