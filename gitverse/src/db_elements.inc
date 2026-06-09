; ============================================================
; Element IDE - Elements Database (db_elements.inc)
; Определения элементов палитры, GET /api/elements
; ============================================================

; ===== JSON-спрайт 14 элементов =====
elements_json:
    db '{"elements":[{"id":"console","name":"Console","category":"system",'
    db '"icon":"C","type":"console"},'
    db '{"id":"service","name":"Service","category":"system","icon":"S","type":"service"},'
    db '{"id":"opencode","name":"OpenCode","category":"system","icon":"O","type":"opencode"},'
    db '{"id":"http_server","name":"HTTP Server","category":"network","icon":"H","type":"http_server"},'
    db '{"id":"tcp_server","name":"TCP Server","category":"network","icon":"T","type":"tcp_server"},'
    db '{"id":"and_gate","name":"AND","category":"logic","icon":"A","type":"and_gate"},'
    db '{"id":"or_gate","name":"OR","category":"logic","icon":"O","type":"or_gate"},'
    db '{"id":"not_gate","name":"NOT","category":"logic","icon":"N","type":"not_gate"},'
    db '{"id":"xor_gate","name":"XOR","category":"logic","icon":"X","type":"xor_gate"},'
    db '{"id":"btn","name":"Button","category":"controls","icon":"B","type":"btn"},'
    db '{"id":"lbl","name":"Label","category":"controls","icon":"L","type":"lbl"},'
    db '{"id":"edt","name":"Edit","category":"controls","icon":"E","type":"edt"},'
    db '{"id":"chk","name":"CheckBox","category":"controls","icon":"K","type":"chk"},'
    db '{"id":"pnl","name":"Panel","category":"controls","icon":"P","type":"pnl"}]}'
elements_json_end:
elements_json_len = elements_json_end - elements_json

; ===== ХЕНДЛЕР GET /api/elements =====
api_get_elements:
    invoke send, [client_s], h_200, h_200_len, 0
    invoke send, [client_s], mime_json, mime_json_len, 0
    invoke send, [client_s], h_ct_end, h_ct_end_len, 0
    invoke send, [client_s], h_cl_prefix, h_cl_prefix_l, 0
    mov eax, elements_json_len
    mov edi, h_cl_buffer
    call number_to_string
    invoke send, [client_s], h_cl_buffer, eax, 0
    invoke send, [client_s], h_ct_end, h_ct_end_len, 0
    invoke send, [client_s], h_end, h_end_len, 0
    invoke send, [client_s], elements_json, elements_json_len, 0
    jmp close_conn
