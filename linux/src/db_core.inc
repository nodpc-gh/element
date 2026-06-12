; ============================================================
; Element IDE - Linux Core Database Engine (db_core.inc)
; ============================================================

; ----- ИНИЦИАЛИЗАЦИЯ БД -----
element_db_init:
    push ebx ecx edx esi edi

    ; Путь: getcwd + "/element.db"
    sys_getcwd db_path, 260
    lea edi, [db_path]
    mov ecx, 260
.find_end:
    cmp byte [edi], 0
    je .append_name
    inc edi
    loop .find_end
.append_name:
    mov dword [edi], '/ele'
    mov dword [edi+4], 'ment'
    mov dword [edi+8], '.db'
    mov byte [edi+12], 0

    ; Открываем существующий файл
    sys_open db_path, O_RDWR, 0
    cmp eax, 0
    jge .open_ok

    ; Файл не существует — создаём
    sys_open db_path, O_RDWR or O_CREAT or O_TRUNC, 0x1A4
    cmp eax, 0
    jl .error
    mov [db_handle], eax

    ; Записываем заголовок
    mov dword [db_header + DBHeader.signature], 'ELMN'
    mov dword [db_header + DBHeader.signature + 4], 'TDB'
    mov byte [db_header + DBHeader.signature + 6], 0
    mov dword [db_header + DBHeader.version], DB_VERSION
    mov dword [db_header + DBHeader.total_records], 0
    mov dword [db_header + DBHeader.free_offset], sizeof.DBHeader + sizeof.DBIndex * MAX_PROJECTS
    call db_write_header

    jmp .done

.open_ok:
    mov [db_handle], eax
    call db_read_header
    jmp .done

.error:
    xor eax, eax
.done:
    pop edi esi edx ecx ebx
    ret

; ----- ЧТЕНИЕ ЗАГОЛОВКА -----
db_read_header:
    sys_lseek [db_handle], 0, SEEK_SET
    sys_read [db_handle], db_header, sizeof.DBHeader
    ret

; ----- ЗАПИСЬ ЗАГОЛОВКА -----
db_write_header:
    sys_lseek [db_handle], 0, SEEK_SET
    sys_write [db_handle], db_header, sizeof.DBHeader
    ret

; ----- СОЗДАНИЕ ЗАПИСИ -----
; Вход: ESI = имя, EDI = JSON-данные, ECX = размер JSON
; Выход: EAX = ID записи или 0
db_create_record:
    push ebx ecx edx esi edi

    mov esi, [esp + 4 + 4*5]    ; name
    mov edi, [esp + 8 + 4*5]    ; json_data
    mov ecx, [esp + 12 + 4*5]   ; json_size

    ; Проверка дубликата
    push esi
    call db_find_by_name
    add esp, 4
    test eax, eax
    jnz .exists

    mov eax, [db_header + DBHeader.total_records]
    inc eax
    mov dword [db_header + DBHeader.total_records], eax

    mov ebx, [db_header + DBHeader.free_offset]
    mov edx, ebx

    ; Заполняем temp_record
    mov dword [temp_record + DBRecord.id], eax
    mov dword [temp_record + DBRecord.name_len], 0

    ; Копируем имя
    mov esi, [esp + 4 + 4*5]    ; name (перечитаем, т.к. esi мог измениться)
    lea edi, [temp_record + DBRecord.name]
    mov ecx, MAX_NAME_LEN
.copy_name:
    lodsb
    test al, al
    jz .name_done
    stosb
    loop .copy_name
.name_done:
    sub edi, temp_record + DBRecord.name
    mov dword [temp_record + DBRecord.name_len], edi

    ; Копируем JSON
    mov esi, [esp + 8 + 4*5]    ; json_data
    lea edi, [temp_record + json_data_offset]
    mov ecx, [esp + 12 + 4*5]   ; json_size
    rep movsb

    ; Размер записи
    mov eax, [esp + 12 + 4*5]
    add eax, DBRecordHeaderSize
    mov dword [db_header + DBHeader.free_offset], edx
    add dword [db_header + DBHeader.free_offset], eax

    ; Записываем на диск
    sys_lseek [db_handle], edx, SEEK_SET
    sys_write [db_handle], temp_record, eax
    mov [bytes_written], eax

    ; Хеш имени
    push dword [esp + 4 + 4*5]
    call calc_hash
    add esp, 4
    mov dword [temp_index + DBIndex.name_hash], eax

    mov eax, [db_header + DBHeader.total_records]
    mov dword [temp_index + DBIndex.id], eax
    mov dword [temp_index + DBIndex.name_offset], 0
    mov dword [temp_index + DBIndex.data_offset], ebx
    mov eax, [esp + 12 + 4*5]
    mov dword [temp_index + DBIndex.data_size], eax
    mov dword [temp_index + DBIndex.flags], 0
    mov dword [temp_index + DBIndex.created], 0
    mov dword [temp_index + DBIndex.modified], 0

    ; Пишем индекс
    mov eax, [db_header + DBHeader.total_records]
    dec eax
    mov ecx, sizeof.DBIndex
    mul ecx
    add eax, sizeof.DBHeader
    sys_lseek [db_handle], eax, SEEK_SET
    sys_write [db_handle], temp_index, sizeof.DBIndex

    ; Обновляем заголовок
    call db_write_header

    mov eax, [db_header + DBHeader.total_records]
    jmp .done

.exists:
    xor eax, eax

.done:
    pop edi esi edx ecx ebx
    ret

