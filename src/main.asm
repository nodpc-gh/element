; main.asm - Element IDE Web Server v1.0

format PE console 4.0
entry start

include 'win32a.inc'

AF_INET = 2
SOCK_STREAM = 1
IPPROTO_TCP = 6
SOCKET_ERROR = -1
INVALID_SOCKET = -1

section '.data' data readable writeable

    wsadata       rb 400
    server_sock   dd ?
    client_sock   dd ?
    server_addr   rb 16
    http_buf      rb 4096
    resp_buf      rb 262144

    http_hdr      db 'HTTP/1.1 200 OK', 13, 10
                  db 'Content-Type: text/html; charset=utf-8', 13, 10
                  db 'Connection: close', 13, 10
                  db 13, 10
    http_hdr_len  = $ - http_hdr

section '.code' code readable executable

start:
    ; WSAStartup
    push wsadata
    push 0x0202
    call [WSAStartup]
    test eax, eax
    jnz .exit

    ; socket
    push IPPROTO_TCP
    push SOCK_STREAM
    push AF_INET
    call [socket]
    add esp, 12
    cmp eax, INVALID_SOCKET
    je .exit
    mov [server_sock], eax

    ; bind
    mov word [server_addr], AF_INET
    mov word [server_addr + 2], 0x901F  ; 8080
    mov dword [server_addr + 4], 0
    push 16
    lea eax, [server_addr]
    push eax
    push [server_sock]
    call [bind]
    add esp, 12
    cmp eax, SOCKET_ERROR
    je .exit

    ; listen
    push 5
    push [server_sock]
    call [listen]
    add esp, 8

.main_loop:
    ; accept
    push 0
    push 0
    push [server_sock]
    call [accept]
    add esp, 12
    cmp eax, INVALID_SOCKET
    je .main_loop
    mov [client_sock], eax

    ; recv
    push 0
    push 4096
    lea eax, [http_buf]
    push eax
    push [client_sock]
    call [recv]
    add esp, 16
    test eax, eax
    jle .close

    ; check GET
    cmp byte [http_buf], 'G'
    jne .close

    ; copy header
    mov esi, http_hdr
    mov edi, resp_buf
    mov ecx, http_hdr_len
    rep movsb

    ; copy html
    mov esi, html_content
    mov ecx, html_content_len
    rep movsb

    ; send
    push 0
    push http_hdr_len + html_content_len
    push resp_buf
    push [client_sock]
    call [send]
    add esp, 16

.close:
    push [client_sock]
    call [closesocket]
    jmp .main_loop

.exit:
    call [WSACleanup]
    push 0
    call [ExitProcess]

section '.idata' import data readable writeable

    library kernel32, 'kernel32.dll', \
          ws2_32, 'ws2_32.dll'

    import kernel32, \
           ExitProcess, 'ExitProcess'

    import ws2_32, \
           WSAStartup, 'WSAStartup', \
           WSACleanup, 'WSACleanup', \
           socket, 'socket', \
           bind, 'bind', \
           listen, 'listen', \
           accept, 'accept', \
           recv, 'recv', \
           send, 'send', \
           closesocket, 'closesocket'

include 'html.inc'
