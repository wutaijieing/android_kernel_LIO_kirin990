/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may
 * *    be used to endorse or promote products derived from this software
 * *    without specific prior written permission.
 *
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __OSL_SPINLOCK_H
#define __OSL_SPINLOCK_H

#ifdef __KERNEL__
#include <linux/spinlock.h>

#elif defined(__OS_RTOSCK__)
#include "osl_irq.h"
#include "osl_thread.h"
#include "spinLockAmp.h"

/*lint -save -e830*/
#define I_BIT   (1<<7)
#define F_BIT   (1<<6)
/*lint -restore +e830*/

typedef struct spinlock {
		volatile unsigned int rlock;
} spinlock_t;

#define DEFINE_SPINLOCK(_lock)	spinlock_t _lock = (spinlock_t ) { .rlock = 0x0 }

#define spin_lock_init(__specific_lock)				\
do {							\
	((spinlock_t*)__specific_lock)->rlock = 0x0;		\
} while (0)

 static inline void spin_lock(spinlock_t *lock)
{
	raw_spin_lock(&lock->rlock);
}

 static inline int spin_trylock(spinlock_t *lock)
{
	return raw_spin_trylock(&lock->rlock);
}

 static inline void spin_unlock(spinlock_t *lock)
{
	raw_spin_unlock(&lock->rlock);
}
/*lint -e683*/
/*
*参数类型
*spinlock_t *  __specific_lock,
*unsigned long __specific_flags
*/
#define spin_lock_irqsave(__specific_lock, __specific_flags)				\
do { \
		local_irq_save(__specific_flags); \
		if(!osl_int_context_disirq()&&!(__specific_flags&I_BIT)) \
			osl_task_lock_disirq(); \
		spin_lock(__specific_lock); \
	} while (0)
/*
*参数类型
*spinlock_t *  __specific_lock,
*unsigned long __specific_flags
*/
#define spin_unlock_irqrestore(__specific_lock, __specific_flags)				\
do { \
    spin_unlock(__specific_lock); \
    local_irq_restore(__specific_flags);   \
    if(!osl_int_context()&&!(__specific_flags&I_BIT)) \
		osl_task_unlock(); \
    } while (0)
/*lint +e683*/

/* new osl_spin interface for both arm and txp */
typedef spinlock_t osl_spinlock_t;
/* 要判断返回值 */
static inline unsigned int osl_spin_lock_init(osl_spinlock_t* _lock)
{
    spin_lock_init(_lock);
    return OK;
}
#define osl_spin_lock_irqsave(_lock , _flags)  spin_lock_irqsave(_lock, _flags)
#define osl_spin_unlock_irqrestore(_lock , _flags)  spin_unlock_irqrestore(_lock, _flags)
#define osl_spin_trylock(_lock )  spin_trylock(_lock )
static inline unsigned int osl_spin_lock_deinit(__attribute__((unused)) osl_spinlock_t* _lock)
{
    return OK;
}

#elif defined(__OS_RTOSCK_SMP__)
#include <osl_cpu.h>
#include "osl_irq.h"
#include "osl_thread.h"
#include "osl_barrier.h"
#ifndef I_BIT
#define I_BIT   (1<<7)
#endif

#ifndef F_BIT
#define F_BIT   (1<<6)
#endif

#ifndef typeof
#define typeof __typeof__
#endif


//lint -esym(607,*)
#define MODEM_SMP(smp, up)					      \
	"9998:	" smp "\n"					          \
	"	.pushsection \".alt.smp.init\", \"a\"\n"  /*lint !e607*/\
	"	.long	9998b\n"				\
	"	" up "\n"					\
	"	.popsection\n"

//lint +esym(607,*)

#define SEV		MODEM_SMP("sev", "nop")
#define WFE(cond)	MODEM_SMP("wfe" cond, "nop")

static inline void dsb_sev(void)
{
	__asm__ __volatile__ (
		"dsb\n"
		SEV
	);
}
typedef struct spinlock {
		union {
			u32 slock;
			struct __raw_tickets {
				u16 owner;
				u16 next;
			} tickets;
		};
} spinlock_t;
#define DEFINE_SPINLOCK(_lock)	spinlock_t _lock = (spinlock_t ) { .slock = 0x0 }
#ifndef ACCESS_ONCE
#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))
#endif
#define wfe()	__asm__ __volatile__ ("wfe" : : : "memory")


