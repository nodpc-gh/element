; ============================================================
; Element IDE - Database Structures (db.inc)
; ============================================================

; ----- Константы -----
DB_SIGNATURE    = 'ELMNTDB'
DB_VERSION      = 1
MAX_PROJECTS    = 1000
MAX_NAME_LEN    = 64

; ----- Оффсеты заголовка БД -----
DBHeader.signature      = 0
DBHeader.version        = 7
DBHeader.total_records  = 11
DBHeader.free_offset    = 15
DBHeader.reserved       = 19
sizeof.DBHeader         = 259

; ----- Оффсеты индекса -----
DBIndex.id              = 0
DBIndex.name_hash       = 4
DBIndex.name_offset     = 8
DBIndex.data_offset     = 12
DBIndex.data_size       = 16
DBIndex.flags           = 20
DBIndex.created         = 24
DBIndex.modified        = 28
sizeof.DBIndex          = 32

; ----- Оффсеты записи (на диске) -----
DBRecord.id             = 0
DBRecord.name           = 4
DBRecord.name_len       = 68
sizeof.DBRecord         = 72

DBRecordHeaderSize = sizeof.DBRecord

; ----- Оффсеты сессии -----
DBSession.id            = 0
DBSession.session_id    = 4
DBSession.user_id       = 36
DBSession.created       = 40
DBSession.expires       = 44
sizeof.DBSession        = 48

; ----- Коды ошибок -----
ERR_OK          = 0
ERR_OPEN        = 1
ERR_READ        = 2
ERR_WRITE       = 3
ERR_NOT_FOUND   = 4
ERR_EXISTS      = 5
ERR_FULL        = 6
ERR_INVALID     = 7

; ----- Буферы БД -----
; Используем db 0 для выделения памяти (rb несовместим с ELF)
BUFFER_SIZE = 65536 + MAX_PROJECTS * sizeof.DBIndex + sizeof.DBRecord + 65536 + sizeof.DBIndex + 4 + sizeof.DBHeader

align 4
data_buffer:
db 0
times BUFFER_SIZE-1 db 0

; Алиасы через EQU (адреса, а не переменные)
read_buffer      = data_buffer
index_buffer     = data_buffer + 65536
temp_record      = data_buffer + 65536 + MAX_PROJECTS * sizeof.DBIndex
temp_index       = data_buffer + 65536 + MAX_PROJECTS * sizeof.DBIndex + sizeof.DBRecord + 65536
db_handle        = data_buffer + 65536 + MAX_PROJECTS * sizeof.DBIndex + sizeof.DBRecord + 65536 + sizeof.DBIndex
db_header        = data_buffer + 65536 + MAX_PROJECTS * sizeof.DBIndex + sizeof.DBRecord + 65536 + sizeof.DBIndex + 4

; ----- Смещение JSON в temp_record -----
json_data_offset = sizeof.DBRecord
