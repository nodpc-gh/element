// Element IDE - Source Code Editor Module
// Просмотр и редактирование исходного кода элементов с точками подключения портов

(function() {
'use strict';

var currentElement = null;
var currentType = null;

// ===== ИНИЦИАЛИЗАЦИЯ =====
function init() {
    setupEventListeners();
}

// ===== НАСТРОЙКА СОБЫТИЙ =====
function setupEventListeners() {
    // Кнопка редактора кода в toolbar - открываем через app.js
    var btnSource = document.getElementById('btnSource');
    if (btnSource) {
        // Обработчик уже есть в app.js, не переопределяем
    }

    // Кнопка редактирования кода в панели свойств - обрабатывается в app.js
    var btnEditCode = document.getElementById('btnEditCode');
    if (btnEditCode) {
        // Обработчик уже есть в app.js, не переопределяем
    }

    // Закрытие модального окна
    var btnCloseSource = document.getElementById('btnCloseSource');
    if (btnCloseSource) {
        btnCloseSource.onclick = closeSourceEditor;
    }

    // Сохранение кода
    var btnSaveSource = document.getElementById('btnSaveSource');
    if (btnSaveSource) {
        btnSaveSource.onclick = saveSourceCode;
    }

    // Очистка кода
    var btnClearSource = document.getElementById('btnClearSource');
    if (btnClearSource) {
        btnClearSource.onclick = clearSourceCode;
    }

    // Генерация шаблона
    var btnGenerateTemplate = document.getElementById('btnGenerateTemplate');
    if (btnGenerateTemplate) {
        btnGenerateTemplate.onclick = generateTemplate;
    }

    // Редактор кода - подсветка при вводе
    var sourceCodeEditor = document.getElementById('sourceCodeEditor');
    if (sourceCodeEditor) {
        sourceCodeEditor.addEventListener('input', updateHighlight);
        sourceCodeEditor.addEventListener('scroll', syncScroll);
    }

    // Overlay
    var overlay = document.getElementById('overlay');
    if (overlay) {
        overlay.addEventListener('click', function() {
            closeSourceEditor();
        });
    }
}

// ===== ОТКРЫТЬ РЕДАКТОР КОДА =====
function openSourceEditor() {
    var modal = document.getElementById('sourceModal');
    if (modal) {
        modal.style.display = 'block';
        document.getElementById('overlay').style.display = 'block';
        
        // Если элемент не выбран, показываем сообщение
        if (!currentElement) {
            document.getElementById('sourceElId').textContent = 'Не выбран';
            document.getElementById('sourceElType').textContent = '-';
            document.getElementById('sourceCodeEditor').value = '// Выберите элемент для редактирования кода';
            updateHighlight();
        }
    }
}

function openSourceEditorForSelected() {
    var selEl = window.selEl || null;
    if (!selEl) {
        alert('Сначала выберите элемент');
        return;
    }
    
    openSourceEditorForElement(selEl);
}

function openSourceEditorForElement(el) {
    currentElement = el;
    var baseId = el.id.replace('_ws', '');
    var type = el.getAttribute('data-type');
    var code = el.getAttribute('data-code') || '';
    
    currentType = type;
    
    // Если код пустой, берём из шаблона
    if (!code) {
        code = window.ElementTemplates ? window.ElementTemplates.getCodeTemplate(type) : '';
    }
    
    document.getElementById('sourceElId').textContent = baseId + ' (' + el.id + ')';
    document.getElementById('sourceElType').textContent = type;
    document.getElementById('sourceCodeEditor').value = code;
    
    updateHighlight();
    
    var modal = document.getElementById('sourceModal');
    if (modal) {
        modal.style.display = 'block';
        document.getElementById('overlay').style.display = 'block';
    }
    
    showStatus('Код загружен', 'success');
}

// Открыть для типа элемента (из базы данных)
function openForType(type) {
    currentType = type;
    currentElement = null;
    
    var code = window.ElementTemplates ? window.ElementTemplates.getCodeTemplate(type) : '';
    
    document.getElementById('sourceElId').textContent = 'Шаблон: ' + type;
    document.getElementById('sourceElType').textContent = type;
    document.getElementById('sourceCodeEditor').value = code;
    
    updateHighlight();
    
    var modal = document.getElementById('sourceModal');
    if (modal) {
        modal.style.display = 'block';
        document.getElementById('overlay').style.display = 'block';
    }
    
    showStatus('Шаблон загружен', 'success');
}

// ===== ЗАКРЫТЬ РЕДАКТОР КОДА =====
function closeSourceEditor() {
    var modal = document.getElementById('sourceModal');
    if (modal) {
        modal.style.display = 'none';
        document.getElementById('overlay').style.display = 'none';
    }
}

// ===== СОХРАНИТЬ КОД =====
function saveSourceCode() {
    var code = document.getElementById('sourceCodeEditor').value;
    
    if (currentElement) {
        currentElement.setAttribute('data-code', code);
        showStatus('Код сохранён в элемент', 'success');
    } else if (currentType) {
        // Сохраняем в шаблоны (временно)
        if (window.ElementTemplates) {
            var tmpl = window.ElementTemplates.get(currentType);
            if (tmpl) {
                tmpl.jsCode = code;
            }
        }
        showStatus('Код сохранён в шаблон', 'success');
    } else {
        showStatus('Нет элемента для сохранения', 'error');
    }
    
    updateHighlight();
}

// ===== ОЧИСТИТЬ КОД =====
function clearSourceCode() {
    if (confirm('Очистить код элемента?')) {
        document.getElementById('sourceCodeEditor').value = '';
        updateHighlight();
        showStatus('Код очищен', 'success');
    }
}

// ===== ГЕНЕРИРОВАТЬ ШАБЛОН =====
function generateTemplate() {
    if (!currentType) {
        showStatus('Тип элемента не определён', 'error');
        return;
    }
    
    var template = generatePortTemplate(currentType);
    document.getElementById('sourceCodeEditor').value = template;
    updateHighlight();
    showStatus('Шаблон портов сгенерирован', 'success');
}

// ===== ГЕНЕРАЦИЯ ШАБЛОНА ПО ТОЧКАМ ПОДКЛЮЧЕНИЯ =====
function generatePortTemplate(type) {
    var ports = window.ElementTemplates ? window.ElementTemplates.getPorts(type) : ['pl', 'pr', 'pt', 'pb'];
    
    var portNames = {
        'pl': 'in',
        'pr': 'out', 
        'pt': 'id',
        'pb': 'data'
    };
    
    var template = '// Element: ' + type + '\n';
    template += '// Исходный код с точками подключения портов\n\n';
    
    // Объявление переменных
    template += '// ===== ПЕРЕМЕННЫЕ =====\n';
    template += 'var self = this;\n';
    template += 'var ports = {};\n';
    template += 'var connections = {};\n\n';
    
    // Инициализация
    template += '// ===== ИНИЦИАЛИЗАЦИЯ =====\n';
    template += 'function init() {\n';
    template += '    console.log("Element ' + type + ' initialized");\n';
    
    // Точки подключения для каждого порта
    ports.forEach(function(pos) {
        var portName = portNames[pos];
        template += '    \n';
        template += '    // Точка подключения: порт ' + pos + ' (' + portName + ')\n';
        template += '    // PORT_' + pos.toUpperCase() + '_START\n';
        template += '    ports["' + pos + '"] = {\n';
        template += '        type: "' + portName + '",\n';
        template += '        position: "' + pos + '",\n';
        template += '        connected: false,\n';
        template += '        data: null,\n';
        template += '        \n';
        template += '        // Обработчик входных данных\n';
        template += '        onReceive: function(data) {\n';
        template += '            console.log("Port ' + pos + ' received:", data);\n';
        template += '            this.data = data;\n';
        template += '            // TODO: Обработать входные данные\n';
        template += '        },\n';
        template += '        \n';
        template += '        // Отправка данных\n';
        template += '        send: function(data) {\n';
        template += '            console.log("Port ' + pos + ' sending:", data);\n';
        template += '            triggerPort("' + pos + '", data);\n';
        template += '        }\n';
        template += '    };\n';
        template += '    // PORT_' + pos.toUpperCase() + '_END\n';
    });
    
    template += '}\n\n';
    
    // Основная функция обработки
    template += '// ===== ОБРАБОТКА СОБЫТИЙ =====\n';
    template += 'function handleEvent(event) {\n';
    template += '    switch(event.type) {\n';
    template += '        case "init":\n';
    template += '            init();\n';
    template += '            break;\n';
    template += '        case "click":\n';
    template += '            // Обработка клика\n';
    template += '            ports["pr"].send({ clicked: true });\n';
    template += '            break;\n';
    template += '        default:\n';
    template += '            console.log("Unknown event:", event);\n';
    template += '    }\n';
    template += '}\n\n';
    
    // Функция triggerPort
    template += '// ===== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ =====\n';
    template += 'function triggerPort(portPos, data) {\n';
    template += '    // Отправка данных в подключенный порт\n';
    template += '    console.log("Trigger port " + portPos + ":", data);\n';
    template += '    \n';
    template += '    // Здесь будет вызов соединения\n';
    template += '    if (window.conns) {\n';
    template += '        window.conns.forEach(function(conn) {\n';
    template += '            if (conn.fromPort === portPos) {\n';
    template += '                // Передать данные в целевой порт\n';
    template += '            }\n';
    template += '        });\n';
    template += '    }\n';
    template += '}\n\n';
    
    // Запуск
    template += '// Автозапуск\n';
    template += 'handleEvent({ type: "init" });\n';
    
    return template;
}

// ===== ПОДСВЕТКА КОДА =====
function updateHighlight() {
    var editor = document.getElementById('sourceCodeEditor');
    var highlight = document.getElementById('sourceCodeHighlight');

    if (!editor || !highlight) return;

    var code = editor.value;
    var highlighted = highlightCode(code);

    highlight.innerHTML = highlighted;
}

function highlightCode(code) {
    // Простая подсветка синтаксиса JavaScript/FASM
    var escaped = code
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;');
    
    // Комментарии
    escaped = escaped.replace(/(\/\/[^\n]*)/g, '<span class="hl-comment">$1</span>');
    escaped = escaped.replace(/(\/\*[\s\S]*?\*\/)/g, '<span class="hl-comment">$1</span>');
    
    // Строки
    escaped = escaped.replace(/(".*?")/g, '<span class="hl-string">$1</span>');
    escaped = escaped.replace(/('.*?')/g, '<span class="hl-string">$1</span>');
    
    // Ключевые слова JS
    var keywords = ['var', 'let', 'const', 'function', 'return', 'if', 'else', 'for', 'while', 'switch', 'case', 'break', 'default', 'new', 'this', 'typeof'];
    keywords.forEach(function(kw) {
        var regex = new RegExp('\\b(' + kw + ')\\b', 'g');
        escaped = escaped.replace(regex, '<span class="hl-keyword">$1</span>');
    });
    
    // Функции
    escaped = escaped.replace(/\b([a-zA-Z_][a-zA-Z0-9_]*)(?=\()/g, '<span class="hl-function">$1</span>');
    
    // Числа
    escaped = escaped.replace(/\b(\d+)\b/g, '<span class="hl-number">$1</span>');
    
    // Ассемблерные директивы (если есть)
    escaped = escaped.replace(/\b(push|pop|call|ret|mov|add|sub|cmp|je|jne|jmp|test|xor|and|or)\b/g, '<span class="hl-asm">$1</span>');
    
    return escaped;
}

function syncScroll() {
    var editor = document.getElementById('sourceCodeEditor');
    var highlight = document.getElementById('sourceCodeHighlight');
    
    if (!editor || !highlight) return;
    
    highlight.scrollTop = editor.scrollTop;
    highlight.scrollLeft = editor.scrollLeft;
}

// ===== СТАТУС =====
function showStatus(message, type) {
    var status = document.getElementById('sourceStatus');
    if (!status) return;
    
    status.textContent = message;
    status.className = 'source-status source-' + (type || 'info');
    
    setTimeout(function() {
        status.textContent = '';
    }, 3000);
}

// ===== ЭКСПОРТ =====
window.SourceEditor = {
    init: init,
    open: openSourceEditor,
    openForElement: openSourceEditorForElement,
    openForType: openForType,
    close: closeSourceEditor,
    save: saveSourceCode,
    clear: clearSourceCode,
    generateTemplate: generateTemplate,
    getCurrentElement: function() { return currentElement; },
    getCurrentType: function() { return currentType; }
};

// Автозапуск
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', init);
} else {
    init();
}

})();
