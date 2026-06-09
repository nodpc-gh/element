format PE console 4.0
entry start

include 'win32a.inc'

; Подключаем секции через инклуды
include 'data.inc'
include 'db_core.inc'     ; Ядро БД
include 'db_users.inc'    ; Пользовательская БД
include 'db_elements.inc' ; Элементы (заглушка)
include 'db_projects.inc' ; Проекты (заглушка)
include 'code.inc'        ; Сервер: роутинг, хендлеры, helpers
include 'imports.inc'
