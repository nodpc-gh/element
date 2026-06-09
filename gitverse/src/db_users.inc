; ============================================================
; Element IDE - Database Init (db_element.inc)
; Инициализация единой БД (element.db)
; Все типы записей хранятся с префиксами:
;   user:    — пользователи
;   project: — проекты
;   element: — элементы палитры
; ============================================================

element_db_file db '\element.db', 0

; ----- ИНИЦИАЛИЗАЦИЯ ЕДИНОЙ БД -----
; Копирует путь сервера в db_path и добавляет element.db
; Вход: нет (db_path должен содержать путь сервера)
; Выход: EAX = 0 (OK) или код ошибки
element_db_init:
    push esi edi
    
    ; Находим конец пути в db_path (сейчас там путь к серверу)
    lea edi, [db_path]
.find_end:
    cmp byte [edi], 0
    je .append
    inc edi
    jmp .find_end
.append:
    lea esi, [element_db_file]
.copy_name:
    lodsb
    stosb
    test al, al
    jnz .copy_name
    
    ; Инициализируем core DB
    push db_path
    call db_init
    pop edi esi
    ret
