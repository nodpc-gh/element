# Database Viewer Implementation

**Дата:** 2026-03-16  
**Версия:** Element IDE v2.0

---

## 📋 Обзор изменений

В проект Element IDE добавлен новый функционал:

1. **Просмотрщик базы данных элементов** - модальное окно с таблицей всех элементов
2. **Редактор исходного кода** - просмотр и редактирование кода элементов
3. **Система шаблонов кода** - генерация кода с точками подключения портов
4. **Новый API endpoint** - `/api/database` для получения полной структуры БД

---

## 🗄️ База данных

### Текущее состояние

База данных **уже вынесена в отдельный файл**:
- **Путь:** `database\elements_db_simple.json`
- **Формат:** JSON
- **Дублирование:** В сервере (`src\data.inc`) есть запасной вариант `fallback_elements`

### Структура БД

```json
{
  "database": "Element IDE Components Database",
  "version": "2.0",
  "elements": [
    {
      "id": "comp_console",
      "name": "Console",
      "category": "system",
      "icon": "C",
      "type": "console",
      "defaultPorts": ["pl", "pr", "pt", "pb"],
      "properties": {
        "BufferSize": "4096",
        "Encoding": "UTF-8"
      }
    }
  ],
  "categories": [
    {"id": "system", "name": "System", "icon": "S"}
  ]
}
```

### Новые API Endpoints

| Endpoint | Метод | Описание |
|----------|-------|----------|
| `/api/elements` | GET | Список элементов (упрощённый) |
| `/api/database` | GET | Полная структура БД с категориями |
| `/api/projects` | GET | Проекты (заглушка) |

---

## 🖥️ Просмотрщик базы данных

### Расположение

- **HTML:** `output\www\index.html` (модальное окно `#databaseModal`)
- **CSS:** `output\www\style.css` (стили `.db-*`)
- **JS:** `output\www\database.js`

### Как открыть

1. Нажмите кнопку **🗄️** в toolbar
2. Или через JavaScript: `window.DatabaseViewer.open()`

### Функционал

#### Вкладка "Элементы"
- Таблица всех элементов с иконками, названиями, типами
- Отображение портов (pl, pr, pt, pb)
- Поиск по элементам
- Клик на элемент → просмотр деталей
- Кнопка "📝 Код" → переход к редактору кода

#### Вкладка "Категории"
- Древовидная структура по категориям
- Счётчик элементов в категории
- Клик на элемент → просмотр кода

#### Вкладка "Структура"
- JSON представление всей базы данных
- Форматированный вывод с подсветкой

### Детали элемента

При выборе элемента показывается:
- Иконка и название
- Тип элемента
- Порты с цветовой индикацией
- Свойства элемента
- Кнопка просмотра кода

---

## 📝 Редактор исходного кода

### Расположение

- **HTML:** `output\www\index.html` (модальное окно `#sourceModal`)
- **CSS:** `output\www\style.css` (стили `.source-*`, `.hl-*`)
- **JS:** `output\www\source_editor.js`

### Как открыть

1. Нажмите кнопку **📝** в toolbar (без выбранного элемента)
2. Кнопка "✏️ Редактировать" в панели свойств
3. Кнопка "📝 Код" в просмотрщике БД
4. Клик на элемент → правая панель → "✏️ Редактировать"
5. Или через JavaScript: `window.SourceEditor.openForElement(el)`

### Функционал

#### Редактирование кода
- Двухслойный редактор (прозрачный textarea + подсветка)
- Подсветка синтаксиса JavaScript/FASM
- Автосохранение при нажатии "💾 Сохранить"
- Привязка кода к элементу (атрибут `data-code`)

#### Генерация шаблона
Кнопка "📝 Шаблон" генерирует код с точками подключения:

```javascript
// Element: btn
// Исходный код с точками подключения портов

// ===== ПЕРЕМЕННЫЕ =====
var self = this;
var ports = {};
var connections = {};

// ===== ИНИЦИАЛИЗАЦИЯ =====
function init() {
    console.log("Element btn initialized");
    
    // Точка подключения: порт pl (in)
    // PORT_PL_START
    ports["pl"] = {
        type: "in",
        position: "pl",
        connected: false,
        data: null,
        
        // Обработчик входных данных
        onReceive: function(data) {
            console.log("Port pl received:", data);
            this.data = data;
            // TODO: Обработать входные данные
        },
        
        // Отправка данных
        send: function(data) {
            console.log("Port pl sending:", data);
            triggerPort("pl", data);
        }
    };
    // PORT_PL_END
}
```

#### Точки подключения портов

