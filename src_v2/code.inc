section '.code' readable executable

start:
    ; Инициализация БД
    invoke GetCurrentDirectory, 260, db_path
    mov eax, db_path
.find_end:
    mov byte [eax], 0
    cmp byte [eax], 0
    jz .found
    inc eax
    jmp .find_end
.found:
    ; Добавляем projects.db к пути
    mov esi, projects_db_path
    mov edi, eax
.copy_db_path:
    lodsb
    stosb
    test al, al
    jnz .copy_db_path
    
    ; Инициализируем БД
    push db_path
    call db_init
    
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
    invoke ShellExecuteA, 0, open_cmd, browser_url, 0, 0, 1

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
    cmp dword [esi], '/api'
    je api_save_project
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
    ; esi указывает на URL после "GET "
    ; Проверяем /api/projects (9 символов)
    cmp dword [esi+8], 'ject'
    jne .check_prj
    
    ; Смотрим, что после /projects
    cmp byte [esi+12], ' '      ; конец URL → список
    je api_get_projects
    cmp byte [esi+12], '/'      ; /api/projects/{id}
    je api_get_project
    jmp send_404
    
.check_prj:
    ; Альтернативный маршрут: /api/prj/{id}
    cmp dword [esi+4], '/prj'
    je api_get_project
    
    jmp send_404

; GET /api/projects - список всех проектов
api_get_projects:
    ; Формируем JSON ответ
    mov edi, resp_buf
    mov byte [edi+0], '{'
    mov byte [edi+1], '"'
    mov byte [edi+2], 'p'
    mov byte [edi+3], 'r'
    mov byte [edi+4], 'o'
    mov byte [edi+5], 'j'
    mov byte [edi+6], 'e'
    mov byte [edi+7], 'c'
    mov byte [edi+8], 't'
    mov byte [edi+9], 's'
    mov byte [edi+10], '"'
    mov byte [edi+11], ':'
    mov byte [edi+12], '['
    add edi, 13
    
    ; Здесь будет код для чтения списка из БД
    ; Пока возвращаем пустой список
    mov byte [edi], ']'
    inc edi
    mov byte [edi], '}'
    inc edi
    
    ; Отправляем ответ
    mov eax, edi
    sub eax, resp_buf
    mov ecx, eax
    
    invoke send, [client_s], h_200, h_200_len, 0
    invoke send, [client_s], mime_json, mime_json_len, 0
    invoke send, [client_s], h_ct_end, h_ct_end_len, 0
    invoke send, [client_s], h_end, h_end_len, 0
    invoke send, [client_s], resp_buf, ecx, 0
    jmp close_conn

; GET /api/projects/{id} - получить проект по ID
api_get_project:
    ; Извлекаем ID из пути
    ; Пока возвращаем заглушку
    jmp send_404

; POST /api/projects - сохранить проект
api_save_project:
    ; Читаем тело запроса
    ; Сохраняем в БД
    ; Возвращаем ID
    jmp send_404

; Строка пути к БД
projects_db_path db '\projects.db', 0

; Сообщение об ошибке
bind_error_msg      db 'Failed to bind to port 8080!', 0
bind_error_title    db 'Server Error', 0

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
