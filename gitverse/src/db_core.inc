; ============================================================
; Element IDE - Core Database Engine (db_core.inc)
; Базовые операции с файловой БД (рекорды + индекс)
; ============================================================

; ----- ИНИЦИАЛИЗАЦИЯ БД -----
; Вход: EDI = путь к файлу БД
; Выход: EAX = 0 (OK) или код ошибки
proc db_init, path_ptr
    push ebx ecx edx esi edi ebp
    
    ; Копируем путь
    mov esi, [path_ptr]
    lea edi, [db_path]
.copy_path:
    lodsb
    stosb
    test al, al
    jnz .copy_path
    
    ; Пытаемся открыть существующий файл
    invoke CreateFile, db_path, GENERIC_READ or GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0
    cmp eax, INVALID_HANDLE_VALUE
    je .create_new
    
    ; Файл существует
    mov [db_handle], eax
    call db_read_header
    
    ; Проверяем сигнатуру
    lea esi, [DB_SIGNATURE]
    lea edi, [db_header.signature]
    mov ecx, 7
    repe cmpsb
    jne .invalid_db
    
    xor eax, eax  ; OK
    jmp .done
    
.create_new:
    ; Создаём новую БД
    invoke CreateFile, db_path, GENERIC_READ or GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0
    cmp eax, INVALID_HANDLE_VALUE
    je .error_open
    
    mov [db_handle], eax
    
    ; Инициализируем заголовок
    mov edi, db_header.signature
    mov esi, DB_SIGNATURE
    mov ecx, 7
    rep movsb
    
    mov [db_header.version], DB_VERSION
    mov [db_header.total_records], 0
    mov [db_header.free_offset], sizeof.DBHeader + sizeof.DBIndex * MAX_PROJECTS
    
    ; Записываем заголовок
    invoke SetFilePointer, [db_handle], 0, 0, FILE_BEGIN
    invoke WriteFile, [db_handle], db_header, sizeof.DBHeader, bytes_written, 0
    
    xor eax, eax  ; OK
    jmp .done
    
.invalid_db:
    mov eax, DB_ERROR_INVALID
    jmp .done
    
.error_open:
    mov eax, DB_ERROR_OPEN
    
.done:
    pop edi esi edx ecx ebx
    ret
endp

; ----- ЧТЕНИЕ ЗАГОЛОВКА -----
proc db_read_header
    push ebx ecx edx esi edi
    
    invoke SetFilePointer, [db_handle], 0, 0, FILE_BEGIN
    invoke ReadFile, [db_handle], db_header, sizeof.DBHeader, bytes_read, 0
    
    pop edi esi edx ecx ebx
    ret
endp

; ----- ЗАПИСЬ ЗАГОЛОВКА -----
proc db_write_header
    push ebx ecx edx esi edi
    
    invoke SetFilePointer, [db_handle], 0, 0, FILE_BEGIN
    invoke WriteFile, [db_handle], db_header, sizeof.DBHeader, bytes_written, 0
    
    pop edi esi edx ecx ebx
    ret
endp

