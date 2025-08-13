#include <3ds/types.h>
#include <3ds/svc.h>
#include <3ds/console.h>
#include <3ds/gfx.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <memory.h>

void rpDoQTMPatchAndToggle(void);

static int qtmPatched = 0;
static int qtmPayloadAddr = 0;
extern int qtmDisabled;

extern int result;

static u32 y_point = 1;

static u32 currentPid = 0;
u32 getCurrentProcessId(void)
{
	if (currentPid != 0)
		return currentPid;
	svcGetProcessId(&currentPid, CUR_PROCESS_HANDLE);
	return currentPid;
}

u32 protectRemoteMemory(Handle hProcess, void *addr, u32 size, u32 perm)
{
	return svcControlProcessMemory(hProcess, (u32)addr, 0, size, MEMOP_PROT, perm);
}

u32 copyRemoteMemoryTimeout(Handle hDst, void *ptrDst, Handle hSrc, void *ptrSrc, u32 size, s64 timeout)
{
	u8 dmaConfig[sizeof(DmaConfig)] = {-1, 0, 4};
	u32 hdma = 0;
	u32 ret;

	ret = svcFlushProcessDataCache(hSrc, (u32)ptrSrc, size);
	if (ret != 0)
	{
		printf("\x1b[%u;1H@svcFlushProcessDataCache src failed: %lu\n", y_point += 2, ret);
		return ret;
	}
	ret = svcFlushProcessDataCache(hDst, (u32)ptrDst, size);
	if (ret != 0)
	{
		printf("\x1b[%u;1H@svcFlushProcessDataCache dst failed: %lu\n", y_point += 2, ret);
		return ret;
	}

	ret = svcStartInterProcessDma(&hdma, hDst, (u32)ptrDst, hSrc, (u32)ptrSrc, size, (DmaConfig *)dmaConfig);
	if (ret != 0)
	{
		printf("\x1b[%u;1H@svcStartInterProcessDma failed: %lu\n", y_point += 2, ret);
		return ret;
	}
	ret = svcWaitSynchronization(hdma, timeout);
	if (ret != 0)
	{
		printf("\x1b[%u;1H@copyRemoteMemory time out (or error) %lu", y_point += 2, ret);
		svcCloseHandle(hdma);
		return 1;
	}

	svcCloseHandle(hdma);
	ret = svcInvalidateProcessDataCache(hDst, (u32)ptrDst, size);
	if (ret != 0)
	{
		printf("\x1b[%u;1H@svcInvalidateProcessDataCache failed: %lu\n", y_point += 2, ret);
		return ret;
	}
	return 0;
}

#define COPY_REMOTE_MEMORY_TIMEOUT (-1)

u32 copyRemoteMemory(Handle hDst, void *ptrDst, Handle hSrc, void *ptrSrc, u32 size)
{
	return copyRemoteMemoryTimeout(hDst, ptrDst, hSrc, ptrSrc, size, COPY_REMOTE_MEMORY_TIMEOUT);
}

#define PAGE_OF_ADDR(addr) ((addr) / 0x1000 * 0x1000)

u32 rtGetPageOfAddress(u32 addr)
{
	return PAGE_OF_ADDR(addr);
}

