// SPDX-FileCopyrightText: 2002-2026 PCSX2 Dev Team / ARMSX2 Team
// SPDX-License-Identifier: GPL-3.0+

#pragma once

#include "common/Pcsx2Defs.h"
#include <cstddef>
#include <cstdint>

namespace SwitchMisc
{
	// Dual-mapped JIT code offset between RW alias and RX execute space:
	// writable_address = executable_address + g_code_rw_offset
	extern ptrdiff_t g_code_rw_offset;

	/// Allocates a dual-mapped JIT memory region using libnx jitCreate.
	/// Returns the RX (executable) pointer for jumping/PC, while
	/// (rx_ptr + g_code_rw_offset) provides the RW mirror for emitting code.
	void* MmapCodeDualMap(size_t size);

	/// Releases the dual-mapped JIT region and closes the libnx Jit handle.
	void MunmapCodeDualMap(void* ptr, size_t size);

	/// Registers the JIT virtual address boundaries.
	void SetJitRange(void* base, size_t size);

	/// Returns the RX base address of the JIT code arena.
	uintptr_t GetJitBase();

	/// Returns the RX upper bound address of the JIT code arena.
	uintptr_t GetJitEnd();

	/// Checks if JIT services were acquired successfully from Atmosphere.
	bool IsJitAvailable();

	/// Performs data cache flush and instruction cache invalidation for ARM64 CPU.
	void FlushInstructionCache(void* address, u32 size);
} // namespace SwitchMisc
