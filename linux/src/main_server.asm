format ELF executable
entry _start

segment executable readable writeable

include 'syscall.inc'
include 'db.inc'
include 'data.inc'
include 'db_core.inc'
include 'db_elements.inc'
include 'db_projects.inc'
include 'code.inc'

_start:
    ; Инициализация БД
    sys_getcwd db_path, 260
    call element_db_init

    ; Создание сокета
    sys_socket AF_INET, SOCK_STREAM, 0
    cmp eax, 0
    jl exit_server
    mov [server_s], eax

    ; Настройка адреса
    mov word [serv_addr.sin_family], AF_INET
    mov eax, 8080
    sys_htons
    mov [serv_addr.sin_port], ax
    mov dword [serv_addr.sin_addr], INADDR_ANY

    ; Привязка к порту
    sys_bind [server_s], serv_addr, 16
    cmp eax, 0
    jl exit_server

    ; Начало прослушивания
    sys_listen [server_s], 5

main_loop:
    sys_accept [server_s], 0, 0
    cmp eax, 0
    jl main_loop
    mov [client_s], eax

    ; Чтение запроса
    sys_recv [client_s], recv_buf, 4096, 0
    cmp eax, 0
    jle close_conn

    ; Проверка метода
    cmp dword [recv_buf], 'GET '
    je .check_api

    cmp dword [recv_buf], 'POST'
    jne close_conn

    ; POST
    lea esi, [recv_buf + 4]
    cmp byte [esi], ' '
    jne @f
    inc esi
@@:
    cmp dword [esi], '/api'
    je handle_api
    jmp close_conn

.check_api:
    lea esi, [recv_buf + 4]
    cmp dword [esi], '/api'
    je handle_api

    ; Статика
    lea esi, [recv_buf + 4]
    cmp byte [esi], '/'
    jne close_conn
    inc esi

    ; Пустой путь → index.html
    cmp byte [esi], ' '
    je .send_index

    lea edi, [path_buf]
    mov byte [edi], '.'   ; "./www/"
    inc edi
    mov byte [edi], '/'
    inc edi
    mov byte [edi], 'w'
    inc edi
    mov byte [edi], 'w'
    inc edi
    mov byte [edi], 'w'
    inc edi
    mov byte [edi], '/'
    inc edi

.copy_path:
    lodsb
    cmp al, ' '
    je .done_path
    cmp al, '?'
    je .done_path
    stosb
    jmp .copy_path

.done_path:
    mov byte [edi], 0
    jmp send_file

.send_index:
    lea edi, [path_buf]
    mov dword [edi], './ww'
    mov dword [edi+4], 'w/in'
    mov dword [edi+8], 'dex.'
    mov dword [edi+12], 'html'
    mov byte [edi+16], 0
    jmp send_file

close_conn:
    sys_close [client_s]
    jmp main_loop

exit_server:
    sys_exit 0
