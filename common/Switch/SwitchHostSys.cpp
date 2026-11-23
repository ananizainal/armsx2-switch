// SPDX-FileCopyrightText: 2002-2026 PCSX2 Dev Team / ARMSX2 Team
// SPDX-License-Identifier: GPL-3.0+

#include "common/Assertions.h"
#include "common/BitUtils.h"
#include "common/Console.h"
#include "common/CrashHandler.h"
#include "common/Error.h"
#include "common/HostSys.h"
#include "common/Switch/SwitchMisc.h"

#if defined(__SWITCH__)
#include <switch.h>
#endif

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <malloc.h>
#include <unistd.h>

#include "fmt/format.h"

#ifdef PCSX2_RUNTIME_PAGE_SIZE
const unsigned int __pagesize = 4096;
const unsigned int __pagemask = __pagesize - 1;
const unsigned int __pageshift = 12;
#endif

void HostSys::MemProtect(void* baseaddr, size_t size, const PageProtectionMode& mode)
{
	// On Horizon OS, userland mprotect/svcSetProcessMemoryPermission is restricted
	// for dynamic code. Code write protection is safely handled via the RW mirror in SwitchMisc.
}

std::string HostSys::GetFileMappingName(const char* prefix)
{
	return fmt::format("armsx2_switch_{}_{}", prefix, getpid());
}

void* HostSys::CreateSharedMemory(const char* name, size_t size)
{
	void* ptr = memalign(4096, size);
	if (ptr)
		std::memset(ptr, 0, size);
	return ptr;
}

void HostSys::DestroySharedMemory(void* ptr)
{
	std::free(ptr);
}

bool HostSys::DiscardSharedMemory(void* ptr, size_t offset, size_t size)
{
	if (ptr && size > 0)
	{
		std::memset(static_cast<u8*>(ptr) + offset, 0, size);
		return true;
	}
	return false;
}

void HostSys::DiscardMemory(void* ptr, size_t size)
{
	if (ptr && size > 0)
		std::memset(ptr, 0, size);
}

void HostSys::FlushInstructionCache(void* address, u32 size)
{
	SwitchMisc::FlushInstructionCache(address, size);
}

size_t HostSys::GetRuntimePageSize()
{
	return 4096;
}

size_t HostSys::GetRuntimeCacheLineSize()
{
	// ARM Cortex-A57 has 64-byte L1/L2 cache lines
	return 64;
}

namespace PageFaultHandler
{
	bool Install(Error* error)
	{
		// Horizon OS does not expose POSIX signals (SIGSEGV).
		// By utilizing Software TLB for EE memory translation, page faults are handled in software.
		return true;
	}

	bool InstallSecondaryThread()
	{
		return true;
	}

	void SetUnhandledFaultReporter(UnhandledFaultReporter reporter)
	{
	}
} // namespace PageFaultHandler

SharedMemoryMappingArea::SharedMemoryMappingArea(u8* base_ptr, size_t size, size_t num_pages)
	: m_base_ptr(base_ptr), m_size(size), m_num_pages(num_pages)
{
}

SharedMemoryMappingArea::~SharedMemoryMappingArea()
{
	if (m_base_ptr)
	{
		// Check if this was a dual-mapped JIT arena or standard heap
		if (m_base_ptr == reinterpret_cast<u8*>(SwitchMisc::GetJitBase()))
		{
			SwitchMisc::MunmapCodeDualMap(m_base_ptr, m_size);
		}
		else
		{
			std::free(m_base_ptr);
		}
		m_base_ptr = nullptr;
	}
}

std::unique_ptr<SharedMemoryMappingArea> SharedMemoryMappingArea::Create(size_t size, bool jit, uptr fixed_base_hint)
{
	const size_t aligned_size = (size + 4095) & ~static_cast<size_t>(4095);
	void* alloc = nullptr;

	if (jit)
	{
		alloc = SwitchMisc::MmapCodeDualMap(aligned_size);
		if (!alloc)
		{
			std::fprintf(stderr, "[Switch] Failed to allocate dual-mapped JIT memory of size %zu\n", aligned_size);
			return nullptr;
		}
	}
	else
	{
		alloc = memalign(4096, aligned_size);
		if (alloc)
			std::memset(alloc, 0, aligned_size);
	}

	if (!alloc)
		return nullptr;

	return std::unique_ptr<SharedMemoryMappingArea>(new SharedMemoryMappingArea(static_cast<u8*>(alloc), aligned_size, aligned_size / 4096));
}

u8* SharedMemoryMappingArea::Map(void* file_handle, size_t file_offset, void* map_base, size_t map_size, const PageProtectionMode& mode)
{
	return static_cast<u8*>(map_base ? map_base : m_base_ptr);
}

bool SharedMemoryMappingArea::Unmap(void* map_base, size_t map_size, bool is_file)
{
	return true;
}

u64 GetTickFrequency()
{
#if defined(__SWITCH__)
	return armGetSystemTickFreq();
#else
	return 19200000;
#endif
}

u64 GetCPUTicks()
{
#if defined(__SWITCH__)
	return armGetSystemTick();
#else
	return 0;
#endif
}

u64 GetPhysicalMemory()
{
	// Total available RAM in Title Override / Game redirection mode (~3.2 GB)
	return 3ULL * 1024 * 1024 * 1024;
}

u64 GetAvailablePhysicalMemory()
{
	return 2ULL * 1024 * 1024 * 1024;
}

const u32 SPIN_TIME_NS = 2000;

u32 ShortSpin()
{
#if defined(__SWITCH__)
	armWaitMicroseconds(1);
#endif
	return 1000;
}

u32 ShortSpinOn(const std::atomic<s32>& word, s32 expected)
{
	return ShortSpin();
}

[[noreturn]] void AbortWithMessage(const char* msg)
{
	std::fprintf(stderr, "FATAL ABORT: %s\n", msg);
	std::abort();
}

std::string GetOSVersionString()
{
	return "Horizon OS (Nintendo Switch Atmosphere CFW)";
}

const CPUInfo& GetCPUInfo()
{
	static CPUInfo info = {
		.name = "Nvidia Tegra X1 (4x ARM Cortex-A57)",
		.num_big_cores = 4,
		.num_small_cores = 0,
		.num_threads = 4,
		.num_clusters = 1,
	};
	return info;
}

namespace Common
{
	bool InhibitScreensaver(bool inhibit)
	{
#if defined(__SWITCH__)
		appletSetMediaPlaybackState(inhibit);
		return true;
#else
		return false;
#endif
	}

	bool PlaySoundAsync(const char* path)
	{
		return false;
	}

	void SetMousePosition(int x, int y) {}
	bool AttachMousePositionCb(std::function<void(int, int)> cb) { return false; }
	void DetachMousePositionCb() {}
} // namespace Common