u32 rtCheckRemoteMemory(Handle hProcess, u32 addr, u32 size, MemPerm perm)
{
	MemInfo memInfo;
	PageInfo pageInfo;
	s32 ret = svcQueryMemory(&memInfo, &pageInfo, addr);
	if (ret != 0)
	{
		printf("\x1b[%u;1H@svcQueryMemory failed for addr %ld: %ld\n", y_point += 2, addr, ret);
		return ret;
	}
	if (memInfo.perm == 0)
	{
		return -1;
	}
	if (memInfo.base_addr + memInfo.size < addr + size)
	{
		return -1;
	}

	if (perm & MEMPERM_WRITE)
		perm |= MEMPERM_READ;
	if ((memInfo.perm & perm) == perm)
	{
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

#define RP_QTM_HDR_SIZE (4)

typedef enum ProcessOp
{
	PROCESSOP_GET_ALL_HANDLES,

	PROCESSOP_SET_MMU_TO_RWX,

	PROCESSOP_GET_ON_MEMORY_CHANGE_EVENT,

	PROCESSOP_SIGNAL_ON_EXIT,

	PROCESSOP_GET_PA_FROM_VA,

	PROCESSOP_SCHEDULE_THREADS,

} ProcessOp;

Result svcControlProcess(Handle process, ProcessOp op, u32 varg2, u32 varg3);

// When enabling this patch, the qtm camera service usage is more or less disabled.
// This increases performance on New 3DS for remote play by up to 20%.
// Performance on New 3DS is still ~5% slower than New 2DS despite the improvement.
// Still trying to figure out where that last difference comes from...
// Note: head tracking is always active even if 3D is turned off, whether by slider or
// through system settings. Hence this patch is necessary for performance.
void rpDoQTMPatchAndToggle(void)
{
	u8 desiredHeader[RP_QTM_HDR_SIZE] = {
		0x32,
		0x00,
		0x00,
		0xef,
	};

	u32 remotePC = 0x00119a48;
	Handle hProcess;
	s32 ret;
	u32 pid = 0x15; // QTM process

	u8 buf[RP_QTM_HDR_SIZE] = {0};

	ret = svcOpenProcess(&hProcess, pid);
	if (ret != 0)
	{
		printf("\x1b[%u;1H@Open QTM process failed: %ld", y_point += 2, ret);
		hProcess = 0;
		goto final;
	}

	if (!qtmPatched)
	{
		ret = copyRemoteMemory(CUR_PROCESS_HANDLE, buf, hProcess, (void *)remotePC, RP_QTM_HDR_SIZE);
		if (ret != 0)
		{
			printf("\x1b[%u;1H@Read QTM memory at %ld failed: %ld", y_point += 2, remotePC, ret);
			goto final;
		}

		if (memcmp(buf, desiredHeader, RP_QTM_HDR_SIZE) != 0)
		{
			printf("\x1b[%u;1H@Unexpected QTM memory content", y_point += 2);
			goto final;
		}
	}

	ret = svcControlProcess(hProcess, PROCESSOP_SCHEDULE_THREADS, 1, 0);
	if (ret != 0)
	{
		printf("\x1b[%u;1H@Locking QTM failed: %ld\n", y_point += 2, ret);
		goto final;
	}

#define RP_QTM_PAYLOAD_SIZE (32)
	u8 payload[RP_QTM_PAYLOAD_SIZE] = {
		0x01, 0x01, 0xA0, 0xE3, // mov r0, #0x40000000
		0x00, 0x10, 0xA0, 0xE3, // mov r1, #0
		0x0A, 0x00, 0x00, 0xEF, // svc #0xa
		0x00, 0x20, 0xA0, 0xE3, // mov r2, #0
		0x00, 0x30, 0xA0, 0xE3, // mov r3, #0
		0x0F, 0x00, 0x85, 0xE8, // stm r5, {r0, r1, r2, r3}
		0x70, 0x80, 0xBD, 0xE8, // ldmia sp!, {r4, r5, r6, pc}
		0x00, 0xF0, 0x20, 0xE3, // nop
	};

	if (!qtmPatched)
	{
		ret = rtCheckRemoteMemory(hProcess, remotePC, RP_QTM_HDR_SIZE, MEMPERM_READWRITE | MEMPERM_EXECUTE);
		if (ret != 0)
		{
			printf("\x1b[%u;1H@QTM protectRemoteMemory failed: %ld", y_point += 2, ret);
			goto final_unlock;
		}

		u32 qtmBinEnd = 0x001ac000;

		u32 qtmPayloadAddrMin = qtmBinEnd - 0x800;
		u32 qtmPayloadAddrTry = qtmBinEnd - RP_QTM_PAYLOAD_SIZE;

		while (1)
		{
		retry:
			if (qtmPayloadAddrTry < qtmPayloadAddrMin)
			{
				printf("\x1b[%u;1H@Unable to find free space to install QTM payload\n", y_point += 2);
				goto final_unlock;
			}

			u8 tmp[RP_QTM_PAYLOAD_SIZE] = {0};

			ret = copyRemoteMemory(CUR_PROCESS_HANDLE, tmp, hProcess, (void *)qtmPayloadAddrTry, RP_QTM_PAYLOAD_SIZE);
			if (ret != 0)
			{
				printf("\x1b[%u;1H@Read QTM memory at %ld failed: %ld", y_point += 2, qtmPayloadAddrTry, ret);
				goto final_unlock;
			}

			for (unsigned i = 0; i < RP_QTM_PAYLOAD_SIZE / sizeof(u32); ++i)
			{
				if (((u32 *)tmp)[i])
				{
					qtmPayloadAddrTry -= RP_QTM_PAYLOAD_SIZE;
					goto retry;
				}
			}

			break;
		}

		ret = rtCheckRemoteMemory(hProcess, qtmPayloadAddrTry, RP_QTM_PAYLOAD_SIZE, MEMPERM_READWRITE | MEMPERM_EXECUTE);
		if (ret != 0)
		{
			printf("\x1b[%u;1H@QTM protectRemoteMemory for payload failed: %ld", y_point += 2, ret);
			goto final_unlock;
		}

		ret = copyRemoteMemory(hProcess, (void *)qtmPayloadAddrTry, CUR_PROCESS_HANDLE, payload, RP_QTM_PAYLOAD_SIZE);
		if (ret != 0)
		{
			printf("\x1b[%u;1H@Write QTM memory for payload at %ld failed: %ld", y_point += 2, qtmPayloadAddrTry, ret);
			goto final_unlock;
		}

		qtmPayloadAddr = qtmPayloadAddrTry;
		qtmPatched = 1;
	}

	if (!qtmDisabled)
	{
		u32 branchDistance = qtmPayloadAddr - remotePC;

		u32 replacementInst = (branchDistance / 4 - 2) | 0xea000000; // b inst

		ret = copyRemoteMemory(hProcess, (void *)remotePC, CUR_PROCESS_HANDLE, &replacementInst, RP_QTM_HDR_SIZE);
		if (ret != 0)
		{
			printf("\x1b[%u;1H@Write QTM memory at %ld failed: %ld", y_point += 2, remotePC, ret);
			goto final_unlock;
		}

		qtmDisabled = 1;
		result = 1;
		printf("\x1b[%u;1H@Patch QTM success", y_point += 2);
	}
	else
	{
		ret = copyRemoteMemory(hProcess, (void *)remotePC, CUR_PROCESS_HANDLE, desiredHeader, RP_QTM_HDR_SIZE);
		if (ret != 0)
		{
			printf("\x1b[%u;1H@Restore QTM memory at %ld failed: %ld", y_point += 2, remotePC, ret);
			goto final_unlock;
		}

		qtmDisabled = 0;
		result = -1;
		printf("\x1b[%u;1H@Restore QTM success", y_point += 2);
	}

final_unlock:
	ret = svcControlProcess(hProcess, PROCESSOP_SCHEDULE_THREADS, 0, 0);
	if (ret != 0)
	{
		goto final;
		printf("\x1b[%u;1H@Unlocking QTM process failed: %ld\n", y_point += 2, ret);
	}

final:
	if (hProcess)
		svcCloseHandle(hProcess);
}
