section '.code' readable executable

start:
    ; Инициализация Winsock
    invoke WSAStartup, 0202h, wsadata
    test eax, eax
    jnz .wsa_error

    ; Создание сокета
    invoke socket, AF_INET, SOCK_STREAM, 0
    cmp eax, INVALID_SOCKET
    je .socket_error
    mov [server_s], eax

    ; Настройка адреса
    mov [serv_addr.sin_family], AF_INET
    invoke htons, 8080
    mov [serv_addr.sin_port], ax
    mov [serv_addr.sin_addr], 0

    ; Привязка к порту
    invoke bind, [server_s], serv_addr, 16
    cmp eax, 0
    jne .bind_error

.bind_ok:
    ; Начало прослушивания
    invoke listen, [server_s], 5
    cmp eax, SOCKET_ERROR
    je .listen_error

    ; Вывод сообщения об успехе
    invoke GetStdHandle, STD_OUTPUT_HANDLE
    invoke WriteConsole, eax, server_started_msg, server_started_msg_len, bytes_read, 0

    jmp main_loop

.wsa_error:
    invoke GetStdHandle, STD_OUTPUT_HANDLE
    invoke WriteConsole, eax, wsa_error_msg, wsa_error_msg_len, bytes_read, 0
    invoke ExitProcess, 1

.socket_error:
    invoke GetStdHandle, STD_OUTPUT_HANDLE
    invoke WriteConsole, eax, socket_error_msg, socket_error_msg_len, bytes_read, 0
    invoke ExitProcess, 1

.bind_error:
    invoke GetStdHandle, STD_OUTPUT_HANDLE
    invoke WriteConsole, eax, bind_error_msg, bind_error_msg_len, bytes_read, 0
    invoke ExitProcess, 1

.listen_error:
    invoke GetStdHandle, STD_OUTPUT_HANDLE
    invoke WriteConsole, eax, listen_error_msg, listen_error_msg_len, bytes_read, 0
    invoke ExitProcess, 1

main_loop:
    ; Принятие подключения
    invoke accept, [server_s], 0, 0
    mov [client_s], eax
    cmp eax, INVALID_SOCKET
    je main_loop
    cmp eax, SOCKET_ERROR
    je main_loop
    
    ; Чтение запроса
    invoke recv, [client_s], recv_buf, 4096, 0
    test eax, eax
    jle close_conn

    ; Проверка метода POST
    cmp dword [recv_buf], 'POST'
    je handle_post

    ; Проверка метода GET
    cmp dword [recv_buf], 'GET '
    jne close_conn

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

    ; Проверяем API запросы к элементам
    ; path_buf содержит "api/elements" (без первого '/')
    lea esi, [path_buf]

    ; Проверяем 'api/' (4 символа)
    cmp dword [esi], 'api/'
    jne .check_empty_path

    ; Проверяем различные API endpoints
    ; /api/elements - база данных элементов
    cmp dword [esi+4], 'elem'
    jne .check_projects
    call send_elements_json
    jmp close_conn

.check_projects:
    ; /api/projects - заглушка для проектов
    cmp dword [esi+4], 'proj'
    jne .check_database
    call send_projects_json
    jmp close_conn

.check_database:
    ; /api/database - полная структура БД
    cmp dword [esi+4], 'data'
    jne .check_empty_path
    call send_database_json
    jmp close_conn

.check_empty_path:
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

send_404:
    invoke send, [client_s], h_404, 24, 0
    jmp close_conn

; ----- ОТПРАВКА JSON ЭЛЕМЕНТОВ -----
; API: GET /api/elements
proc send_elements_json
    push ebx ecx edx esi edi ebp

    ; Отправляем HTTP заголовок
    invoke send, [client_s], h_200, h_200_len, 0
    invoke send, [client_s], mime_json, mime_json_len, 0
    invoke send, [client_s], h_ct_end, h_ct_end_len, 0
    invoke send, [client_s], h_end, h_end_len, 0

    ; Отправляем JSON
    invoke send, [client_s], fallback_elements, fallback_elements_len, 0

    pop ebp edi esi edx ecx ebx
    ret
endp

close_conn:
    invoke closesocket, [client_s]
    jmp main_loop

; ----- ОТПРАВКА JSON ПРОЕКТОВ (заглушка) -----
proc send_projects_json
    push ebx ecx edx esi edi ebp

    ; Отправляем HTTP заголовок
    invoke send, [client_s], h_200, h_200_len, 0
    invoke send, [client_s], mime_json, mime_json_len, 0
    invoke send, [client_s], h_ct_end, h_ct_end_len, 0
    invoke send, [client_s], h_end, h_end_len, 0

    ; Отправляем пустой проект
    invoke send, [client_s], empty_project, empty_project_len, 0

    pop ebp edi esi edx ecx ebx
    ret
endp

; ----- ОТПРАВКА JSON БАЗЫ ДАННЫХ -----
; API: GET /api/database
proc send_database_json
    push ebx ecx edx esi edi ebp

    ; Отправляем HTTP заголовок
    invoke send, [client_s], h_200, h_200_len, 0
    invoke send, [client_s], mime_json, mime_json_len, 0
    invoke send, [client_s], h_ct_end, h_ct_end_len, 0
    invoke send, [client_s], h_end, h_end_len, 0

    ; Отправляем JSON базы данных
    invoke send, [client_s], database_json, database_json_len, 0

    pop ebp edi esi edx ecx ebx
    ret
endp

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
