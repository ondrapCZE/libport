/*
This software was developed as part of a project at MIT.

Copyright (c) 2005-2006 Russ Cox,
Massachusetts Institute of Technology

Permission is hereby granted, io_free of charge, to any person obtaining
a copy of this software and associated documentation files (the
												"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <sched/libcoroutine/PortableUContext.h>

#ifdef NEEDPOWERMAKECONTEXT
void makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...)
{
	unsigned long *sp, *tos;
	va_list arg;

	tos = (unsigned long*)ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size / sizeof(unsigned long);
	sp = tos - 16;
	ucp->mc.pc = (long)func;
	ucp->mc.sp = (long)sp;
	va_start(arg, argc);
	ucp->mc.r3 = va_arg(arg, long);
	va_end(arg);
}
#endif

#ifdef NEEDARMMAKECONTEXT
#include <stdarg.h>
void makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...)
{
  ucp->uc_mcontext.arm_ip = (unsigned long)func;
  ucp->uc_mcontext.arm_sp = (unsigned long)((unsigned long*)ucp->uc_stack.ss_sp
    + ucp->uc_stack.ss_size /   sizeof(unsigned long));
  va_list args;
  va_start(args, argc);
  #define arg(i) if (argc > i) ucp->uc_mcontext.arm_r##i = va_arg(args,long)
  arg(0);arg(1);arg(2);arg(3);arg(4);arg(5);arg(6);arg(7);arg(8);arg(9);
  #undef arg
  // Return address: Jumped to by the setmcontext code.
  ucp->uc_mcontext.arm_lr = (unsigned long)func;
  va_end(args);
}
#endif

#ifdef NEEDX86MAKECONTEXT
void makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...)
{
	uintptr_t *sp;

	sp = (uintptr_t *)ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size / sizeof(void *);
	sp -= argc;
	sp = (void*)((uintptr_t)sp - (uintptr_t)sp % 16);	/* 16-align for OS X */
	memmove(sp, &argc + 1, argc * sizeof(uintptr_t));

	*--sp = 0;		/* return address */
	ucp->uc_mcontext.mc_eip = (long)func;
	ucp->uc_mcontext.mc_esp = (long)sp;
}
#endif

#ifdef NEEDSWAPCONTEXT
int swapcontext(ucontext_t *oucp,
#ifdef __arm__
const
#endif
ucontext_t *ucp)
{
	if(getcontext(oucp) == 0)
		setcontext(ucp);
	return 0;
}
#endif
