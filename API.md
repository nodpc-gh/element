# Element IDE API Documentation

## REST API Endpoints

Сервер работает на порту **8080**. Все запросы — `Connection: close`.

### GET /
Главная страница IDE (index.html).

**Response:** `200 OK`, `Content-Type: text/html`

### GET /{path}
Статические файлы из `output/www/` (index.html, style.css, app.js, ...).

**Response:** `200 OK`, соответствующий MIME-тип (html, css, js, png, ico, json)

**Errors:** `404 Not Found`

---

### GET /api/elements
Список всех элементов палитры (14 элементов: System, Network, Logic, Controls).

**Response:** `200 OK`, `Content-Type: application/json`
```json
{
  "elements": [
    {"id":"console","name":"Console","category":"system","icon":"C","type":"console"},
    {"id":"btn","name":"Button","category":"controls","icon":"B","type":"btn"},
    {"id":"and_gate","name":"AND","category":"logic","icon":"A","type":"and_gate"}
  ]
}
```

---

### GET /api/projects
Список всех сохранённых проектов.

**Response:** `200 OK`, `Content-Type: application/json`
```json
{
  "total": 2,
  "projects": [
    {"id": 1, "name": "Proj_1"},
    {"id": 2, "name": "Proj_2"}
  ]
}
```

---

### GET /api/projects/{id}
Получить проект по ID.

**Response:** `200 OK`, `Content-Type: application/json`
```json
{"html":"proj1"}
```

**Errors:** `404 Not Found` — если ID не найден

### GET /api/prj/{id}
Alias для `/api/projects/{id}`.

---

### POST /api/save-project
Создать новый проект. Тело запроса — JSON, сохраняется как данные проекта.

**Request:** `Content-Type: application/json`
```json
{"html":"proj1"}
```

**Response:** `201 Created`, `Content-Type: application/json`
```json
{"id": 1}
```

### POST /api/projects
Alias для `/api/save-project`.

---

## Error Responses

| Status | Тело |
|--------|------|
| `400 Bad Request` | (пустое тело) |
| `401 Unauthorized` | `{"error":"unauthorized"}` |
| `404 Not Found` | `Not Found` |

---

## JSON Project Format

### Структура
```json
{
  "version": "2.0",
  "elements": [...],
  "connections": [...]
}
```

### Element Object
```json
{
  "id": "el_1",
  "type": "btn",
  "caption": "Button",
  "x": 100,
  "y": 150,
  "ev": 0,
  "code": "; Element code...",
  "ports": [
    {"pos": "pl", "type": "in", "id": "port_0"},
    {"pos": "pr", "type": "out", "id": "port_1"}
  ]
}
```

**Fields:**
- `id` — уникальный ID (el_1, el_2, ...)
- `type` — тип элемента (btn, lbl, edt, chk, pnl, console, ...)
- `caption` — отображаемое имя
- `x`, `y` — координаты на канвасе
- `ev` — счётчик событий
- `code` — FASM-код элемента (опционально)
- `ports` — массив портов

### Port Object
```json
{"pos": "pl", "type": "in", "id": "port_0"}
```

**Fields:**
- `pos` — сторона: pl (left), pr (right), pt (top), pb (bottom)
- `type` — тип: in, out, id, data
- `id` — уникальный ID порта

### Connection Object
```json
{
  "from": "port_0",
  "to": "port_1",
  "type": "event"
}
```

**Fields:**
- `from` — ID порта-источника
- `to` — ID порта-назначения
- `type` — тип соединения: data, event, id

### Connection Types

| Type | Color | Description |
|------|-------|-------------|
| `data` | 🟢 Green (#0a8) | Data flow |
| `event` | 🟣 Purple (#94f) | Event triggers |
| `id` | 🟠 Orange (#f60) | Identification (dashed) |

---

## Server Commands

### Start (dev)
```batch
run
```

### Start (public)
```batch
run_pub
```

### Stop
```batch
stop
```

### Status
```batch
status
```

---

**Version:** 2.0.0  
**Last Updated:** 2026-06-09
