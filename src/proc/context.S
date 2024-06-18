section .text
  global context_switch
  global sched_task_init

; rdi = new rsp
sched_task_init:
  ; Load task's stack
  mov rsp, rdi
  ; Load task's registers
  pop rsi
  pop rdi
  pop rdx
  pop rcx
  pop rbx
  pop rax
  ; This will leave out the task entry point, which will be popped into RIP
  ret

; rdi = current
; rsi = next rsp
context_switch:
  cli
  ; We should NOT add the following: "push rbp"
  ; By doing so, we create a stack frame that could mess up the current processes stack.

  ; Save old task's stack + registers
  push rax
  push rbx
  push rcx
  push rdx
  push rdi
  push rsi
  
  mov rax, [rdi + 0x40]     ; current->ctx
  mov [rax + 0x20], rsp     ; current->ctx.rsp = current_rsp

  ; Now load new task's stack + registers
  mov rsp, rsi

  pop rsi
  pop rdi
  pop rdx
  pop rcx
  pop rbx
  pop rax

  sti

  ret     ; Perform RET instruction to complete the context switch
