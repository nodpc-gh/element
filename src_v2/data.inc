section '.data' readable writeable

    ; Константы сокетов
    INVALID_SOCKET = -1
    SOCKET_ERROR   = -1

    ; Сокеты и системные переменные
    wsadata     WSADATA
    server_s    dd ?
    client_s    dd ?
    fhandle     dd ?
    bytes_read  dd ?

    serv_addr   sockaddr_in

    ; Буферы
    recv_buf    rb 4096
    path_buf    rb 260
    file_buf    rb 65536

    ; Заголовки HTTP
    h_200       db 'HTTP/1.1 200 OK', 13, 10, 'Content-Type: '
    h_200_len   = $ - h_200
    
    h_ct_end    db 13, 10  ; Завершение Content-Type
    h_ct_end_len = $ - h_ct_end

    h_404       db 'HTTP/1.1 404 Not Found', 13, 10, 13, 10, 'Not Found', 0
    h_404_len   = 24

    ; Конец заголовков
    h_end       db 'Connection: close', 13, 10, 13, 10
    h_end_len   = $ - h_end

    ; MIME-типы (фиксированная длина для простоты)
    mime_html   db 'text/html; charset=utf-8', 0
    mime_html_len = $ - mime_html - 1
    mime_css    db 'text/css; charset=utf-8', 0
    mime_css_len = $ - mime_css - 1
    mime_js     db 'application/javascript; charset=utf-8', 0
    mime_js_len = $ - mime_js - 1
    mime_png    db 'image/png', 0
    mime_png_len = $ - mime_png - 1
    mime_jpg    db 'image/jpeg', 0
    mime_jpg_len = $ - mime_jpg - 1
    mime_ico    db 'image/x-icon', 0
    mime_ico_len = $ - mime_ico - 1
    mime_json   db 'application/json; charset=utf-8', 0
    mime_json_len = $ - mime_json - 1
    
    ; Для POST запроса
    msg_post    db 13, 10, '--- Received POST request ---', 13, 10, 0
    msg_post_len = $ - msg_post

    resp_json   db '{"status":"ok","message":"data received"}'
    resp_len    = $ - resp_json

    h_cl_prefix db 'Content-Length: '
    h_cl_prefix_l = $ - h_cl_prefix
    h_cl_buffer rb 16

    msg_body_label   db 13, 10, 'Body content: ', 0
    msg_body_label_l = $ - msg_body_label - 1

    ; Корневая папка www
    www_root    db 'www\', 0

    ; URL для открытия браузера
    browser_url db 'http://localhost:8080', 0
    open_cmd    db 'open', 0
    
    ; Для API проектов
    api_projects_list db '{"projects":[]}'
    
    ; Буфер для JSON ответов
    resp_buf    rb 4096
    
    ; Переменные для БД
    include 'db.inc'
