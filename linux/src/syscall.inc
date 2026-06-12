; ============================================================
; Element IDE - Linux Syscall Macros (syscall.inc)
; 32-bit Linux syscalls via int 0x80
; ============================================================

; ----- Syscall numbers (32-bit) -----
SYS_EXIT       = 1
SYS_READ       = 3
SYS_WRITE      = 4
SYS_OPEN       = 5
SYS_CLOSE      = 6
SYS_LSEEK      = 19
SYS_GETCWD     = 183
SYS_SOCKETCALL = 102
SYS_TIME       = 13
SYS_FORK       = 2
SYS_EXECVE     = 11
SYS_WAITPID    = 7

; ----- File flags -----
O_RDWR         = 2
O_CREAT        = 0x40
O_TRUNC        = 0x200
O_APPEND       = 0x400

; ----- File whence -----
SEEK_SET       = 0
SEEK_CUR       = 1
SEEK_END       = 2

; ----- Socket subcalls (socketcall) -----
SYS_SOCKET     = 1
SYS_BIND       = 2
SYS_CONNECT    = 3
SYS_LISTEN     = 4
SYS_ACCEPT     = 5
SYS_SEND       = 9
SYS_RECV       = 10

; ----- Socket constants -----
AF_INET        = 2
SOCK_STREAM    = 1
INADDR_ANY     = 0

; ----- STDIN/STDOUT/STDERR -----
STDIN          = 0
STDOUT         = 1
STDERR         = 2

; ----- Macros -----
macro sys_open path, flags, mode {
    mov eax, SYS_OPEN
    mov ebx, path
    mov ecx, flags
    mov edx, mode
    int 0x80
}

macro sys_read fd, buf, size {
    mov eax, SYS_READ
    mov ebx, fd
    mov ecx, buf
    mov edx, size
    int 0x80
}

macro sys_write fd, buf, size {
    mov eax, SYS_WRITE
    mov ebx, fd
    mov ecx, buf
    mov edx, size
    int 0x80
}

macro sys_close fd {
    mov eax, SYS_CLOSE
    mov ebx, fd
    int 0x80
}

macro sys_lseek fd, offset, whence {
    mov eax, SYS_LSEEK
    mov ebx, fd
    mov ecx, offset
    mov edx, whence
    int 0x80
}

macro sys_getcwd buf, size {
    mov eax, SYS_GETCWD
    mov ebx, buf
    mov ecx, size
    int 0x80
}

macro sys_exit code {
    mov eax, SYS_EXIT
    mov ebx, code
    int 0x80
}

macro sys_time buf {
    mov eax, SYS_TIME
    mov ebx, buf
    int 0x80
}

; ----- Socketcall wrapper -----
macro socketcall call_num, args_ptr {
    mov eax, SYS_SOCKETCALL
    mov ebx, call_num
    mov ecx, args_ptr
    int 0x80
}

macro sys_socket domain, type, protocol {
    push protocol
    push type
    push domain
    mov ecx, esp
    socketcall SYS_SOCKET, ecx
    add esp, 12
}

macro sys_bind sockfd, addr_ptr, addr_len {
    push addr_len
    push addr_ptr
    push sockfd
    mov ecx, esp
    socketcall SYS_BIND, ecx
    add esp, 12
}

macro sys_listen sockfd, backlog {
    push backlog
    push sockfd
    mov ecx, esp
    socketcall SYS_LISTEN, ecx
    add esp, 8
}

macro sys_accept sockfd, addr_ptr, addr_len {
    push addr_len
    push addr_ptr
    push sockfd
    mov ecx, esp
    socketcall SYS_ACCEPT, ecx
    add esp, 12
}

macro sys_send sockfd, buf, len, flags {
    push flags
    push len
    push buf
    push sockfd
    mov ecx, esp
    socketcall SYS_SEND, ecx
    add esp, 16
}

macro sys_recv sockfd, buf, len, flags {
    push flags
    push len
    push buf
    push sockfd
    mov ecx, esp
    socketcall SYS_RECV, ecx
    add esp, 16
}

; ----- htons: convert port to network byte order -----
macro sys_htons port {
    ; port in eax, result in eax
    xchg ah, al
}
