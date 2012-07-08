/*
 *  QEMU VAX CPU
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

#include "cpu.h"
#include "qemu-common.h"

typedef struct VAXCPUInfo {
	const char *name;
	void (*initfn)(Object *obj);
} VAXCPUInfo;

static const VAXCPUInfo vax_cpus[] = {
};

static void vax_cpu_class_init(ObjectClass *oc, void *data)
{
	assert(0);
}

static void vax_cpu_initfn(Object *obj)
{
	assert(0);
}

static void cpu_register(const VAXCPUInfo *info)
{
    TypeInfo type_info = {
        .name = info->name,
        .parent = TYPE_VAX_CPU,
        .instance_size = sizeof(VAXCPU),
        .instance_init = info->initfn,
        .class_size = sizeof(VAXCPUClass),
    };

    type_register_static(&type_info);
}

static const TypeInfo vax_cpu_type_info = {
	.name			= TYPE_VAX_CPU,
	.parent			= TYPE_CPU,
	.instance_size		= sizeof(VAXCPU),
	.instance_init		= vax_cpu_initfn,
	.abstract		= true,
	.class_size		= sizeof(VAXCPUClass),
	.class_init		= vax_cpu_class_init,
};

static void vax_cpu_register_types(void)
{
	int i;

	type_register_static(&vax_cpu_type_info);
	for (i = 0; i < ARRAY_SIZE(vax_cpus); i++) {
		cpu_register(&vax_cpus[i]);
	}
}

type_init(vax_cpu_register_types)
