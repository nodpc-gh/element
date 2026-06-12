; ============================================================
; Element IDE - Linux Data (data.inc)
; Буферы и константы для Linux-версии
; ============================================================

; ----- Сокетные переменные -----
server_s     dd ?
client_s     dd ?
file_fd      dd ?
bytes_read   dd ?
bytes_written dd ?

; ----- Адрес сервера -----
serv_addr:
.sin_family  dw 0
.sin_port    dw 0
.sin_addr    dd 0
.sin_zero    db 0, 0, 0, 0, 0, 0, 0, 0

; ----- Буферы -----
recv_buf     rb 4096
path_buf     rb 260
file_buf     rb 65536

; ----- HTTP-заголовки -----
h_200        db 'HTTP/1.1 200 OK', 13, 10
h_200_len    = $ - h_200

h_400        db 'HTTP/1.1 400 Bad Request', 13, 10, 13, 10
h_400_len    = $ - h_400

h_404        db 'HTTP/1.1 404 Not Found', 13, 10, 13, 10, 'Not Found', 0
h_404_len    = 24

h_401        db 'HTTP/1.1 401 Unauthorized', 13, 10
             db 'Content-Type: application/json', 13, 10
             db 'Connection: close', 13, 10, 13, 10
             db '{"error":"unauthorized"}', 0
h_401_len    = $ - h_401

; ----- MIME-типы -----
mime_html    db 'Content-Type: text/html; charset=utf-8', 13, 10
mime_html_len = $ - mime_html

mime_css     db 'Content-Type: text/css', 13, 10
mime_css_len = $ - mime_css

mime_js      db 'Content-Type: application/javascript', 13, 10
mime_js_len  = $ - mime_js

mime_png     db 'Content-Type: image/png', 13, 10
mime_png_len = $ - mime_png

mime_jpg     db 'Content-Type: image/jpeg', 13, 10
mime_jpg_len = $ - mime_jpg

mime_ico     db 'Content-Type: image/x-icon', 13, 10
mime_ico_len = $ - mime_ico

mime_json    db 'Content-Type: application/json; charset=utf-8', 13, 10
mime_json_len = $ - mime_json

; ----- Общие заголовки -----
h_ct_end     db 13, 10
h_ct_end_len = $ - h_ct_end

h_end        db 'Connection: close', 13, 10, 13, 10
h_end_len    = $ - h_end

h_cl_prefix  db 'Content-Length: '
h_cl_prefix_l = $ - h_cl_prefix

h_cl_buffer  rb 16

; ----- Ответ для POST -----
h_post_ok    db 'HTTP/1.1 200 OK', 13, 10
             db 'Content-Type: application/json; charset=utf-8', 13, 10
             db 'Content-Length: 8', 13, 10
             db 'Connection: close', 13, 10, 13, 10
             db '{"id":1}', 0
h_post_ok_len = $ - h_post_ok

; ----- Путь к БД -----
db_path      rb 260

; ----- Сообщения об ошибках -----
bind_error_msg db 'Failed to bind to port 8080!', 0

; (db.inc подключается из main_server.asm)
