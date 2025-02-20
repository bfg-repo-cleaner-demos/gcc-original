! { dg-do compile }
! Test fix for PR54286.
!
! Contributed by Janus Weil  <janus@gcc.gnu.org>
!
implicit integer (a)
type :: t
  procedure(a), pointer, nopass :: p
end type
type(t) :: x

procedure(iabs), pointer :: pp
procedure(foo), pointer :: pp1

x%p => a     ! ok
if (x%p(0) .ne. loc(foo)) call abort
if (x%p(1) .ne. loc(iabs)) call abort

x%p => a(1)  ! { dg-error "PROCEDURE POINTER mismatch in function result" }

pp => a(1)   ! ok
if (pp(-99) .ne. iabs(-99)) call abort

pp1 => a(2)   ! ok
if (pp1(-99) .ne. -iabs(-99)) call abort

pp => a  ! { dg-error "PROCEDURE POINTER mismatch in function result" }

contains

  function a (c) result (b)
    integer, intent(in) :: c
    procedure(iabs), pointer :: b
    if (c .eq. 1) then
      b => iabs
    else
      b => foo
    end if
  end function

  integer function foo (arg)
    integer, intent (in) :: arg
    foo = -iabs(arg)
  end function
end
