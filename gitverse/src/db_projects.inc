; ============================================================
; Element IDE - Projects Database (db_projects.inc)
; Работа с проектами в единой БД element.db
; Проекты хранятся с префиксом "project:"
; ============================================================

project_prefix db 'project:', 0

; ----- ЧТЕНИЕ ИМЕНИ ЗАПИСИ ПО DATA_OFFSET -----
db_read_record_name:
    push ebx ecx edx edi
    mov ebx, eax

    add ebx, 4  ; пропускаем DBRecord.id
    invoke SetFilePointer, [db_handle], ebx, 0, FILE_BEGIN
    invoke ReadFile, [db_handle], read_buffer, MAX_NAME_LEN, bytes_read, 0

    pop edi edx ecx ebx
    ret
