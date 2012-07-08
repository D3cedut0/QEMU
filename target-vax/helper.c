#include "cpu.h"
#include "gdbstub.h"
#include "helper.h"
#include "host-utils.h"
#include "sysemu.h"

void do_interrupt(CPUVAXState *env)
{
	assert(0);
}

target_phys_addr_t cpu_get_phys_page_debug(CPUVAXState *env, target_ulong addr)
{
	assert(0);
}
