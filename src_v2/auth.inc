; ============================================================
; Element IDE - Auth Module (auth.inc)
; Управление сессиями и API авторизации
; ============================================================

; ===== УПРАВЛЕНИЕ СЕССИЯМИ =====

; ----- ИНИЦИАЛИЗАЦИЯ СЕССИЙ -----
session_init:
    push ebx ecx edx esi edi
    mov [session_count], 0
    mov edi, session_table
    mov ecx, sizeof.DBSession * MAX_SESSIONS
    xor al, al
    rep stosb
    pop edi esi edx ecx ebx
    ret

; ----- ГЕНЕРАЦИЯ SESSION ID (HEX) -----
; Выход: EAX = указатель на session_temp_id (32 hex символа)
session_generate_id:
    push ebx ecx edx esi edi
    invoke GetTickCount
    xor eax, [session_counter]
    inc [session_counter]
    mov ebx, eax
    invoke GetTickCount
    xor eax, ebx
    ror eax, 7
    add ebx, eax
    mov edi, session_temp_id
    mov ecx, 16
.loop_byte:
    push ecx
    add eax, ebx
    ror eax, 3
    add ebx, eax
    ror ebx, 5
    mov edx, eax
    and dl, 0Fh
    cmp dl, 10
    jl .digit1
    add dl, 'a' - 10
    jmp .store1
.digit1:
    add dl, '0'
.store1:
    mov [edi], dl
    inc edi
    mov edx, ebx
    and dl, 0Fh
    cmp dl, 10
    jl .digit2
    add dl, 'a' - 10
    jmp .store2
.digit2:
    add dl, '0'
.store2:
    mov [edi], dl
    inc edi
    pop ecx
    loop .loop_byte
    mov eax, session_temp_id
    pop edi esi edx ecx ebx
    ret

; ----- СОЗДАНИЕ СЕССИИ -----
; Выход: EAX = указатель на session_temp_id (0 если ошибка)
session_create:
    push ebx ecx edx esi edi
    mov eax, [session_count]
    cmp eax, MAX_SESSIONS
    jae .full
    mov edi, session_table
    xor ebx, ebx
.find_free:
    cmp ebx, MAX_SESSIONS
    je .full
    mov eax, [edi + DBSession.id]
    test eax, eax
    jz .found_slot
    add edi, sizeof.DBSession
    inc ebx
    jmp .find_free
.found_slot:
    inc [session_count]
    mov eax, ebx
    inc eax
    mov [edi + DBSession.id], eax
    call session_generate_id
    lea esi, [session_temp_id]
    lea edi, [edi + DBSession.session_id]
    mov ecx, SESSION_ID_LEN
    rep movsb
    invoke GetTickCount
    mov [edi + 4], eax       ; DBSession.created
    add eax, SESSION_LIFETIME
    mov [edi + 8], eax       ; DBSession.expires
    mov eax, session_temp_id
    jmp .done
.full:
    xor eax, eax
.done:
    pop edi esi edx ecx ebx
    ret

