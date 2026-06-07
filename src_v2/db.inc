; ============================================================
; Element IDE - Database Module (db.inc)
; Файловая база данных для хранения проектов
; ============================================================

; ----- КОНСТАНТЫ -----
DB_SIGNATURE    db 'ELMNTDB'
DB_VERSION      = 1
MAX_PROJECTS    = 1000
MAX_NAME_LEN    = 64

; ----- СТРУКТУРЫ -----

; Заголовок базы данных
struct DBHeader
    signature       db 7 dup(?)      ; "ELMNTDB"
    version         dd ?
    total_records   dd ?
    free_offset     dd ?             ; следующая свободная позиция
    reserved        db 240 dup(?)    ; резерв
ends

; Индекс записи (для быстрого поиска)
struct DBIndex
    id              dd ?             ; ID записи
    name_hash       dd ?             ; хеш имени для поиска
    name_offset     dd ?             ; смещение имени
    data_offset     dd ?             ; смещение данных JSON
    data_size       dd ?             ; размер данных
    flags           dd ?             ; флаги (бит 0: удалено)
    created         dd ?             ; timestamp создания
    modified        dd ?             ; timestamp изменения
ends

; Запись проекта
struct DBRecord
    id              dd ?
    name            db MAX_NAME_LEN dup(?)
    name_len        dd ?
    json_data       db ?             ; начало JSON данных (переменная длина)
ends

; ----- ПЕРЕМЕННЫЕ -----
section '.data' readable writeable

    db_handle       dd ?             ; дескриптор файла БД
    db_header       DBHeader         ; заголовок БД
    db_path         db 260 dup(?)    ; путь к файлу БД
    
    ; Буферы
    read_buffer     rb 65536         ; буфер для чтения/записи
    search_name     db MAX_NAME_LEN dup(?)
    
    bytes_written  dd ?
    
    ; Ошибки
    DB_OK                   = 0
    DB_ERROR_OPEN           = 1
    DB_ERROR_READ           = 2
    DB_ERROR_WRITE          = 3
    DB_ERROR_NOT_FOUND      = 4
    DB_ERROR_EXISTS         = 5
    DB_ERROR_FULL           = 6
    DB_ERROR_INVALID        = 7
    
    db_error_msg:
        db 'DB Error: ', 0
    db_success_msg:
        db 'OK', 0

section '.bss' readable writeable

    temp_record       rb sizeof.DBRecord + 65536
    index_buffer      rb sizeof.DBIndex * MAX_PROJECTS
