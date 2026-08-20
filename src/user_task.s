.global user_task
.global user_task_end

user_task:
  ecall
  j user_task
user_task_end:
