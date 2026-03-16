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

    ; Структура sockaddr_in (16 байт)
    serv_addr:
    .sin_family dw ?
    .sin_port   dw ?
    .sin_addr   dd ?
    .sin_zero   rb 8
    
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

    ; Путь к JSON файлу элементов (относительно output/)
    elements_json_path db 'database\elements_db_simple.json', 0

    ; Запасные данные элементов (если файл не найден) - единая строка
    fallback_elements db '{"elements":[',13,10
                      db '{"id":"console","name":"Console","category":"system","icon":"C","type":"console"},',13,10
                      db '{"id":"service","name":"Service","category":"system","icon":"S","type":"service"},',13,10
                      db '{"id":"http_server","name":"HTTP Server","category":"network","icon":"H","type":"http_server"},',13,10
                      db '{"id":"tcp_server","name":"TCP Server","category":"network","icon":"T","type":"tcp_server"},',13,10
                      db '{"id":"and_gate","name":"AND","category":"logic","icon":"A","type":"and_gate"},',13,10
                      db '{"id":"or_gate","name":"OR","category":"logic","icon":"O","type":"or_gate"},',13,10
                      db '{"id":"not_gate","name":"NOT","category":"logic","icon":"N","type":"not_gate"},',13,10
                      db '{"id":"xor_gate","name":"XOR","category":"logic","icon":"X","type":"xor_gate"},',13,10
                      db '{"id":"btn","name":"Button","category":"controls","icon":"B","type":"btn"},',13,10
                      db '{"id":"lbl","name":"Label","category":"controls","icon":"L","type":"lbl"},',13,10
                      db '{"id":"edt","name":"Edit","category":"controls","icon":"E","type":"edt"},',13,10
                      db '{"id":"chk","name":"CheckBox","category":"controls","icon":"K","type":"chk"},',13,10
                      db '{"id":"pnl","name":"Panel","category":"controls","icon":"P","type":"pnl"}',13,10
                      db ']}',0
    fallback_elements_len = $ - fallback_elements - 1  ; Вычитаем нулевой байт

    ; Пустой проект (заглушка для /api/projects)
    empty_project db '{"version":"2.0","elements":[],"connections":[]}',0
    empty_project_len = $ - empty_project - 1

    ; Полная база данных элементов (для /api/database)
    database_json db '{"database":"Element IDE Components Database","version":"2.0","elements":[',13,10
                  db '{"id":"comp_console","name":"Console","category":"system","icon":"C","type":"console","defaultPorts":["pl","pr","pt","pb"]},',13,10
                  db '{"id":"comp_service","name":"Service","category":"system","icon":"S","type":"service","defaultPorts":["pl","pr","pt","pb"]},',13,10
                  db '{"id":"comp_http_server","name":"HTTP Server","category":"network","icon":"H","type":"http_server","defaultPorts":["pl","pr","pt","pb"]},',13,10
                  db '{"id":"comp_tcp_server","name":"TCP Server","category":"network","icon":"T","type":"tcp_server","defaultPorts":["pl","pr","pt","pb"]},',13,10
                  db '{"id":"comp_and","name":"AND","category":"logic","icon":"A","type":"and_gate","defaultPorts":["pl","pr","pt","pb"]},',13,10
                  db '{"id":"comp_or","name":"OR","category":"logic","icon":"O","type":"or_gate","defaultPorts":["pl","pr","pt","pb"]},',13,10
                  db '{"id":"comp_not","name":"NOT","category":"logic","icon":"N","type":"not_gate","defaultPorts":["pl","pr","pt","pb"]},',13,10
                  db '{"id":"comp_xor","name":"XOR","category":"logic","icon":"X","type":"xor_gate","defaultPorts":["pl","pr","pt","pb"]},',13,10
                  db '{"id":"comp_btn","name":"Button","category":"controls","icon":"B","type":"btn","defaultPorts":["pl","pr","pt","pb"]},',13,10
                  db '{"id":"comp_lbl","name":"Label","category":"controls","icon":"L","type":"lbl","defaultPorts":["pl","pr","pt","pb"]},',13,10
                  db '{"id":"comp_edt","name":"Edit","category":"controls","icon":"E","type":"edt","defaultPorts":["pl","pr","pt","pb"]},',13,10
                  db '{"id":"comp_chk","name":"CheckBox","category":"controls","icon":"K","type":"chk","defaultPorts":["pl","pr","pt","pb"]},',13,10
                  db '{"id":"comp_pnl","name":"Panel","category":"controls","icon":"P","type":"pnl","defaultPorts":["pl","pr","pt","pb"]}',13,10
                  db '],"categories":[',13,10
                  db '{"id":"system","name":"System","icon":"S"},',13,10
                  db '{"id":"network","name":"Network","icon":"N"},',13,10
                  db '{"id":"logic","name":"Logic","icon":"L"},',13,10
                  db '{"id":"controls","name":"Controls","icon":"C"}',13,10
                  db ']}',0
    database_json_len = $ - database_json - 1

    ; Сообщения об ошибках
    wsa_error_msg       db 'WSAStartup failed', 13, 10, 0
    wsa_error_msg_len   = $ - wsa_error_msg
    socket_error_msg    db 'Socket creation failed', 13, 10, 0
    socket_error_msg_len = $ - socket_error_msg
    bind_error_msg      db 'Failed to bind to port 8080!', 13, 10, 0
    bind_error_msg_len  = $ - bind_error_msg
    listen_error_msg    db 'Listen failed', 13, 10, 0
    listen_error_msg_len = $ - listen_error_msg
    server_started_msg  db 'Server started on port 8080...', 13, 10, 0
    server_started_msg_len = $ - server_started_msg
