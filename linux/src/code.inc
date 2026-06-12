; ============================================================
; Element IDE - Linux HTTP Server (code.inc)
; ============================================================

; ----- ОТПРАВКА ФАЙЛА -----
send_file:
    push esi edi

    ; Открываем файл
    sys_open path_buf, O_RDWR, 0
    cmp eax, 0
    jl .not_found
    mov [file_fd], eax

    ; Определяем MIME по расширению
    lea esi, [path_buf]
.find_ext:
    lodsb
    test al, al
    jz .got_ext
    cmp al, '.'
    jne .find_ext
    mov edx, esi
    ; edx = указатель на расширение
    jmp .check_ext

.got_ext:
    lea edx, [h_404]   ; fallback

.check_ext:
    cmp dword [edx], 'css'
    je .mime_css
    cmp dword [edx], '.js'
    je .mime_js
    cmp dword [edx], 'png'
    je .mime_png
    cmp dword [edx], 'jpg'
    je .mime_jpg
    cmp dword [edx], 'ico'
    je .mime_ico
    cmp dword [edx], 'son'
    je .mime_json

    ; HTML по умолчанию
    lea edx, [mime_html]
    mov ecx, mime_html_len
    jmp .send_mime

.mime_css:
    lea edx, [mime_css]
    mov ecx, mime_css_len
    jmp .send_mime
.mime_js:
    lea edx, [mime_js]
    mov ecx, mime_js_len
    jmp .send_mime
.mime_png:
    lea edx, [mime_png]
    mov ecx, mime_png_len
    jmp .send_mime
.mime_jpg:
    lea edx, [mime_jpg]
    mov ecx, mime_jpg_len
    jmp .send_mime
.mime_ico:
    lea edx, [mime_ico]
    mov ecx, mime_ico_len
    jmp .send_mime
.mime_json:
    lea edx, [mime_json]
    mov ecx, mime_json_len

.send_mime:
    push edx ecx

    ; Получаем размер файла
    sys_lseek [file_fd], 0, SEEK_END
    push eax
    sys_lseek [file_fd], 0, SEEK_SET

    ; Отправляем заголовки
    sys_send [client_s], h_200, h_200_len, 0
    pop ecx
    pop edx
    push ecx
    sys_send [client_s], edx, ecx, 0
    sys_send [client_s], h_ct_end, h_ct_end_len, 0
    sys_send [client_s], h_cl_prefix, h_cl_prefix_l, 0

    pop ecx
    push ecx
    mov eax, ecx
    lea edi, [h_cl_buffer]
    call number_to_string
    sys_send [client_s], h_cl_buffer, eax, 0
    sys_send [client_s], h_ct_end, h_ct_end_len, 0
    sys_send [client_s], h_end, h_end_len, 0

    ; Читаем и отправляем тело файла
    pop ecx
    cmp ecx, 65536
    jbe .read_all
    mov ecx, 65536
.read_all:
    sys_read [file_fd], file_buf, ecx
    cmp eax, 0
    jle .file_done
    sys_send [client_s], file_buf, eax, 0
    jmp .read_all

.file_done:
    sys_close [file_fd]
    pop edi esi
    jmp close_conn

.not_found:
    pop edi esi
    jmp send_404

; ===== ОБРАБОТКА API =====
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
    cmp dword [esi+8], 'ject'
    jne .get_prj
    cmp byte [esi+13], ' '
    je api_get_projects
    cmp byte [esi+13], '/'
    je api_get_project
    jmp send_404

.get_prj:
    cmp dword [esi+4], '/prj'
    je api_get_project
    jmp send_404

.api_post:
    cmp dword [esi+5], 'save'
    je api_save_project
    cmp dword [esi+8], 'ject'
    je api_save_project
    jmp send_404

