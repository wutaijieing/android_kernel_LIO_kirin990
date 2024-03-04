
#ifndef _HWAA_TRUSTED_ZYGOTE_H
#define _HWAA_TRUSTED_ZYGOTE_H
#ifndef CONFIG_DISABLE_HWAA
#include <linux/types.h>

s32 hwaa_trusted_zygote_lookup(const s8 *exe);

/* checks if the process is actaually forked by zygote */
bool hwaa_trusted_zygote_has_exe_path(pid_t pid);
#endif
#endif
