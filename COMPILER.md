# Компилятор проектов Element IDE

**План реализации**

---

## 🎯 Концепция

```
Project.json (JSON с элементами и соединениями)
    ↓
Compiler (читает JSON)
    ↓
┌─────────────────────────────────────┐
│ main.asm (главный файл)             │
│ - Console element (начальный)       │
│ - include 'elements/el_2.inc'       │
│ - include 'elements/el_3.inc'       │
└─────────────────────────────────────┘
    ↓
elements/el_2.inc (HTTP Server)
elements/el_3.inc (Button)
    ↓
FASM Compiler (fasm main.asm output.exe)
    ↓
output.exe
```

---

## 📋 Структура JSON проекта

```json
{
  "version": "2.0",
  "name": "MyConsoleApp",
  "main_element": "el_1",
  "elements": [
    {
      "id": "el_1",
      "type": "console",
      "caption": "Console",
      "x": 100,
      "y": 100,
      "code": "; Console code...",
      "ports": [...]
    }
  ],
  "connections": [
    {
      "from": "el_1_port_pr_0",
      "to": "el_2_port_pl_0",
      "type": "event"
    }
  ]
}
```

---

## 🔧 Этапы компиляции

### 1. Чтение JSON проекта

```javascript
var project = JSON.parse(projectJson);
var mainElement = project.elements.find(el => el.id === project.main_element);
var otherElements = project.elements.filter(el => el.id !== project.main_element);
```

### 2. Генерация main.asm

```javascript
var mainAsm = '; Project: ' + project.name + '\n';
mainAsm += 'format PE console 4.0\n';
mainAsm += 'entry start\n\n';
mainAsm += 'include \'win32a.inc\'\n\n';

// Инклуды элементов
otherElements.forEach(el => {
    mainAsm += 'include \'elements\\' + el.id + '.inc\'\n';
});

mainAsm += '\nsection \'.code\' readable executable\n\n';
mainAsm += 'start:\n';

// Код главного элемента
mainAsm += mainElement.code + '\n';
```

### 3. Генерация inc файлов

```javascript
otherElements.forEach(el => {
    var incCode = '; Element: ' + el.caption + '\n';
    incCode += '; ID: ' + el.id + '\n\n';
    incCode += el.code;
    
    saveFile('elements/' + el.id + '.inc', incCode);
});
```

### 4. Обработка соединений

```javascript
project.connections.forEach(conn => {
    var fromEl = getElementByPort(conn.from);
    var toEl = getElementByPort(conn.to);
    
    if (conn.type === 'event') {
        // Вызов to из from
        var callCode = '    call ' + toEl.code_function_name + '\n';
        insertCode(fromEl.code, callCode);
    }
});
```

### 5. Компиляция

```batch
fasm main.asm output.exe
```

---

## 📝 Пример

### Проект: Console + HTTP Server

**JSON:**
```json
{
  "name": "WebServerApp",
  "main_element": "el_1",
  "elements": [
    {"id": "el_1", "type": "console"},
    {"id": "el_2", "type": "http_server"}
  ],
  "connections": [
    {"from": "el_1_port_pr_0", "to": "el_2_port_pl_0", "type": "event"}
  ]
}
```

**main.asm:**
```asm
; Project: WebServerApp
format PE console 4.0
entry start

include 'win32a.inc'
include 'elements\el_2.inc'

section '.code' readable executable

start:
    call console_Init
    call http_server_Start    ; OnInit → Start
    
.main_loop:
    call console_Read
    cmp dword [input_buffer], 'exit'
    je .exit
    call console_Write
    jmp .main_loop
    
.exit:
    call http_server_Stop
    call console_Close
    invoke ExitProcess, 0
```

**elements/el_2.inc:**
```asm
; Element: HTTP_Server
; ID: el_2

http_server_Start:
    pushad
    invoke WSAStartup, 0202h, wsadata
    invoke socket, AF_INET, SOCK_STREAM, IPPROTO_TCP
    mov [server_socket], eax
    popad
    ret

http_server_Stop:
    pushad
    invoke closesocket, [server_socket]
    invoke WSACleanup
    popad
    ret
```

---

## 🚀 Реализация

### Файлы для создания:

1. **www/compiler_fasm.js** — компилятор JSON → FASM
2. **src/compiler.asm** — серверная часть компилятора
3. **templates/console.asm** — шаблон Console
4. **templates/service.asm** — шаблон Service
5. **templates/http_server.asm** — шаблон HTTP Server

### Функции компилятора:

```javascript
CompilerFASM = {
    compile: function(project) {...},
    generateMain: function(project) {...},
    generateInc: function(element) {...},
    processConnections: function(connections) {...},
    build: function(projectName) {...}
}
```

---

**Дата:** 2026-03-03  
**Статус:** В разработке
