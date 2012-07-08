/*
 * QEMU VAX CPU
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

#ifndef QEMU_VAX_CPU_QOM_H
#define QEMU_VAX_CPU_QOM_H

#include "qemu/cpu.h"

#define TYPE_VAX_CPU "vax-cpu"

#define VAX_CPU_CLASS(klass) \
    OBJECT_CLASS_CHECK(VAXCPUClass, (klass), TYPE_VAX_CPU)
#define VAX_CPU(obj) \
    OBJECT_CHECK(VAXCPU, (obj), TYPE_VAX_CPU)
#define VAX_CPU_GET_CLASS(obj) \
    OBJECT_GET_CLASS(VAXCPUClass, (obj), TYPE_VAX_CPU)

/**
 * VAXCPUClass:
 * @parent_reset: The parent class' reset handler.
 *
 * An VAX CPU model.
 */
typedef struct VAXCPUClass {
    /*< private >*/
    CPUClass parent_class;
    /*< public >*/

    void (*parent_reset)(CPUState *cpu);
} VAXCPUClass;

/**
 * VAXCPU:
 * @env: #CPUVAXState
 *
 * An VAX CPU core.
 */
typedef struct VAXCPU {
    /*< private >*/
    CPUState parent_obj;
    /*< public >*/

    CPUVAXState env;
} VAXCPU;

static inline VAXCPU *vax_env_get_cpu(CPUVAXState *env)
{
    return VAX_CPU(container_of(env, VAXCPU, env));
}

#define ENV_GET_CPU(e) CPU(vax_env_get_cpu(e))

void vax_cpu_realize(VAXCPU *cpu);

#endif
