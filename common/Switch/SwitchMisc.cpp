// SPDX-FileCopyrightText: 2002-2026 PCSX2 Dev Team / ARMSX2 Team
// SPDX-License-Identifier: GPL-3.0+

#include "common/Switch/SwitchMisc.h"

#if defined(__SWITCH__)
#include <switch.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace SwitchMisc
{
	ptrdiff_t g_code_rw_offset = 0;

	static uintptr_t s_jit_base = 0;
	static uintptr_t s_jit_end = 0;
	static bool s_jit_active = false;

#if defined(__SWITCH__)
	static Jit s_switch_jit;
#endif

	void* MmapCodeDualMap(size_t size)
	{
#if defined(__SWITCH__)
		// Align size to page boundary (typically 4KB / 0x1000 or 64KB for Horizon OS)
		const size_t aligned_size = (size + 0xFFF) & ~static_cast<size_t>(0xFFF);

		Result rc = jitCreate(&s_switch_jit, aligned_size);
		if (R_FAILED(rc))
		{
			std::fprintf(stderr,
				"[SwitchMisc] Failed to allocate dual-mapped JIT memory: 0x%08X\n"
				"[SwitchMisc] IMPORTANT: Make sure you launch via Title Override (Hold R) or Forwarder NSP, not Album!\n",
				rc);
			s_jit_active = false;
			return nullptr;
		}

		void* rx_addr = jitGetRxAddress(&s_switch_jit);
		void* rw_addr = jitGetRwAddress(&s_switch_jit);

		if (!rx_addr || !rw_addr)
		{
			std::fprintf(stderr, "[SwitchMisc] jitGetRxAddress or jitGetRwAddress returned null!\n");
			jitClose(&s_switch_jit);
			s_jit_active = false;
			return nullptr;
		}

		s_jit_base = reinterpret_cast<uintptr_t>(rx_addr);
		s_jit_end = s_jit_base + aligned_size;
		g_code_rw_offset = reinterpret_cast<u8*>(rw_addr) - reinterpret_cast<u8*>(rx_addr);
		s_jit_active = true;

		std::printf("[SwitchMisc] Dual-Mapping JIT initialized: RX=%p, RW=%p, offset=%td, size=%zu\n",
			rx_addr, rw_addr, g_code_rw_offset, aligned_size);

		return rx_addr;
#else
		// Fallback mock for non-switch development/linting
		void* ptr = std::malloc(size);
		s_jit_base = reinterpret_cast<uintptr_t>(ptr);
		s_jit_end = s_jit_base + size;
		g_code_rw_offset = 0;
		s_jit_active = (ptr != nullptr);
		return ptr;
#endif
	}

	void MunmapCodeDualMap(void* ptr, size_t size)
	{
#if defined(__SWITCH__)
		if (s_jit_active)
		{
			jitClose(&s_switch_jit);
			s_jit_active = false;
			s_jit_base = 0;
			s_jit_end = 0;
			g_code_rw_offset = 0;
			std::printf("[SwitchMisc] Dual-mapped JIT closed.\n");
		}
#else
		std::free(ptr);
		s_jit_active = false;
		s_jit_base = 0;
		s_jit_end = 0;
		g_code_rw_offset = 0;
#endif
	}

	void SetJitRange(void* base, size_t size)
	{
		s_jit_base = reinterpret_cast<uintptr_t>(base);
		s_jit_end = s_jit_base + size;
	}

	uintptr_t GetJitBase()
	{
		return s_jit_base;
	}

	uintptr_t GetJitEnd()
	{
		return s_jit_end;
	}

	bool IsJitAvailable()
	{
		return s_jit_active;
	}

	void FlushInstructionCache(void* address, u32 size)
	{
#if defined(__SWITCH__)
		// Data cache flush and instruction cache invalidation for ARM Cortex-A57
		armDCacheFlush(address, size);
		armICacheInvalidate(address, size);
#endif
	}
} // namespace SwitchMisc