Для каждого порта генерируются:
- **PORT_<POS>_START** - начало точки подключения
- **PORT_<POS>_END** - конец точки подключения

Компилятор может искать эти метки для вставки кода соединений.

---

## 🔧 Система шаблонов кода

### Расположение

- **JS:** `output\www\element_templates.js`

### Структура шаблона

```javascript
'btn': {
    caption: 'Button',
    ports: ['pl', 'pr', 'pt', 'pb'],
    width: 100,
    height: 30,
    backgroundColor: '#3c3c3c',
    borderColor: '#007acc',
    color: '#ffffff',
    properties: {
        'Caption': 'Button',
        'Width': '100',
        'Height': '30'
    },
    // JavaScript код элемента
    jsCode: '// Button Element\n...',
    // HTML шаблон
    htmlTemplate: '<button class="visual-element visual-btn">{caption}</button>'
}
```

### API шаблонов

```javascript
window.ElementTemplates.get(type)        // Получить полный шаблон
window.ElementTemplates.getCaption(type) // Получить название
window.ElementTemplates.getPorts(type)   // Получить порты
window.ElementTemplates.getCodeTemplate(type) // Получить код
window.ElementTemplates.getDefaultSize(type)  // Получить размер
```

---

## 📁 Созданные файлы

### Новые файлы

| Файл | Назначение |
|------|------------|
| `output\www\database.js` | Модуль просмотра БД |
| `output\www\source_editor.js` | Редактор кода элементов |
| `src\data.inc` (обновлён) | Данные БД в сервере |
| `src\code.inc` (обновлён) | API endpoint `/api/database` |

### Обновлённые файлы

| Файл | Изменения |
|------|-----------|
| `output\www\index.html` | Модальные окна, кнопка БД |
| `output\www\style.css` | Стили для БД и редактора |
| `output\www\database.js` | Интеграция с API |

---

## 🎯 Как использовать

### 1. Просмотр базы данных

```
1. Запустить сервер (output\element.exe)
2. Открыть http://localhost:8080
3. Нажать 🗄️ в toolbar
4. Выбрать элемент в таблице
5. Просмотреть детали и код
```

### 2. Редактирование кода элемента

```
1. Создать элемент на рабочей области
2. Выделить элемент
3. Нажать "✏️ Редактировать" в панели свойств
4. Ввести код или сгенерировать шаблон
5. Нажать "💾 Сохранить"
```

### 3. Генерация кода с точками подключения

```
1. Открыть редактор кода
2. Нажать "📝 Шаблон"
3. Выбрать тип элемента
4. Сгенерированный код содержит:
   - Переменные (self, ports, connections)
   - Функцию init()
   - Точки подключения для каждого порта
   - Функцию handleEvent()
   - Вспомогательную функцию triggerPort()
```

---

## 🔄 Компиляция проекта

### Последовательность

```
Project.json (JSON с элементами и соединениями)
    ↓
Compiler (читает JSON, извлекает код элементов)
    ↓
┌─────────────────────────────────────┐
│ main.asm (главный файл)             │
│ - Console element (начальный)       │
│ - include 'elements/el_2.inc'       │
│ - include 'elements/el_3.inc'       │
└─────────────────────────────────────┘
    ↓
elements/el_2.inc (код элемента 2)
elements/el_3.inc (код элемента 3)
    ↓
FASM Compiler (fasm main.asm output.exe)
    ↓
output.exe
```

### Обработка точек подключения

Компилятор ищет в коде элементов метки:
- `PORT_PL_START` ... `PORT_PL_END`
- `PORT_PR_START` ... `PORT_PR_END`
- `PORT_PT_START` ... `PORT_PT_END`
- `PORT_PB_START` ... `PORT_PB_END`

В эти секции вставляется код соединений из проекта.

---

## 📊 Статистика

| Компонент | Строк кода | Файлов |
|-----------|------------|--------|
| Database Viewer | ~360 | 1 |
| Source Editor | ~280 | 1 |
| CSS стили | ~380 | 1 |
| HTML разметка | ~60 | 1 |
| Server (ASM) | ~20 | 2 |
| **Итого** | **~1100** | **6** |

---

## 🚀 Планы развития

- [ ] Импорт/экспорт элементов БД
- [ ] Редактирование свойств элементов в БД
- [ ] Добавление новых элементов через UI
- [ ] Валидация кода элементов
- [ ] Подсветка синтаксиса FASM
- [ ] Автодополнение кода
- [ ] Отладка кода в браузере
- [ ] Компиляция в реальном времени

---

**Element IDE Team**  
2026-03-16
