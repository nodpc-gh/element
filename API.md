# Element IDE API Documentation

## Web Server API

### Endpoints

#### GET /
Returns the IDE interface (HTML/CSS/JS)

**Response:** `text/html`

```
HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8
Connection: close

<!DOCTYPE html>...
```

## JSON Project Format

### Structure

```json
{
  "version": "1.0",
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
  "ports": [
    {"pos": "pl", "type": "in"},
    {"pos": "pr", "type": "out"}
  ]
}
```

**Fields:**
- `id` - Unique element ID (el_1, el_2, ...)
- `type` - Element type (btn, lbl, edt, chk, pnl)
- `caption` - Display name
- `x` - X coordinate on canvas
- `y` - Y coordinate on canvas
- `ev` - Event counter (statistics)
- `ports` - Array of port objects

### Port Object

```json
{
  "pos": "pl",
  "type": "in"
}
```

**Fields:**
- `pos` - Position (pl=left, pr=right, pt=top, pb=bottom)
- `type` - Port type (in, out, id, data)

### Connection Object

```json
{
  "from": "out",
  "to": "in",
  "type": "event"
}
```

**Fields:**
- `from` - Source port type
- `to` - Destination port type
- `type` - Connection type (data, event, id)

## Element Types

| Type | Description | Default Ports |
|------|-------------|---------------|
| `btn` | Button | in, out, id, data |
| `lbl` | Label | in, out, id, data |
| `edt` | Edit | in, out, id, data |
| `chk` | CheckBox | in, out, id, data |
| `pnl` | Panel | in, out, id, data |

## Port Positions

| Code | Position | Side |
|------|----------|------|
| `pl` | Left | Input |
| `pr` | Right | Output |
| `pt` | Top | Input |
| `pb` | Bottom | Output |

## Connection Types

| Type | Color | Description |
|------|-------|-------------|
| `data` | 🟢 Green (#0a8) | Data flow |
| `event` | 🟣 Purple (#94f) | Event triggers |
| `id` | 🟠 Orange (#f60) | Identification (dashed) |

## Example Project

```json
{
  "version": "1.0",
  "elements": [
    {
      "id": "el_1",
      "type": "btn",
      "caption": "Button",
      "x": 50,
      "y": 50,
      "ev": 3,
      "ports": [
        {"pos": "pl", "type": "in"},
        {"pos": "pr", "type": "out"},
        {"pos": "pt", "type": "id"},
        {"pos": "pb", "type": "data"}
      ]
    },
    {
      "id": "el_2",
      "type": "lbl",
      "caption": "Label",
      "x": 200,
      "y": 100,
      "ev": 0,
      "ports": [
        {"pos": "pl", "type": "in"},
        {"pos": "pr", "type": "out"},
        {"pos": "pt", "type": "id"},
        {"pos": "pb", "type": "data"}
      ]
    }
  ],
  "connections": [
    {
      "from": "out",
      "to": "in",
      "type": "event"
    }
  ]
}
```

## Server Commands

### Start Server
```batch
output\element.exe
```

### Check Status
```batch
netstat -ano | findstr :8080
```

### Stop Server
```batch
taskkill /F /IM element.exe
```

---

**Version:** 1.0.0  
**Last Updated:** 2026-02-28