static inline void raw_smp_spin_lock(spinlock_t *p_lock)
{
	unsigned long tmp;
	u32 newval;
	spinlock_t lock_val;

	__asm__ __volatile__(
	"1:	ldrex	%0, [%3]\n"
	"	add	%1, %0, %4\n"
	"	strex	%2, %1, [%3]\n"
	"	teq	%2, #0\n"
	"	bne	1b"
	: "=&r" (lock_val), "=&r" (newval), "=&r" (tmp)
	: "r" (&p_lock->slock), "I" (1 << 16)
	: "cc");

	while ( lock_val.tickets.owner != lock_val.tickets.next) {/*lint !e530*/
		wfe();
		lock_val.tickets.owner = ACCESS_ONCE(p_lock->tickets.owner);
	}

	smp_mb();
}/*lint !e529*/

static inline void raw_smp_spin_unlock(spinlock_t *p_lock)
{
	smp_mb();
	p_lock->tickets.owner++;
	dsb_sev();
}

#define spin_lock_init(__specific_lock)				\
do {							\
	((spinlock_t*)__specific_lock)->slock = 0x0;		\
} while (0)

static inline int raw_smp_spin_trylock(spinlock_t *p_lock)
{
	volatile unsigned long flag, resource;
	u32 slock;

	do {
		__asm__ __volatile__(
		"	ldrex	%0, [%3]\n"
		"	mov	%2, #0\n"
		"	subs	%1, %0, %0, ror #16\n"
		"	addeq	%0, %0, %4\n"
		"	strexeq	%2, %0, [%3]"
		: "=&r" (slock), "=&r" (flag), "=&r" (resource)
		: "r" (&p_lock->slock), "I" (1 << 16)
		: "cc");
	} while (resource);/*lint !e530*/

	if (!flag) {/*lint !e530*/
		smp_mb();
		return 1;
	} else {
		return 0;
	}
}/*lint !e529*/

 /*lint -save -e553*/
 static inline void spin_lock(spinlock_t *p_lock)
{
#if (NR_CPUS > 1)
	raw_smp_spin_lock(p_lock);
#endif
}
 static inline void spin_unlock(spinlock_t *p_lock)
{
#if (NR_CPUS > 1)
	raw_smp_spin_unlock(p_lock);
#endif
}
 /*lint -restore +e553*/

 static inline int spin_trylock(spinlock_t *p_lock)
{
	return raw_smp_spin_trylock(p_lock);
}


/*
*参数类型
*spinlock_t *  __specific_lock,
*unsigned long __specific_flags
*/
//lint -esym(683,*)
#define spin_lock_irqsave(__specific_lock, __specific_flags)				\
do { \
		local_irq_save(__specific_flags); \
		if(!osl_int_context_disirq()&&!(__specific_flags&I_BIT)) \
		{osl_task_lock_disirq();}\
		spin_lock(__specific_lock); \
	} while (0)/*lint !e683*/

/*
*参数类型
*spinlock_t *  __specific_lock,
*unsigned long __specific_flags
*/
#define spin_unlock_irqrestore(__specific_lock, __specific_flags)				\
do { \
    spin_unlock(__specific_lock); \
    local_irq_restore(__specific_flags);   \
    if(!osl_int_context()&&!(__specific_flags&I_BIT)) \
	{osl_task_unlock();}\
    } while (0)/*lint !e683*/

//lint +esym(683,*)

/* new osl_spin interface for both arm and txp */
typedef spinlock_t osl_spinlock_t;
/* 要判断返回值 */
static inline unsigned int osl_spin_lock_init(osl_spinlock_t* _lock)
{
    spin_lock_init(_lock);
    return OK;
}
#define osl_spin_lock_irqsave(_lock , _flags)  spin_lock_irqsave(_lock, _flags)
#define osl_spin_unlock_irqrestore(_lock , _flags)  spin_unlock_irqrestore(_lock, _flags)
#define osl_spin_trylock(_lock )  spin_trylock(_lock )
static inline unsigned int osl_spin_lock_deinit(__attribute__((unused)) osl_spinlock_t* _lock)
{
    return OK;
}

#elif defined(__CMSIS_RTOS) || defined(__M3_OS_RTOSCK__)
#include "osl_types.h"
#include "osl_irq.h"

typedef  s32 spinlock_t;

#define spin_lock_init(lock)    \
do{ 	\
	*lock = *lock; \
}while(0)


#define spin_lock_irqsave(lock, flags)    \
do{ 	\
	UNUSED(lock);\
	local_irq_save(flags); \
}while(0)

#define spin_unlock_irqrestore(lock, flags)    \
do{ 	\
	UNUSED(lock);\
	local_irq_restore(flags); \
}while(0)
#elif defined(__OS_RTOSCK_TSP__)|| defined(__OS_RTOSCK_TVP__)

#else
#include "osl_types.h"

typedef int spinlock_t;

#define spin_lock_init(lock)
#define spin_lock_irqsave(lock, flags)
#define spin_unlock_irqrestore(lock, flags)

#endif /* __KERNEL__ */

#endif

