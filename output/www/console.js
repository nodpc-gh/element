// Element IDE - Console Module
// Консоль для вывода сообщений и выполнения команд

(function() {
'use strict';

var consoleOutput = null;
var consoleInput = null;
var consolePanel = null;
var commandHistory = [];
var historyIndex = -1;

// ===== ИНИЦИАЛИЗАЦИЯ =====
function init() {
    consoleOutput = document.getElementById('consoleOutput');
    consoleInput = document.getElementById('consoleInput');
    consolePanel = document.getElementById('consolePanel');

    if (consoleOutput) {
        // Приветственное сообщение
        log('Element IDE Console v1.0', 'success');
        log('Введите "help" для списка команд', 'info');
    }

    setupEventListeners();
}

// ===== НАСТРОЙКА СОБЫТИЙ =====
function setupEventListeners() {
    // Ввод команд
    if (consoleInput) {
        consoleInput.addEventListener('keydown', function(e) {
            if (e.key === 'Enter') {
                executeCommand(this.value);
                this.value = '';
                commandHistory.push(this.value);
                historyIndex = commandHistory.length;
            } else if (e.key === 'ArrowUp') {
                e.preventDefault();
                if (historyIndex > 0) {
                    historyIndex--;
                    this.value = commandHistory[historyIndex] || '';
                }
            } else if (e.key === 'ArrowDown') {
                e.preventDefault();
                if (historyIndex < commandHistory.length - 1) {
                    historyIndex++;
                    this.value = commandHistory[historyIndex] || '';
                } else {
                    historyIndex = commandHistory.length;
                    this.value = '';
                }
            }
        });
    }

    // Кнопка очистки
    var btnClear = document.getElementById('btnClearConsole');
    if (btnClear) {
        btnClear.onclick = clear;
    }

    // Кнопка сворачивания
    var btnToggle = document.getElementById('btnToggleConsole');
    if (btnToggle) {
        btnToggle.onclick = toggle;
    }
}

// ===== ЛОГИРОВАНИЕ =====
function log(message, type) {
    if (!consoleOutput) return;

    var entry = document.createElement('div');
    entry.className = 'log-entry log-' + (type || 'info');

    var timestamp = new Date().toLocaleTimeString();
    entry.textContent = '[' + timestamp + '] ' + message;

    consoleOutput.appendChild(entry);
    consoleOutput.scrollTop = consoleOutput.scrollHeight;
}

function info(message) { log(message, 'info'); }
function warn(message) { log(message, 'warn'); }
function error(message) { log(message, 'error'); }
function success(message) { log(message, 'success'); }
function command(message) { log(message, 'command'); }

// Активная оболочка — отображает действия как команды
function cmd(operation, details) {
    if (!consoleOutput) return;

    var entry = document.createElement('div');
    entry.className = 'log-entry log-cmd';

    var timestamp = new Date().toLocaleTimeString();
    var msg = '[' + timestamp + '] <span class="cmd-prompt">$</span> <span class="cmd-op">' + operation + '</span>';

    if (details) {
        msg += ' <span class="cmd-args">' + details + '</span>';
    }

    entry.innerHTML = msg;

    consoleOutput.appendChild(entry);
    consoleOutput.scrollTop = consoleOutput.scrollHeight;
}

// ===== ОЧИСТКА =====
function clear() {
    if (consoleOutput) {
        consoleOutput.innerHTML = '';
        log('Консоль очищена', 'info');
    }
}

// ===== СВЁРНУТЬ/РАЗВЕРНУТЬ =====
function toggle() {
    if (consolePanel) {
        consolePanel.classList.toggle('collapsed');
        var btn = document.getElementById('btnToggleConsole');
        if (btn) {
            btn.textContent = consolePanel.classList.contains('collapsed') ? '▲' : '▼';
        }
    }
}

// ===== ВЫПОЛНЕНИЕ КОМАНД =====
function executeCommand(cmd) {
    if (!cmd || !cmd.trim()) return;

    cmd = cmd.trim();
    command(' > ' + cmd);

    // Разбираем команду
    var parts = cmd.split(' ');
    var commandName = parts[0].toLowerCase();
    var args = parts.slice(1);

    switch (commandName) {
        case 'help':
            showHelp();
            break;

        case 'clear':
        case 'cls':
            clear();
            break;

        case 'log':
            info(args.join(' '));
            break;

        case 'elements':
        case 'els':
            listElements();
            break;

        case 'connections':
        case 'conns':
            listConnections();
            break;

        case 'select':
        case 'sel':
            selectElement(args[0]);
            break;

        case 'reset':
            resetElementPosition(args[0]);
            break;

        case 'resetall':
            resetAllElements();
            break;

        case 'run':
            runProject();
            break;

        case 'compile':
            compileProject();
            break;

        case 'eval':
            evaluateCode(args.join(' '));
            break;

        default:
            // Пробуем выполнить как JavaScript
            try {
                var result = eval(cmd);
                if (result !== undefined) {
                    log(' = ' + JSON.stringify(result), 'info');
                }
            } catch (e) {
                error('Неизвестная команда: ' + commandName);
                log('Введите "help" для списка команд', 'info');
            }
    }
}

function showHelp() {
    log('Доступные команды:', 'info');
    log('  help          - Показать эту справку', 'info');
    log('  clear, cls    - Очистить консоль', 'info');
    log('  log <msg>     - Вывести сообщение', 'info');
    log('  elements, els - Список элементов', 'info');
    log('  connections   - Список соединений', 'info');
    log('  select <id>   - Выбрать элемент', 'info');
    log('  reset <id>    - Сбросить позицию элемента', 'info');
    log('  resetall      - Сбросить все элементы в центр', 'info');
    log('  run           - Запустить проект', 'info');
    log('  compile       - Компилировать проект', 'info');
    log('  eval <code>   - Выполнить JavaScript', 'info');
    log('  <js-code>     - Выполнить JavaScript', 'info');
}

function listElements() {
    var ec = document.getElementById('ec');
    if (!ec) {
        warn('Рабочая область не найдена');
        return;
    }

    var elements = ec.querySelectorAll('.element');
    if (elements.length === 0) {
        log('Нет элементов', 'info');
        return;
    }

    log('Элементы (' + elements.length + '):', 'success');
    elements.forEach(function(el) {
        var type = el.getAttribute('data-type');
        var caption = el.getAttribute('data-caption');
        var id = el.id;
        log('  ' + id + ' - ' + type + ' "' + caption + '"', 'info');
    });
}

function listConnections() {
    var conns = window.conns || [];
    if (conns.length === 0) {
        log('Нет соединений', 'info');
        return;
    }

    log('Соединения (' + conns.length + '):', 'success');
    conns.forEach(function(c, i) {
        log('  ' + (i + 1) + '. ' + c.fromId + ' → ' + c.toId + ' (' + c.type + ')', 'info');
    });
}

function selectElement(id) {
    if (!id) {
        warn('Укажите ID элемента');
        return;
    }

    var el = document.getElementById(id) || document.getElementById(id + '_ws');
    if (!el) {
        error('Элемент не найден: ' + id);
        return;
    }

    if (window.sel) {
        window.sel(el);
        success('Выбран элемент: ' + el.id);
    }
}

function resetElementPosition(id) {
    if (!id) {
        warn('Укажите ID элемента (например: reset el_1)');
        return;
    }

    var el = document.getElementById(id) || document.getElementById(id + '_ws');
    if (!el) {
        error('Элемент не найден: ' + id);
        return;
    }

    el.style.left = '100px';
    el.style.top = '100px';

    // Синхронизируем с парным элементом
    var baseId = el.getAttribute('data-base-id');
    if (baseId) {
        var pair = document.querySelector('[data-base-id="' + baseId + '"]:not(#' + el.id + ')');
        if (pair) {
            pair.style.left = '100px';
            pair.style.top = '100px';
        }
    }

    success('Позиция элемента сброшена: ' + el.id);
}

function resetAllElements() {
    var ec = document.getElementById('ec');
    if (!ec) {
        warn('Рабочая область не найдена');
        return;
    }

    var elements = ec.querySelectorAll('.element');
    if (elements.length === 0) {
        log('Нет элементов', 'info');
        return;
    }

    var x = 50, y = 50;
    elements.forEach(function(el, i) {
        el.style.left = (x + (i * 30)) + 'px';
        el.style.top = (y + (i * 30)) + 'px';

        // Синхронизируем с парным элементом
        var baseId = el.getAttribute('data-base-id');
        if (baseId) {
            var pair = document.querySelector('[data-base-id="' + baseId + '"]:not(#' + el.id + ')');
            if (pair) {
                pair.style.left = el.style.left;
                pair.style.top = el.style.top;
            }
        }
    });

    success('Все элементы (' + elements.length + ') размещены в центре');
}

function runProject() {
    log('Запуск проекта...', 'command');
    // Здесь будет логика запуска
    log('Проект запущен (симуляция)', 'success');
}

function compileProject() {
    log('Компиляция проекта...', 'command');
    if (window.Compiler) {
        var project = window.Compiler.compile();
        log('Элементов: ' + (project.elements || []).length, 'info');
        log('Соединений: ' + (project.connections || []).length, 'info');
        success('Компиляция завершена', 'success');
    } else {
        error('Компилятор не доступен');
    }
}

function evaluateCode(code) {
    if (!code) return;
    try {
        var result = eval(code);
        if (result !== undefined) {
            log(' = ' + JSON.stringify(result, null, 2), 'info');
        }
    } catch (e) {
        error('Ошибка: ' + e.message);
    }
}

// ===== ЭКСПОРТ =====
window.Console = {
    init: init,
    log: log,
    info: info,
    warn: warn,
    error: error,
    success: success,
    command: command,
    cmd: cmd,
    clear: clear,
    execute: executeCommand
};

// Переопределяем console.log для вывода в нашу консоль
(function() {
    var originalLog = console.log;
    console.log = function() {
        originalLog.apply(console, arguments);
        var args = Array.prototype.slice.call(arguments);
        log(args.join(' '), 'info');
    };

    var originalError = console.error;
    console.error = function() {
        originalError.apply(console, arguments);
        var args = Array.prototype.slice.call(arguments);
        log(args.join(' '), 'error');
    };

    var originalWarn = console.warn;
    console.warn = function() {
        originalWarn.apply(console, arguments);
        var args = Array.prototype.slice.call(arguments);
        log(args.join(' '), 'warn');
    };
})();

// Автозапуск
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', init);
} else {
    init();
}

})();