; ----- ПОИСК СЕССИИ ПО ID -----
; Вход: ESI = указатель на строку session_id
; Выход: EAX = указатель на DBSession или 0
session_find:
    push ebx ecx edx esi edi
    mov esi, [esp + 4 + 20]  ; параметр (после call + 5 push'ей)
    mov edi, session_table
    xor ebx, ebx
.loop:
    cmp ebx, MAX_SESSIONS
    je .not_found
    mov eax, [edi + DBSession.id]
    test eax, eax
    jz .next
    push edi esi ecx
    lea edi, [edi + DBSession.session_id]
    mov ecx, SESSION_ID_LEN
    repe cmpsb
    pop ecx esi edi
    je .check_expiry
.next:
    add edi, sizeof.DBSession
    inc ebx
    jmp .loop
.check_expiry:
    invoke GetTickCount
    cmp eax, [edi + DBSession.expires]
    ja .expired
    mov eax, edi
    jmp .done
.expired:
    mov [edi + DBSession.id], 0
    dec [session_count]
.not_found:
    xor eax, eax
.done:
    pop edi esi edx ecx ebx
    ret 4  ; чистим параметр

; ----- УДАЛЕНИЕ СЕССИИ -----
; Вход: [ESP+4] = указатель на строку session_id
; Выход: EAX = 1 (ok) или 0 (не найдена)
session_destroy:
    push ebx ecx edx esi edi
    mov esi, [esp + 4 + 20]  ; параметр
    push esi
    call session_find
    test eax, eax
    jz .not_found
    mov [eax + DBSession.id], 0
    dec [session_count]
    mov eax, 1
    jmp .done
.not_found:
    xor eax, eax
.done:
    pop edi esi edx ecx ebx
    ret 4  ; чистим параметр

; ===== API: POST /api/auth/login =====
api_auth_login:
    lea esi, [recv_buf]
.find_body_login:
    cmp byte [esi], 13
    jne .next_login
    cmp dword [esi], 0A0D0A0Dh
    je .found_body_login
.next_login:
    inc esi
    cmp esi, recv_buf + 4000
    jb .find_body_login
    jmp send_400

.found_body_login:
    add esi, 4  ; ESI = тело JSON
    mov ebp, esi  ; сохраняем указатель на JSON

    ; Пробуем хардкодного admin
    lea edi, [username_key]
    call json_find_value
    test eax, eax
    jz .try_db_user

    mov esi, eax
    lea edi, [auth_user]
.check_user:
    movzx ecx, byte [edi]
    inc edi
    test cl, cl
    jz .user_done
    lodsb
    cmp al, cl
    jne .try_db_user
    jmp .check_user
.user_done:
    cmp byte [esi], '"'
    jne .try_db_user

    ; admin совпал — проверяем пароль
    mov esi, ebp
    lea edi, [password_key]
    call json_find_value
    test eax, eax
    jz .try_db_user

    mov esi, eax
    lea edi, [auth_pass]
.check_pass:
    movzx ecx, byte [edi]
    inc edi
    test cl, cl
    jz .pass_done
    lodsb
    cmp al, cl
    jne .try_db_user
    jmp .check_pass
.pass_done:
    cmp byte [esi], '"'
    jne .try_db_user
    jmp .login_success

.try_db_user:
    ; Пробуем пользователя из БД
    mov esi, ebp
    lea edi, [username_key]
    call json_find_value
    test eax, eax
    jz .login_fail

    ; Формируем "user:<username>" в search_name
    push eax
    mov edi, search_name
    lea esi, [user_prefix]
.copy_prefix:
    lodsb
    test al, al
    jz .copy_db_uname
    stosb
    jmp .copy_prefix
.copy_db_uname:
    pop esi
    inc esi
.copy_db_loop:
    lodsb
    cmp al, '"'
    je .db_uname_done
    test al, al
    jz .login_fail
    stosb
    jmp .copy_db_loop
.db_uname_done:
    mov byte [edi], 0

    ; Ищем пользователя в БД
    lea esi, [search_name]
    push esi
    call db_find_by_name
    add esp, 4
    test eax, eax
    jz .login_fail

    ; Читаем json_data записи
    push eax
    call db_read_record_data
    add esp, 4
    test ecx, ecx
    jz .login_fail

    ; ESI = read_buffer с паролем из БД
    push esi

    ; Парсим пароль из тела JSON запроса
    mov esi, ebp
    lea edi, [password_key]
    call json_find_value
    test eax, eax
    jz .db_fail

    ; Сравниваем введённый пароль с паролем из БД
    mov esi, eax
    pop edi
.pass_comp_loop:
    movzx ecx, byte [edi]
    inc edi
    test cl, cl
    jz .pass_comp_done
    lodsb
    cmp al, cl
    jne .login_fail
    jmp .pass_comp_loop
.pass_comp_done:
    cmp byte [esi], '"'
    jne .login_fail
    jmp .login_success

.db_fail:
    add esp, 4  ; чистим saved esi
    jmp .login_fail

.login_success:
    call session_create
    test eax, eax
    jz .login_fail2

    mov edi, resp_buf
    mov byte [edi], '{'
    inc edi
    mov byte [edi], '"'
    inc edi
    lea esi, [json_key_token]
    mov ecx, 5
    rep movsb
    mov byte [edi], '"'
    inc edi
    mov byte [edi], ':'
    inc edi
    mov byte [edi], '"'
    inc edi
    lea esi, [session_temp_id]
    mov ecx, SESSION_ID_LEN
    rep movsb
    mov byte [edi], '"'
    inc edi
    mov byte [edi], '}'
    inc edi

    mov eax, edi
    sub eax, resp_buf
    mov ecx, eax
    push ecx

    invoke send, [client_s], h_200, h_200_len, 0
    invoke send, [client_s], mime_json, mime_json_len, 0
    invoke send, [client_s], h_ct_end, h_ct_end_len, 0
    invoke send, [client_s], set_cookie_hdr, set_cookie_hdr_l, 0
    invoke send, [client_s], cookie_prefix, cookie_prefix_l, 0
    invoke send, [client_s], session_temp_id, SESSION_ID_LEN, 0
    invoke send, [client_s], cookie_path, cookie_path_l, 0
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

.login_fail:
.login_fail2:
    invoke send, [client_s], h_401, h_401_len, 0
    jmp close_conn

; ===== POST /api/auth/register =====
api_auth_register:
    lea esi, [recv_buf]
.find_body_reg:
    cmp byte [esi], 13
    jne .next_byte_reg
    cmp dword [esi], 0A0D0A0Dh
    je .found_body_reg
.next_byte_reg:
    inc esi
    cmp esi, recv_buf + 4000
    jb .find_body_reg
    jmp send_400

.found_body_reg:
    add esi, 4  ; ESI = тело JSON
    mov ebp, esi  ; сохраняем указатель на JSON

    ; Парсим username
    lea edi, [username_key]
    call json_find_value
    test eax, eax
    jz .reg_err
    push eax  ; сохраняем указатель на значение username

    ; Формируем "user:<username>" в search_name
    mov edi, search_name
    lea esi, [user_prefix]
.copy_prefix:
    lodsb
    test al, al
    jz .copy_uname
    stosb
    jmp .copy_prefix
.copy_uname:
    pop esi
    inc esi  ; пропускаем первый символ (json_find_value уже за кавычкой)
.copy_uname_loop:
    lodsb
    cmp al, '"'
    je .uname_done
    test al, al
    jz .reg_err
    stosb
    jmp .copy_uname_loop
.uname_done:
    mov byte [edi], 0

    ; Проверяем, не занято ли имя
    lea esi, [search_name]
    push esi
    call db_find_by_name
    add esp, 4
    test eax, eax
    jnz .reg_conflict

    ; Парсим password
    mov esi, ebp
    lea edi, [password_key]
    call json_find_value
    test eax, eax
    jz .reg_err

    ; Копируем пароль в read_buffer (без кавычек)
    mov esi, eax
    mov edi, read_buffer
.copy_pass_loop:
    lodsb
    cmp al, '"'
    je .pass_done
    test al, al
    jz .reg_err
    stosb
    jmp .copy_pass_loop
.pass_done:
    mov byte [edi], 0

    ; Null-terminate и вычисляем длину (включая null)
    mov byte [edi], 0
    inc edi
    mov ecx, edi
    sub ecx, read_buffer

    ; Создаём запись пользователя
    lea esi, [search_name]
    lea edi, [read_buffer]

    push ecx
    push edi
    push esi
    call db_create_record
    add esp, 12

    test eax, eax
    jz .reg_err

    ; Формируем ответ {"status":"registered","id":<id>}
    mov edi, resp_buf
    mov dword [edi], '{"st'
    add edi, 4
    mov dword [edi], 'atus'
    add edi, 4
    mov byte [edi], '"'
    inc edi
    mov byte [edi], ':'
    inc edi
    mov dword [edi], '"re'
    add edi, 3
    mov dword [edi], 'gist'
    add edi, 4
    mov dword [edi], 'ered'
    add edi, 4
    mov byte [edi], '"'
    inc edi
    mov byte [edi], ','
    inc edi
    mov dword [edi], '"id'
    add edi, 3
    mov byte [edi], '"'
    inc edi
    mov byte [edi], ':'
    inc edi

    ; Конвертируем ID записи из EAX в строку
    call number_to_string
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

.reg_conflict:
    invoke send, [client_s], h_409, h_409_len, 0
    jmp close_conn

.reg_err:
    invoke send, [client_s], h_401, h_401_len, 0
    jmp close_conn

; ===== POST /api/auth/logout =====
api_auth_logout:
    call get_session_id_from_cookie
    test eax, eax
    jz send_400
    push eax
    call session_destroy
    ; Возвращаем успех
    mov edi, resp_buf
    mov byte [edi], '{'
    mov byte [edi+1], '"'
    mov byte [edi+2], 's'
    mov byte [edi+3], 't'
    mov byte [edi+4], 'a'
    mov byte [edi+5], 't'
    mov byte [edi+6], 'u'
    mov byte [edi+7], 's'
    mov byte [edi+8], '"'
    mov byte [edi+9], ':'
    mov byte [edi+10], '"'
    mov byte [edi+11], 'o'
    mov byte [edi+12], 'k'
    mov byte [edi+13], '"'
    mov byte [edi+14], '}'
    mov ecx, 15
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

; ===== ПРОВЕРКА СЕССИИ =====
; Вход: recv_buf содержит HTTP запрос
; Выход: EAX = 1 (есть сессия), 0 (нет сессии)
check_session:
    push ebx ecx edx esi edi
    call get_session_id_from_cookie
    test eax, eax
    jz .invalid
    push eax
    call session_find
    test eax, eax
    jz .invalid
    mov eax, 1
    jmp .done
.invalid:
    xor eax, eax
.done:
    pop edi esi edx ecx ebx
    ret

; ===== ИЗВЛЕЧЕНИЕ SESSION_ID ИЗ COOKIE =====
; Вход: recv_buf
; Выход: EAX = указатель на session_id в cookie_buf, или 0
get_session_id_from_cookie:
    push ebx ecx edx esi edi
    ; Ищем "Cookie:" в заголовках
    lea esi, [recv_buf]
.find_cookie:
    cmp dword [esi], 'Cook'
    jne .next_c
    cmp byte [esi+4], 'i'
    jne .next_c
    cmp byte [esi+5], 'e'
    jne .next_c
    cmp byte [esi+6], ':'
    je .found_cookie
.next_c:
    inc esi
    cmp esi, recv_buf + 2000
    jb .find_cookie
    jmp .no_cookie

.found_cookie:
    add esi, 7  ; пропускаем "Cookie:"
    ; Пропускаем пробелы
.skip_spaces:
    cmp byte [esi], ' '
    jne .find_sid
    inc esi
    jmp .skip_spaces

.find_sid:
    ; Ищем "session_id="
    push edx
    lea edx, [recv_buf + 4000]
.find_sid_loop:
    cmp esi, edx
    jae .sid_not_found
    cmp dword [esi], 'sess'
    jne .next_sid
    cmp dword [esi+4], 'ion_'
    jne .next_sid
    cmp byte [esi+8], 'i'
    jne .next_sid
    cmp byte [esi+9], 'd'
    jne .next_sid
    cmp byte [esi+10], '='
    je .copy_sid
.next_sid:
    inc esi
    jmp .find_sid_loop
.sid_not_found:
    pop edx
    jmp .no_cookie

.copy_sid:
    pop edx  ; восстанавливаем EDX
    add esi, 11  ; пропускаем "session_id="
    mov edi, cookie_buf
    mov ecx, SESSION_ID_LEN
.copy_loop:
    lodsb
    cmp al, ';'
    je .end_sid
    cmp al, 13
    je .end_sid
    cmp al, 10
    je .end_sid
    cmp al, ' '
    je .end_sid
    cmp al, 0
    je .end_sid
    stosb
    loop .copy_loop
.end_sid:
    mov byte [edi], 0
    mov eax, cookie_buf
    jmp .done

.no_cookie:
    xor eax, eax
.done:
    pop edi esi edx ecx ebx
    ret

; JSON ключи для авторизации
username_key     db 'username', 0
password_key     db 'password', 0
json_key_token   db 'token', 0
