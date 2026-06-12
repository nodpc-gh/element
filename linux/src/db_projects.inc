; ============================================================
; Element IDE - Linux Projects Database (db_projects.inc)
; ============================================================

project_prefix db 'project:', 0

; Чтение имени по data_offset (EAX)
db_read_record_name:
    push ebx ecx edx edi
    mov ebx, eax
    add ebx, 4
    sys_lseek [db_handle], ebx, SEEK_SET
    sys_read [db_handle], read_buffer, MAX_NAME_LEN
    pop edi edx ecx ebx
    ret
