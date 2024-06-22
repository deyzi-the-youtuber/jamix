section .text
  global context_switch
  global sched_tasking_enter
  extern tss_set_stack
  extern task_stack_dump

; sched_tasking_enter:
; arguments
;   rdi = new rsp
sched_tasking_enter:
  cli
  ; load task's stack
  mov rsp, rdi
  ; load task's registers
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rsi
  pop rdi
  pop rbp
  pop rdx
  pop rcx
  pop rbx
  pop rax
  iretq

; context_switch:
; arguments:
;   rdi = current context
;   rsi = next rsp
context_switch:
  cli
  ; save old task's stack + registers
  push rax
  push rbx
  push rcx
  push rdx
  push rbp
  push rdi
  push rsi
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  mov [rdi], rsp
  ; now load new task's stack + registers
  mov rsp, rsi
  ; restore all task registers
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rsi
  pop rdi
  pop rbp
  pop rdx
  pop rcx
  pop rbx
  pop rax
  iretq
