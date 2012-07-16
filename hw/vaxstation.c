/*
 *  QEMU VAXstation System Emulator
 *
 *  Copyright (c) 2012 Pekka Enberg
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "hw/boards.h"
#include "hw/qdev.h"

static void
vaxstation_4000_90_init(ram_addr_t RAM_size,
			const char *boot_device,
			const char *kernel_filename,
			const char *kernel_cmdline,
			const char *initrd_filename,
			const char *cpu_model)
{
}

static QEMUMachine vaxstation_4000_90_machine = {
	.name		= "VAXstation 4000/90",
	.desc		= "VAXstation 4000 Model 90",
	.init		= vaxstation_4000_90_init,
	.is_default	= 1,
};

static void vaxstation_machine_init(void)
{
	qemu_register_machine(&vaxstation_4000_90_machine);
}

machine_init(vaxstation_machine_init);
