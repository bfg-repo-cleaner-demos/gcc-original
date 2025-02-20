/* Basic data types for Objective C.
   Copyright (C) 1993, 1995, 1996, 2004, 2009, 
   2010, 2011 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

#ifndef __objc_INCLUDE_GNU
#define __objc_INCLUDE_GNU

/* This file contains the definition of the basic types used by the
   Objective-C language.  It needs to be included to do almost
   anything with Objective-C.  */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* The current version of the GNU Objective-C Runtime library in
   compressed ISO date format.  This should be updated any time a new
   version is released with changes to the public API (there is no
   need to update it if there were no API changes since the previous
   release).  This macro is only defined starting with the GNU
   Objective-C Runtime shipped with GCC 4.6.0.  If it is not defined,
   it is either an older version of the runtime, or another runtime.  */
#define __GNU_LIBOBJC__ 20110608

/* Definition of the boolean type.

   Compatibility note: the Apple/NeXT runtime defines a BOOL as a
   'signed char'.  The GNU runtime uses an 'unsigned char'.

   Important: this could change and we could switch to 'typedef bool
   BOOL' in the future.  Do not depend on the type of BOOL.  */
#undef BOOL
typedef unsigned char  BOOL;

#define YES   (BOOL)1
#define NO    (BOOL)0

/* The basic Objective-C types (SEL, Class, id) are defined as pointer
   to opaque structures.  The details of the structures are private to
   the runtime and may potentially change from one version to the
   other.  */

/* A SEL (selector) represents an abstract method (in the
   object-oriented sense) and includes all the details of how to
   invoke the method (which means its name, arguments and return
   types) but provides no implementation of its own.  You can check
   whether a class implements a selector or not, and if you have a
   selector and know that the class implements it, you can use it to
   call the method for an object in the class.  */
typedef const struct objc_selector *SEL;

/* A Class is a class (in the object-oriented sense).  In Objective-C
   there is the complication that each Class is an object itself, and
   so belongs to a class too.  This class that a class belongs to is
   called its 'meta class'.  */
typedef struct objc_class *Class;

/* An 'id' is an object of an unknown class.  The way the object data
   is stored inside the object is private and what you see here is
   only the beginning of the actual struct.  The first field is always
   a pointer to the Class that the object belongs to.  */
typedef struct objc_object
{
  /* 'class_pointer' is the Class that the object belongs to.  In case
     of a Class object, this pointer points to the meta class.

     Compatibility Note: The Apple/NeXT runtime calls this field
     'isa'.  To access this field, use object_getClass() from
     runtime.h, which is an inline function so does not add any
     overhead and is also portable to other runtimes.  */
  Class class_pointer;
} *id;

/* 'IMP' is a C function that implements a method.  When retrieving
   the implementation of a method from the runtime, this is the type
   of the pointer returned.  The idea of the definition of IMP is to
   represent a 'pointer to a general function taking an id, a SEL,
   followed by other unspecified arguments'.  You must always cast an
   IMP to a pointer to a function taking the appropriate, specific
   types for that function, before calling it - to make sure the
   appropriate arguments are passed to it.  The code generated by the
   compiler to perform method calls automatically does this cast
   inside method calls.  */
typedef id (*IMP)(id, SEL, ...); 

/* 'nil' is the null object.  Messages to nil do nothing and always
   return 0.  */
#define nil (id)0

/* 'Nil' is the null class.  Since classes are objects too, this is
   actually the same object as 'nil' (and behaves in the same way),
   but it has a type of Class, so it is good to use it instead of
   'nil' if you are comparing a Class object to nil as it enables the
   compiler to do some type-checking.  */
#define Nil (Class)0

/* TODO: Move the 'Protocol' declaration into objc/runtime.h.  A
   Protocol is simply an object, not a basic Objective-C type.  The
   Apple runtime defines Protocol in objc/runtime.h too, so it's good
   to move it there for API compatibility.  */

/* A 'Protocol' is a formally defined list of selectors (normally
   created using the @protocol Objective-C syntax).  It is mostly used
   at compile-time to check that classes implement all the methods
   that they are supposed to.  Protocols are also available in the
   runtime system as Protocol objects.  */
#ifndef __OBJC__
  /* Once we stop including the deprecated struct_objc_protocol.h
     there is no reason to even define a 'struct objc_protocol'.  As
     all the structure details will be hidden, a Protocol basically is
     simply an object (as it should be).  */
  typedef struct objc_object Protocol;
#else /* __OBJC__ */
  @class Protocol;
#endif 

/* Compatibility note: the Apple/NeXT runtime defines sel_getName(),
   sel_registerName(), object_getClassName(), object_getIndexedIvars()
   in this file while the GNU runtime defines them in runtime.h.

   The reason the GNU runtime does not define them here is that they
   are not basic Objective-C types (defined in this file), but are
   part of the runtime API (defined in runtime.h).  */

#ifdef __cplusplus
}
#endif

#endif /* not __objc_INCLUDE_GNU */