; ----- СОЗДАНИЕ ЗАПИСИ -----
; Вход: ESI = имя проекта, EDI = JSON данные, ECX = размер JSON
; Выход: EAX = ID записи или 0 (ошибка)
proc db_create_record, name, json_data, json_size
    push ebx ecx edx esi edi
    
    mov esi, [name]
    mov edi, [json_data]
    mov ecx, [json_size]
    
    ; Проверяем, нет ли уже такого имени
    push esi
    call db_find_by_name
    add esp, 4
    test eax, eax
    jnz .exists
    
    ; Создаём новую запись
    mov eax, [db_header.total_records]
    inc eax
    mov [db_header.total_records], eax
    
    ; Вычисляем смещение — сохраняем в EBX (callee-saved, переживёт invoke)
    mov ebx, [db_header.free_offset]
    mov edx, ebx
    
    ; Записываем данные в буфер
    mov dword [temp_record + DBRecord.id], eax
    mov dword [temp_record + DBRecord.name_len], 0
    
    ; Копируем имя
    mov esi, [name]
    mov edi, temp_record + DBRecord.name
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
    mov esi, [json_data]
    mov edi, temp_record + json_data_offset
    mov ecx, [json_size]
    rep movsb
    
    ; Вычисляем размер записи
    mov eax, [json_size]
    add eax, DBRecordHeaderSize
    mov [db_header.free_offset], edx
    add [db_header.free_offset], eax
    
    ; Записываем в файл
    invoke SetFilePointer, [db_handle], edx, 0, FILE_BEGIN
    invoke WriteFile, [db_handle], temp_record, eax, bytes_written, 0
    
    ; Записываем индекс
    push [name]
    call calc_hash
    add esp, 4
    mov dword [temp_index + DBIndex.name_hash], eax
    
    mov eax, [db_header.total_records]
    mov dword [temp_index + DBIndex.id], eax
    mov dword [temp_index + DBIndex.name_offset], 0
    mov dword [temp_index + DBIndex.data_offset], ebx
    mov eax, [json_size]
    mov dword [temp_index + DBIndex.data_size], eax
    mov dword [temp_index + DBIndex.flags], 0
    mov dword [temp_index + DBIndex.created], 0
    mov dword [temp_index + DBIndex.modified], 0
    
    mov eax, [db_header.total_records]
    dec eax
    mov ecx, sizeof.DBIndex
    mul ecx
    add eax, sizeof.DBHeader
    invoke SetFilePointer, [db_handle], eax, 0, FILE_BEGIN
    invoke WriteFile, [db_handle], temp_index, sizeof.DBIndex, bytes_written, 0
    
    ; Обновляем заголовок
    call db_write_header
    
    ; Возвращаем ID
    mov eax, [db_header.total_records]
    jmp .done
    
.exists:
    mov eax, 0  ; Уже существует
    
.done:
    pop edi esi edx ecx ebx
    ret
endp

; ----- ПОИСК ПО ИМЕНИ -----
; Вход: ESI = имя для поиска => изменил на поиск по стэку
; Выход: EAX = смещение данных или 0 (не найдено)
proc db_find_by_name, name
    push ebx ecx esi edi
    
    mov edx, [name]  ; EDX = искомое имя (сторка)
    
    invoke SetFilePointer, [db_handle], sizeof.DBHeader, 0, FILE_BEGIN
    
    mov ecx, [db_header.total_records]
    test ecx, ecx
    jz .not_found
    
    ; Читаем все индексы
    mov edi, index_buffer
    mov eax, sizeof.DBIndex
    mul ecx
    invoke ReadFile, [db_handle], index_buffer, eax, bytes_read, 0
    
    ; Перебираем записи
    mov ecx, [db_header.total_records]
    mov edi, index_buffer
.search_loop:
    test ecx, ecx
    jz .not_found
    
    ; Пропускаем удалённые
    mov eax, [edi + DBIndex.flags]
    test eax, 1
    jnz .skip_record
    
    ; Смещение данных записи
    mov eax, [edi + DBIndex.data_offset]
    test eax, eax
    jz .skip_record
    
    ; Читаем имя из data_offset + 4 (пропускаем DBRecord.id)
    push ecx edi edx
    add eax, 4
    invoke SetFilePointer, [db_handle], eax, 0, FILE_BEGIN
    invoke ReadFile, [db_handle], read_buffer, MAX_NAME_LEN, bytes_read, 0
    pop edx edi ecx
    
    ; Сравниваем read_buffer с [edx]
    push ecx edi edx esi
    mov esi, read_buffer
    mov edi, edx
.cmp_loop:
    lodsb
    movzx ebx, byte [edi]
    inc edi
    cmp al, bl
    jne .not_match
    test al, al
    jz .match_found  ; обе строки завершились null
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
endp

