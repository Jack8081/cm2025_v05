
/* auto-generated by gen_syscalls.py, don't edit */
#ifndef Z_INCLUDE_SYSCALLS_PWM_H
#define Z_INCLUDE_SYSCALLS_PWM_H


#ifndef _ASMLANGUAGE

#include <syscall_list.h>
#include <syscall.h>

#include <linker/sections.h>

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#if !defined(__XCC__)
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern int z_impl_pwm_pin_set_cycles(const struct device * dev, uint32_t pwm, uint32_t period, uint32_t pulse, pwm_flags_t flags);

__pinned_func
static inline int pwm_pin_set_cycles(const struct device * dev, uint32_t pwm, uint32_t period, uint32_t pulse, pwm_flags_t flags)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		/* coverity[OVERRUN] */
		return (int) arch_syscall_invoke5(*(uintptr_t *)&dev, *(uintptr_t *)&pwm, *(uintptr_t *)&period, *(uintptr_t *)&pulse, *(uintptr_t *)&flags, K_SYSCALL_PWM_PIN_SET_CYCLES);
	}
#endif
	compiler_barrier();
	return z_impl_pwm_pin_set_cycles(dev, pwm, period, pulse, flags);
}


extern int z_impl_pwm_pin_set_duty(const struct device * dev, uint32_t pwm, uint32_t pulse);

__pinned_func
static inline int pwm_pin_set_duty(const struct device * dev, uint32_t pwm, uint32_t pulse)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		/* coverity[OVERRUN] */
		return (int) arch_syscall_invoke3(*(uintptr_t *)&dev, *(uintptr_t *)&pwm, *(uintptr_t *)&pulse, K_SYSCALL_PWM_PIN_SET_DUTY);
	}
#endif
	compiler_barrier();
	return z_impl_pwm_pin_set_duty(dev, pwm, pulse);
}


extern int z_impl_pwm_pin_enable_capture(const struct device * dev, uint32_t pwm);

__pinned_func
static inline int pwm_pin_enable_capture(const struct device * dev, uint32_t pwm)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		/* coverity[OVERRUN] */
		return (int) arch_syscall_invoke2(*(uintptr_t *)&dev, *(uintptr_t *)&pwm, K_SYSCALL_PWM_PIN_ENABLE_CAPTURE);
	}
#endif
	compiler_barrier();
	return z_impl_pwm_pin_enable_capture(dev, pwm);
}


extern int z_impl_pwm_pin_disable_capture(const struct device * dev, uint32_t pwm);

__pinned_func
static inline int pwm_pin_disable_capture(const struct device * dev, uint32_t pwm)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		/* coverity[OVERRUN] */
		return (int) arch_syscall_invoke2(*(uintptr_t *)&dev, *(uintptr_t *)&pwm, K_SYSCALL_PWM_PIN_DISABLE_CAPTURE);
	}
#endif
	compiler_barrier();
	return z_impl_pwm_pin_disable_capture(dev, pwm);
}


extern int z_impl_pwm_pin_capture_cycles(const struct device * dev, uint32_t pwm, pwm_flags_t flags, uint32_t * period, uint32_t * pulse, k_timeout_t timeout);

__pinned_func
static inline int pwm_pin_capture_cycles(const struct device * dev, uint32_t pwm, pwm_flags_t flags, uint32_t * period, uint32_t * pulse, k_timeout_t timeout)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		union { struct { uintptr_t lo, hi; } split; k_timeout_t val; } parm0;
		parm0.val = timeout;
		uintptr_t more[] = {
			parm0.split.lo,
			parm0.split.hi
		};
		/* coverity[OVERRUN] */
		return (int) arch_syscall_invoke6(*(uintptr_t *)&dev, *(uintptr_t *)&pwm, *(uintptr_t *)&flags, *(uintptr_t *)&period, *(uintptr_t *)&pulse, (uintptr_t) &more, K_SYSCALL_PWM_PIN_CAPTURE_CYCLES);
	}
#endif
	compiler_barrier();
	return z_impl_pwm_pin_capture_cycles(dev, pwm, flags, period, pulse, timeout);
}


extern int z_impl_pwm_get_cycles_per_sec(const struct device * dev, uint32_t pwm, uint64_t * cycles);

__pinned_func
static inline int pwm_get_cycles_per_sec(const struct device * dev, uint32_t pwm, uint64_t * cycles)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		/* coverity[OVERRUN] */
		return (int) arch_syscall_invoke3(*(uintptr_t *)&dev, *(uintptr_t *)&pwm, *(uintptr_t *)&cycles, K_SYSCALL_PWM_GET_CYCLES_PER_SEC);
	}
#endif
	compiler_barrier();
	return z_impl_pwm_get_cycles_per_sec(dev, pwm, cycles);
}


#ifdef __cplusplus
}
#endif

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

#endif
#endif /* include guard */