; ----- ПОИСК ПО ИМЕНИ (прямое сравнение) -----
; Вход: стек = [name_ptr]
; Выход: EAX = data_offset или 0
db_find_by_name:
    push ebx ecx esi edi

    mov edx, [esp + 4 + 4*4]

    sys_lseek [db_handle], sizeof.DBHeader, SEEK_SET

    mov ecx, [db_header + DBHeader.total_records]
    test ecx, ecx
    jz .not_found

    ; Читаем все индексы
    lea edi, [index_buffer]
    mov eax, sizeof.DBIndex
    mul ecx
    sys_read [db_handle], index_buffer, eax

    mov ecx, [db_header + DBHeader.total_records]
    lea edi, [index_buffer]
.search_loop:
    test ecx, ecx
    jz .not_found

    mov eax, [edi + DBIndex.flags]
    test eax, 1
    jnz .skip_record

    mov eax, [edi + DBIndex.data_offset]
    test eax, eax
    jz .skip_record

    ; Читаем имя из data_offset + 4
    push ecx edi edx
    add eax, 4
    sys_lseek [db_handle], eax, SEEK_SET
    sys_read [db_handle], read_buffer, MAX_NAME_LEN
    pop edx edi ecx

    ; Сравниваем
    push ecx edi edx esi
    lea esi, [read_buffer]
    mov edi, edx
.cmp_loop:
    lodsb
    movzx ebx, byte [edi]
    inc edi
    cmp al, bl
    jne .not_match
    test al, al
    jz .match_found
    jmp .cmp_loop

.not_match:
    pop esi edx edi ecx

.skip_record:
    add edi, sizeof.DBIndex
    dec ecx
    jmp .search_loop

.match_found:
    pop esi edx edi ecx
    mov eax, [edi + DBIndex.data_offset]
    jmp .done

.not_found:
    xor eax, eax

.done:
    pop edi esi ecx ebx
    ret

; ----- ЧТЕНИЕ ЗАПИСИ ПО ID -----
; Вход: стек = [ID]
; Выход: ESI = буфер, ECX = размер
db_read_record:
    push ebx edi

    mov ebx, [esp + 12]

    sys_lseek [db_handle], sizeof.DBHeader, SEEK_SET

    mov ecx, [db_header + DBHeader.total_records]
    test ecx, ecx
    jz .not_found

    lea edi, [index_buffer]
    mov eax, sizeof.DBIndex
    mul ecx
    sys_read [db_handle], index_buffer, eax

    mov ecx, [db_header + DBHeader.total_records]
    lea edi, [index_buffer]
.search_id:
    test ecx, ecx
    jz .not_found

    mov eax, [edi + DBIndex.id]
    cmp eax, ebx
    je .found

    add edi, sizeof.DBIndex
    dec ecx
    jmp .search_id

.found:
    mov eax, [edi + DBIndex.data_offset]
    mov ecx, [edi + DBIndex.data_size]

    push ecx
    add eax, DBRecordHeaderSize
    sys_lseek [db_handle], eax, SEEK_SET
    pop ecx
    sys_read [db_handle], read_buffer, ecx
    mov [bytes_read], eax

    lea esi, [read_buffer]
    mov ecx, [bytes_read]
    jmp .done

.not_found:
    xor esi, esi
    xor ecx, ecx

.done:
    pop edi ebx
    ret

; ----- УДАЛЕНИЕ ЗАПИСИ -----
; Вход: EAX = ID
db_delete_record:
    push ebx ecx edx esi edi

    mov ebx, eax

    sys_lseek [db_handle], sizeof.DBHeader, SEEK_SET

    mov ecx, [db_header + DBHeader.total_records]
    test ecx, ecx
    jz .done

    lea edi, [index_buffer]
    mov eax, sizeof.DBIndex
    mul ecx
    sys_read [db_handle], index_buffer, eax

    mov ecx, [db_header + DBHeader.total_records]
    lea edi, [index_buffer]
.search:
    test ecx, ecx
    jz .done

    mov eax, [edi + DBIndex.id]
    cmp eax, ebx
    je .found

    add edi, sizeof.DBIndex
    dec ecx
    jmp .search

.found:
    ; Устанавливаем флаг удаления (бит 0)
    mov eax, [edi + DBIndex.flags]
    or eax, 1
    mov dword [edi + DBIndex.flags], eax

    ; Пишем индекс обратно
    mov eax, edi
    sub eax, index_buffer
    add eax, sizeof.DBHeader
    sys_lseek [db_handle], eax, SEEK_SET
    sys_write [db_handle], edi, sizeof.DBIndex

.done:
    pop edi esi edx ecx ebx
    ret

; ----- ХЕШ djb2 -----
; Вход: стек = [str_ptr]
; Выход: EAX = 32-bit hash
calc_hash:
    push ebx ecx edx esi

    mov esi, [esp + 4 + 4*4]
    mov eax, 5381
    xor ecx, ecx

.hash_loop:
    movzx ecx, byte [esi]
    test cl, cl
    jz .hash_done

    mov edx, eax
    shl eax, 5
    add eax, edx
    add eax, ecx
    inc esi
    jmp .hash_loop

.hash_done:
    pop esi edx ecx ebx
    ret

; ----- ЧТЕНИЕ JSON ПО СМЕЩЕНИЮ -----
; Вход: EAX = data_offset
; Выход: ESI = буфер, ECX = размер
db_read_record_data:
    push ebx edx edi

    mov ebx, eax
    add ebx, DBRecordHeaderSize
    sys_lseek [db_handle], ebx, SEEK_SET
    sys_read [db_handle], read_buffer, 256
    mov [bytes_read], eax

    lea esi, [read_buffer]
    mov ecx, [bytes_read]

    pop edi edx ebx
    ret

; ----- ЗАКРЫТИЕ БД -----
db_close:
    sys_close [db_handle]
    ret
