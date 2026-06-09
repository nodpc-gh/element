section '.code' readable executable

start:
    ; Инициализация БД
    invoke GetCurrentDirectory, 260, db_path
    call element_db_init

    ; Инициализация Winsock
    invoke WSAStartup, 0202h, wsadata
    
    ; Создание сокета
    invoke socket, AF_INET, SOCK_STREAM, 0
    mov [server_s], eax
    
    ; Настройка адреса
    mov [serv_addr.sin_family], AF_INET
    invoke htons, 8080
    mov [serv_addr.sin_port], ax
    mov [serv_addr.sin_addr], 0
    
    ; Привязка к порту
    invoke bind, [server_s], serv_addr, 16
    cmp eax, 0
    je .bind_ok
    ; Ошибка привязки
    invoke MessageBox, 0, bind_error_msg, bind_error_title, MB_ICONERROR or MB_OK
    invoke ExitProcess, 1
.bind_ok:
    
    ; Начало прослушивания
    invoke listen, [server_s], 5

    ; Открываем браузер
    ; invoke ShellExecuteA, 0, open_cmd, browser_url, 0, 0, 1

main_loop:
    ; Принятие подключения
    invoke accept, [server_s], 0, 0
    mov [client_s], eax
    cmp eax, INVALID_SOCKET
    je main_loop
    
    ; Чтение запроса
    invoke recv, [client_s], recv_buf, 4096, 0
    test eax, eax
    jle close_conn

    ; Проверка метода
    cmp dword [recv_buf], 'GET '
    je .check_api

    cmp dword [recv_buf], 'POST'
    jne close_conn

    ; POST — проверяем, не API ли это
    lea esi, [recv_buf + 4]
    cmp byte [esi], ' '       ; после POST есть пробел перед путём
    jne @f
    inc esi
@@:
    cmp dword [esi], '/api'
    je handle_api
    jmp handle_post

.check_api:
    ; Проверка на API запрос
    lea esi, [recv_buf + 4]
    cmp dword [esi], '/api'
    je handle_api
    
    ; Извлечение пути запроса
    lea esi, [recv_buf + 4]  ; Пропускаем "GET "
    
    ; Пропускаем первый слэш
    cmp byte [esi], '/'
    jne close_conn
    inc esi

    ; Копируем путь в path_buf
    lea edi, [path_buf]
.copy_path:
    lodsb
    cmp al, ' '
    je .done_path
    cmp al, 0
    je .done_path
    cmp al, 13
    je .done_path
    cmp al, 10
    je .done_path
    stosb
    jmp .copy_path
.done_path:
    mov byte [edi], 0
    
    ; Если путь пустой - используем index.html
    cmp byte [path_buf], 0
    jne .build_path
    mov dword [path_buf], 'inde'
    mov dword [path_buf+4], 'x.ht'
    mov dword [path_buf+8], 'ml'
    mov byte [path_buf+10], 0

.build_path:
    ; Собираем полный путь: www\ + path
    lea edi, [file_buf]
    lea esi, [www_root]
.copy_www:
    lodsb
    test al, al
    jz .copy_path_final
    stosb
    jmp .copy_www
.copy_path_final:
    lea esi, [path_buf]
.copy_path_loop:
    lodsb
    stosb
    test al, al
    jnz .copy_path_loop

    ; Поиск MIME-типа по расширению
    ; Ищем точку в path_buf
    lea esi, [path_buf]
    mov edx, mime_html  ; По умолчанию HTML
    
.find_mime:
    lodsb
    test al, al
    jz .open_file       ; Конец строки - нет расширения
    cmp al, '.'
    jne .find_mime      ; Ищем точку
    
    ; Нашли точку, esi указывает на символ после точки
    ; Сравниваем первые 2 символа расширения
    movzx eax, byte [esi]     ; Первый символ
    movzx ebx, byte [esi+1]   ; Второй символ
    shl ebx, 8
    or eax, ebx
    
    ; Сравниваем (eax содержит 2 символа в little endian)
    cmp eax, 'js'             ; css: 'c'=0x63, 's'=0x73 → 0x7363
    je .is_js
    cmp eax, 'cs'             ; css: 'c'=0x63, 's'=0x73 → 0x7363
    je .is_css
    cmp eax, 'pg'             ; png: 'p'=0x70, 'g'=0x67 → 0x6770
    je .is_png
    cmp eax, 'jp'             ; jpg: 'j'=0x6A, 'p'=0x70 → 0x706A
    je .is_jpg
    cmp eax, 'co'             ; ico: 'c'=0x63, 'o'=0x6F → 0x6F63
    je .is_ico
    cmp eax, 'on'             ; json: 'o'=0x6F, 'n'=0x6E → 0x6E6F
    je .is_json
    jmp .open_file

