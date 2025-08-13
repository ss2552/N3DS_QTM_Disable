.arm
.balign 4

.section .text.svcControlProcess, "ax", %progbits
.global svcControlProcess
.type svcControlProcess, %function
.align 2
svcControlProcess:
    svc 0xB3
    bx lr
.cfi_startproc
.cfi_endproc