; ----- ЧТЕНИЕ ЗАПИСИ ПО ID -----
; Вход: стек = [ID]
; Выход: ESI = буфер с данными, ECX = размер
; Примечание: НЕ proc — возвращает значения в ESI/ECX
db_read_record:
    push ebx edi
    
    mov ebx, [esp + 12]  ; ID (после push ebx[4],edi[4],ret[4] = 12)
    
    invoke SetFilePointer, [db_handle], sizeof.DBHeader, 0, FILE_BEGIN
    
    mov ecx, [db_header.total_records]
    test ecx, ecx
    jz .not_found
    
    mov edi, index_buffer
    mov eax, sizeof.DBIndex
    mul ecx
    invoke ReadFile, [db_handle], index_buffer, eax, bytes_read, 0
    
    mov ecx, [db_header.total_records]
    mov edi, index_buffer
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
    
    push ecx                        ; сохраняем data_size
    add eax, DBRecordHeaderSize     ; пропускаем заголовок записи
    invoke SetFilePointer, [db_handle], eax, 0, FILE_BEGIN
    pop ecx                          ; восстанавливаем data_size
    invoke ReadFile, [db_handle], read_buffer, ecx, bytes_read, 0
    
    mov esi, read_buffer
    mov ecx, [bytes_read]
    jmp .done
    
.not_found:
    xor esi, esi
    xor ecx, ecx
    
.done:
    pop edi ebx
    ret  ; caller cleans up with add esp, 4

; ----- УДАЛЕНИЕ ЗАПИСИ -----
; Вход: EAX = ID записи
; Выход: EAX = 1 (OK) или 0 (ошибка)
proc db_delete_record, id
    push ebx ecx edx esi edi
    
    mov ebx, [id]
    
    ; Ищем запись по ID
    mov ecx, [db_header.total_records]
    mov edi, index_buffer
    mov eax, sizeof.DBIndex
    mul ecx
    invoke ReadFile, [db_handle], index_buffer, eax, bytes_read, 0
    
    ; Ищем и помечаем как удалённое
    mov ecx, [db_header.total_records]
    mov edi, index_buffer
.search_del:
    test ecx, ecx
    jz .not_found
    
    mov eax, [edi + DBIndex.id]
    cmp eax, ebx
    je .mark_deleted
    
    add edi, sizeof.DBIndex
    dec ecx
    jmp .search_del
    
.mark_deleted:
    ; Устанавливаем бит 0 во флагах
    mov eax, [edi + DBIndex.flags]
    or eax, 1
    mov [edi + DBIndex.flags], eax
    
    ; Записываем обновлённый индекс
    invoke SetFilePointer, [db_handle], sizeof.DBHeader, 0, FILE_BEGIN
    mov eax, sizeof.DBIndex
    mul ecx
    add eax, [db_header.total_records]
    dec eax
    mul sizeof.DBIndex
    invoke WriteFile, [db_handle], index_buffer, eax, bytes_written, 0
    
    mov eax, 1  ; OK
    jmp .done
    
.not_found:
    xor eax, eax
    
.done:
    pop edi esi edx ecx ebx
    ret
endp

; ----- ВЫЧИСЛЕНИЕ ХЕША -----
; Вход: стек = [str_ptr]
; Выход: EAX = 32-бит хеш (djb2)
proc calc_hash, str
    push ebx ecx edx esi
    
    mov esi, [str]
    mov eax, 5381         ; djb2 initial hash
    xor ecx, ecx
    
.hash_loop:
    movzx ecx, byte [esi]
    test cl, cl
    jz .hash_done
    
    ; eax = eax * 33 + ecx
    mov edx, eax
    shl eax, 5            ; eax * 32
    add eax, edx          ; eax * 33
    add eax, ecx          ; + char
    inc esi
    jmp .hash_loop
    
.hash_done:
    pop esi edx ecx ebx
    ret
endp

; ----- ЧТЕНИЕ JSON ДАННЫХ ЗАПИСИ ПО СМЕЩЕНИЮ -----
; Вход: EAX = data_offset (из db_find_by_name)
; Выход: ESI = буфер с json_data, ECX = размер данных (0 если ошибка)
proc db_read_record_data, offset
    push ebx edx edi

    mov ebx, [offset]

    ; Читаем json_data: пропускаем DBRecord (id 4 + name 64 + name_len 4 = DBRecordHeaderSize)
    add ebx, DBRecordHeaderSize
    invoke SetFilePointer, [db_handle], ebx, 0, FILE_BEGIN
    invoke ReadFile, [db_handle], read_buffer, 256, bytes_read, 0

    mov esi, read_buffer
    mov ecx, [bytes_read]

    pop edi edx ebx
    ret
endp

; ----- ЗАКРЫТИЕ БД -----
proc db_close
    invoke CloseHandle, [db_handle]
    ret
endp