.is_js:
    mov edx, mime_js
    mov ecx, mime_js_len
    jmp .send_mime
.is_css:
    mov edx, mime_css
    mov ecx, mime_css_len
    jmp .send_mime
.is_png:
    mov edx, mime_png
    mov ecx, mime_png_len
    jmp .send_mime
.is_jpg:
    mov edx, mime_jpg
    mov ecx, mime_jpg_len
    jmp .send_mime
.is_ico:
    mov edx, mime_ico
    mov ecx, mime_ico_len
    jmp .send_mime
.is_json:
    mov edx, mime_json
    mov ecx, mime_json_len
    jmp .send_mime
    
.open_file:
    ; По умолчанию HTML
    mov edx, mime_html
    mov ecx, mime_html_len
    
.send_mime:
    ; Сохраняем длину в стек
    push ecx
    push edx
    
    invoke CreateFile, file_buf, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0
    cmp eax, INVALID_HANDLE_VALUE
    je send_404
    mov [fhandle], eax

    ; Отправляем HTTP 200 OK
    invoke send, [client_s], h_200, h_200_len, 0

    ; Восстанавливаем MIME-тип и длину
    pop edx
    pop ecx
    
    ; Отправляем MIME-тип
    invoke send, [client_s], edx, ecx, 0

    ; Отправляем завершение Content-Type
    invoke send, [client_s], h_ct_end, h_ct_end_len, 0

    ; Отправляем конец заголовков
    invoke send, [client_s], h_end, h_end_len, 0

    ; Чтение и отправка файла порциями
.send_file_loop:
    invoke ReadFile, [fhandle], file_buf, 65536, bytes_read, 0
    mov eax, [bytes_read]
    test eax, eax
    jz .finish_file
    
    invoke send, [client_s], file_buf, [bytes_read], 0
    test eax, eax
    jl .finish_file
    
    jmp .send_file_loop

.finish_file:
    invoke CloseHandle, [fhandle]
    jmp close_conn

; ===== ОБРАБОТКА API ЗАПРОСОВ =====
handle_api:
    cmp dword [recv_buf], 'GET '
    je .api_get
    cmp dword [recv_buf], 'POST'
    je .api_post
    jmp send_404

.api_get:
    ; GET /api/elements
    cmp dword [esi+5], 'elem'
    jne .get_projects
    cmp byte [esi+12], 's'
    je api_get_elements
    jmp send_404
    
.get_projects:
    ; GET /api/projects
    cmp dword [esi+8], 'ject'
    jne .get_prj
    cmp byte [esi+13], ' '
    je api_get_projects
    cmp byte [esi+13], '/'
    je api_get_project
    jmp send_404
    
.get_prj:
    ; GET /api/prj/{id}
    cmp dword [esi+4], '/prj'
    je api_get_project
    
    jmp send_404

.api_post:
    ; POST /api/save-project
    cmp dword [esi+5], 'save'
    je api_save_project
    
    ; POST /api/projects (создание проекта)
    cmp dword [esi+8], 'ject'
    je api_save_project
    
    jmp send_404

; GET /api/projects - список всех проектов
api_get_projects:
    push esi edi

    mov edi, resp_buf
    mov dword [edi], '{"to'
    mov dword [edi+4], 'tal"'
    mov byte [edi+8], ':'
    add edi, 9

    push edi
    mov eax, [db_header.total_records]
    call number_to_string
    pop edi
    add edi, eax

    mov dword [edi], ',"pr'
    mov dword [edi+4], 'ojec'
    mov dword [edi+8], 'ts":'
    mov byte [edi+12], '['
    add edi, 13

    invoke SetFilePointer, [db_handle], sizeof.DBHeader, 0, FILE_BEGIN
    mov ecx, [db_header.total_records]
    test ecx, ecx
    jz .close_json

    mov eax, sizeof.DBIndex
    mul ecx
    invoke ReadFile, [db_handle], index_buffer, eax, bytes_read, 0

    xor ebx, ebx
    xor esi, esi
    mov ecx, [db_header.total_records]

.iter_proj:
    push ecx
    lea edx, [index_buffer + esi]

    mov eax, [edx + DBIndex.flags]
    test eax, 1
    jnz .skip_proj

    mov eax, [edx + DBIndex.data_offset]
    call db_read_record_name

    lea eax, [read_buffer]
    cmp dword [eax], 'proj'
    jne .skip_proj
    cmp byte [eax+7], ':'
    jne .skip_proj

    test ebx, ebx
    jz .first_proj
    mov byte [edi], ','
    inc edi
