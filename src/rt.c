#include "global.h"

#include "3ds/services/soc.h"
#include "3ds/services/fs.h"

#include <memory.h>
#include <errno.h>

u32 rtGetPageOfAddress(u32 addr) {
	return PAGE_OF_ADDR(addr);
}

u32 rtCheckRemoteMemory(Handle hProcess, u32 addr, u32 size, MemPerm perm) {
	MemInfo memInfo;
	PageInfo pageInfo;
	s32 ret = svcQueryMemory(&memInfo, &pageInfo, addr);
	if (ret != 0) {
		nsDbgPrint("svcQueryMemory failed for addr %08"PRIx32": %08"PRIx32"\n", addr, ret);
		return ret;
	}
	if (memInfo.perm == 0) {
		return -1;
	}
	if (memInfo.base_addr + memInfo.size < addr + size) {
		return -1;
	}

	if (perm & MEMPERM_WRITE)
		perm |= MEMPERM_READ;
	if ((memInfo.perm & perm) == perm) {
		return 0;
	}

	perm |= memInfo.perm;

	u32 startPage, endPage;

	startPage = rtGetPageOfAddress(addr);
	endPage = rtGetPageOfAddress(addr + size - 1);
	size = endPage - startPage + 0x1000;

	ret = protectRemoteMemory(hProcess, (void *)startPage, size, perm);
	return ret;
}