; ===== GET /api/projects — список проектов =====
api_get_projects:
    push esi edi

    lea edi, [resp_buf]
    mov dword [edi], '{"to'
    mov dword [edi+4], 'tal"'
    mov byte [edi+8], ':'
    add edi, 9

    push edi
    mov eax, [db_header + DBHeader.total_records]
    call number_to_string
    pop edi
    add edi, eax

    mov dword [edi], ',"pr'
    mov dword [edi+4], 'ojec'
    mov dword [edi+8], 'ts":'
    mov byte [edi+12], '['
    add edi, 13

    sys_lseek [db_handle], sizeof.DBHeader, SEEK_SET
    mov ecx, [db_header + DBHeader.total_records]
    test ecx, ecx
    jz .close_json

    mov eax, sizeof.DBIndex
    mul ecx
    sys_read [db_handle], index_buffer, eax

    xor ebx, ebx
    xor esi, esi
    mov ecx, [db_header + DBHeader.total_records]

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
    lea edi, [resp_buf + 4096]
    call number_to_string
    pop edi
    push edx esi ecx
    lea esi, [resp_buf + 4096]
    mov ecx, eax
    rep movsb
    pop ecx esi edx

    mov dword [edi], ',"na'
    add edi, 4
    mov dword [edi+0], 'me"'
    mov byte [edi+3], ':'
    add edi, 4
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

    sys_send [client_s], h_200, h_200_len, 0
    sys_send [client_s], mime_json, mime_json_len, 0
    sys_send [client_s], h_ct_end, h_ct_end_len, 0
    sys_send [client_s], h_end, h_end_len, 0
    mov ecx, edi
    sub ecx, resp_buf
    sys_send [client_s], resp_buf, ecx, 0

    pop edi esi
    jmp close_conn

; ===== GET /api/projects/{id} — один проект =====
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
    sys_send [client_s], h_200, h_200_len, 0
    sys_send [client_s], mime_json, mime_json_len, 0
    sys_send [client_s], h_ct_end, h_ct_end_len, 0
    sys_send [client_s], h_cl_prefix, h_cl_prefix_l, 0
    pop ecx
    push ecx
    mov eax, ecx
    lea edi, [h_cl_buffer]
    call number_to_string
    sys_send [client_s], h_cl_buffer, eax, 0
    sys_send [client_s], h_ct_end, h_ct_end_len, 0
    sys_send [client_s], h_end, h_end_len, 0
    pop ecx
    sys_send [client_s], read_buffer, ecx, 0
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

    lea edi, [path_buf]
    mov dword [edi], 'proj'
    mov dword [edi+4], 'ect:'
    add edi, 8
    mov byte [edi], 'P'
    inc edi

    mov eax, [db_header + DBHeader.total_records]
    inc eax
    push edi
    lea edi, [path_buf + 64]
    call number_to_string
    pop edi
    push esi
    lea esi, [path_buf + 64]
    mov ecx, eax
    rep movsb
    pop esi
    mov byte [edi], 0

    ; Определяем размер JSON-тела
    push esi
    mov edi, esi
    xor eax, eax
    mov ecx, -1
    repne scasb
    not ecx
    dec ecx
    pop esi

    push ecx
    push ecx

    ; Сохраняем
    lea edx, [path_buf]
    push esi               ; json_data
    push edx               ; name
    call db_create_record
    add esp, 8
    pop ecx
    test eax, eax
    jz close_conn

    ; JSON-ответ {"id":N}
    lea edi, [resp_buf]
    mov dword [edi], '{"id'
    mov dword [edi+4], '":'
    add edi, 6

    push edi
    lea edi, [resp_buf + 64]
    call number_to_string
    pop edi
    push eax
    mov ecx, eax
    lea esi, [resp_buf + 64]
    rep movsb
    pop eax
    mov byte [edi], '}'
    inc edi
    mov byte [edi], 0

    mov ecx, edi
    sub ecx, resp_buf

    sys_send [client_s], h_200, h_200_len, 0
    sys_send [client_s], mime_json, mime_json_len, 0
    sys_send [client_s], h_ct_end, h_ct_end_len, 0
    sys_send [client_s], h_cl_prefix, h_cl_prefix_l, 0
    push ecx
    mov eax, ecx
    lea edi, [h_cl_buffer]
    call number_to_string
    sys_send [client_s], h_cl_buffer, eax, 0
    sys_send [client_s], h_ct_end, h_ct_end_len, 0
    sys_send [client_s], h_end, h_end_len, 0
    pop ecx
    sys_send [client_s], resp_buf, ecx, 0
    jmp close_conn

; ===== ОТПРАВКА СТАТУСНЫХ ОТВЕТОВ =====
send_400:
    sys_send [client_s], h_400, h_400_len, 0
    jmp close_conn

send_404:
    sys_send [client_s], h_404, 24, 0
    jmp close_conn

; ===== КОНВЕРТАЦИЯ ЧИСЛА В СТРОКУ =====
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

; ===== БУФЕР ДЛЯ ОТВЕТА =====
resp_buf rb 8192