.first_proj:

    mov byte [edi], '{'
    inc edi
    mov dword [edi], '"id"'
    add edi, 4
    mov byte [edi], ':'
    inc edi

    push edi
    mov eax, [edx + DBIndex.id]
    mov edi, resp_buf + 4096
    call number_to_string
    pop edi
    push edx esi ecx
    mov esi, resp_buf + 4096
    mov ecx, eax
    rep movsb
    pop ecx esi edx

    mov dword [edi], ',"na'
    add edi, 4
    mov byte [edi], 'm'
    inc edi
    mov byte [edi], 'e'
    inc edi
    mov byte [edi], '"'
    inc edi
    mov byte [edi], ':'
    inc edi
    mov byte [edi], '"'
    inc edi

    push edx esi ecx
    lea esi, [read_buffer + 8]
.copy_name:
    lodsb
    test al, al
    jz .name_done
    stosb
    jmp .copy_name
.name_done:
    pop ecx esi edx

    mov byte [edi], '"'
    inc edi
    mov byte [edi], '}'
    inc edi

    inc ebx

.skip_proj:
    add esi, sizeof.DBIndex
    pop ecx
    dec ecx
    jnz .iter_proj

.close_json:
    mov byte [edi], ']'
    inc edi
    mov byte [edi], '}'
    inc edi

    invoke send, [client_s], h_200, h_200_len, 0
    invoke send, [client_s], mime_json, mime_json_len, 0
    invoke send, [client_s], h_ct_end, h_ct_end_len, 0
    invoke send, [client_s], h_end, h_end_len, 0
    mov ecx, edi
    sub ecx, resp_buf
    invoke send, [client_s], resp_buf, ecx, 0

    pop edi esi
    jmp close_conn

; GET /api/projects/{id} - получить проект по ID
api_get_project:
    lea esi, [recv_buf + 4]
    xor edx, edx
.find_last:
    lodsb
    cmp al, ' '
    je .found_end
    cmp al, 13
    je .found_end
    cmp al, '/'
    jne .find_last
    lea edx, [esi]
    jmp .find_last
.found_end:
    test edx, edx
    jz send_404

    mov esi, edx
    xor eax, eax
.id_loop:
    movzx ecx, byte [esi]
    cmp cl, '0'
    jb .got_id
    cmp cl, '9'
    ja .got_id
    imul eax, 10
    sub ecx, '0'
    add eax, ecx
    inc esi
    jmp .id_loop
.got_id:
    test eax, eax
    jz send_404

    push eax
    call db_read_record
    add esp, 4
    test ecx, ecx
    jz send_404

    push ecx
    invoke send, [client_s], h_200, h_200_len, 0
    invoke send, [client_s], mime_json, mime_json_len, 0
    invoke send, [client_s], h_ct_end, h_ct_end_len, 0
    invoke send, [client_s], h_cl_prefix, h_cl_prefix_l, 0
    pop ecx
    push ecx
    mov eax, ecx
    mov edi, h_cl_buffer
    call number_to_string
    invoke send, [client_s], h_cl_buffer, eax, 0
    invoke send, [client_s], h_ct_end, h_ct_end_len, 0
    invoke send, [client_s], h_end, h_end_len, 0
    pop ecx
    invoke send, [client_s], read_buffer, ecx, 0
    jmp close_conn

; ===== POST /api/save-project =====
api_save_project:
    lea esi, [recv_buf]
.find_body_save:
    cmp byte [esi], 13
    jne .next_save
    cmp dword [esi], 0A0D0A0Dh
    je .found_body_save
.next_save:
    inc esi
    cmp esi, recv_buf + 4000
    jb .find_body_save
    jmp close_conn

.found_body_save:
    add esi, 4

    mov edi, path_buf
    mov dword [edi], 'proj'
    mov dword [edi+4], 'ect:'
    add edi, 8
    mov byte [edi], 'P'
    inc edi
    mov byte [edi], 'r'
    inc edi
    mov byte [edi], 'o'
    inc edi
    mov byte [edi], 'j'
    inc edi
    mov byte [edi], '_'
    inc edi

    mov eax, [db_header.total_records]
    inc eax
    call number_to_string
    add edi, eax
    mov byte [edi], 0

    push esi
    mov edi, esi
    xor eax, eax
    or ecx, -1
    repne scasb
    not ecx
    dec ecx
    pop esi

    push ecx
    push esi
    lea eax, [path_buf]
    push eax
    call db_create_record
    add esp, 12

    test eax, eax
    jz .save_error

    mov edi, resp_buf
    mov byte [edi], '{'
    inc edi
    mov byte [edi], '"'
    inc edi
    lea esi, [json_key_id]
    mov ecx, 2
    rep movsb
    mov byte [edi], '"'
    inc edi
    mov byte [edi], ':'
    inc edi
    push eax
    call number_to_string
    add esp, 4
    add edi, eax
    mov byte [edi], '}'
    inc edi

    mov eax, edi
    sub eax, resp_buf
    mov ecx, eax
    push ecx

    invoke send, [client_s], h_200, h_200_len, 0
    invoke send, [client_s], mime_json, mime_json_len, 0
    invoke send, [client_s], h_ct_end, h_ct_end_len, 0
    invoke send, [client_s], h_cl_prefix, h_cl_prefix_l, 0
    pop ecx
    push ecx
    mov eax, ecx
    mov edi, h_cl_buffer
    call number_to_string
    invoke send, [client_s], h_cl_buffer, eax, 0
    invoke send, [client_s], h_ct_end, h_ct_end_len, 0
    invoke send, [client_s], h_end, h_end_len, 0
    pop ecx
    invoke send, [client_s], resp_buf, ecx, 0
    jmp close_conn

.save_error:
    invoke send, [client_s], h_404, 24, 0
    jmp close_conn

; ===== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ =====

; Поиск значения в JSON по ключу
; Вход: ESI = JSON строка, EDI = ключ (ASCIIZ)
; Выход: EAX = указатель на значение (строку, без кавычек) или 0
json_find_value:
    push ebx ecx edx esi edi
    mov ebx, edi         ; EBX = search key
.find_key:
    lodsb                ; читаем из JSON
    test al, al
    jz .not_found
    cmp al, '"'          ; найден ключ (начинается с ")
    jne .find_key
    mov edx, esi         ; EDX = позиция первого символа ключа в JSON
    mov edi, ebx         ; EDI = search key
.check_match:
    lodsb
    cmp al, '"'
    je .check_key_end
    movzx ecx, byte [edi]
    test cl, cl
    jz .not_match        ; ключ поиска кончился, а JSON ключ нет
    cmp al, cl
    jne .not_match
    inc edi
    jmp .check_match
.check_key_end:
    movzx ecx, byte [edi]
    test cl, cl
    jnz .not_match       ; JSON ключ кончился, а ключ поиска нет
    ; Ключ совпал, ищем :
.find_colon:
    lodsb
    cmp al, ':'
    je .skip_spaces
    cmp al, ' '
    je .find_colon
    jmp .not_found
.skip_spaces:
    lodsb
    cmp al, ' '
    je .skip_spaces
    cmp al, '"'
    jne .not_found
    mov eax, esi         ; EAX = указатель на значение (без кавычек)
    jmp .done
.not_match:
    mov esi, edx
    dec esi              ; ESI -> перед открывающей кавычкой
.skip_key_json:
    lodsb
    cmp al, '"'
    jne .skip_key_json
    jmp .find_key
.not_found:
    xor eax, eax
.done:
    pop edi esi edx ecx ebx
    ret

; Сообщение об ошибке
bind_error_msg      db 'Failed to bind to port 8080!', 0
bind_error_title    db 'Server Error', 0

; JSON ключи
json_key_id      db 'id', 0

handle_post:
    ; Поиск тела POST запроса (после \r\n\r\n)
    lea esi, [recv_buf]
.find_body:
    cmp byte [esi], 13
    jne .next_byte
    cmp dword [esi], 0A0D0A0Dh
    je .found_body
.next_byte:
    inc esi
    cmp esi, recv_buf + 2000
    jb .find_body
    jmp close_conn

.found_body:
    add esi, 4

    ; Отправляем JSON ответ
    invoke send, [client_s], h_200, h_200_len, 0
    invoke send, [client_s], mime_json, mime_json_len, 0
    
    ; Отправляем Content-Length
    invoke send, [client_s], h_cl_prefix, h_cl_prefix_l, 0
    
    ; Конвертируем длину ответа в строку
    mov eax, resp_len
    mov edi, h_cl_buffer
    call number_to_string
    
    ; Отправляем число
    invoke send, [client_s], h_cl_buffer, eax, 0
    invoke send, [client_s], h_end, h_end_len, 0
    invoke send, [client_s], resp_json, resp_len, 0
    jmp close_conn

send_400:
    invoke send, [client_s], h_400, h_400_len, 0

send_404:
    invoke send, [client_s], h_404, 24, 0

close_conn:
    invoke closesocket, [client_s]
    jmp main_loop

; Конвертация числа в строку
; Вход: EAX = число, EDI = буфер
; Выход: EAX = длина строки
number_to_string:
    push ebx ecx edx edi
    mov ebx, 10
    xor ecx, ecx
.convert_loop:
    xor edx, edx
    div ebx
    add dl, '0'
    push edx
    inc ecx
    test eax, eax
    jnz .convert_loop
    mov ebx, ecx
.copy_loop:
    pop eax
    stosb
    loop .copy_loop
    mov eax, ebx
    pop edi edx ecx ebx
    ret
