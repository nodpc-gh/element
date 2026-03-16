// Element IDE - Compiler Module
// Экспорт/импорт проектов в JSON

(function() {
'use strict';

// ===== ЭКСПОРТ ПРОЕКТА =====
function compileProject() {
    var elements = [];
    var ec = document.getElementById('ec');

    // Собираем только элементы рабочей области (с суффиксом _ws)
    ec.querySelectorAll('.element[id$="_ws"]').forEach(function(el) {
        var ports = [];
        el.querySelectorAll('.port').forEach(function(p) {
            ports.push({
                pos: p.className.split(' ')[1],
                type: p.getAttribute('data-port'),
                id: p.getAttribute('data-port-id')
            });
        });

        // Получаем код элемента или из шаблона
        var code = el.getAttribute('data-code') || '';
        if (!code) {
            // Если код не задан, берём шаблон
            var type = el.getAttribute('data-type');
            code = ElementTemplates.getCodeTemplate(type) || '';
        }

        elements.push({
            id: el.id.replace('_ws', ''),  // Убираем суффикс _ws
            type: el.getAttribute('data-type'),
            caption: el.getAttribute('data-caption'),
            x: parseInt(el.style.left) || 0,
            y: parseInt(el.style.top) || 0,
            ev: parseInt(el.getAttribute('data-ev') || 0),
            code: code,
            ports: ports
        });
    });

    var connections = [];
    if (typeof window.conns !== 'undefined') {
        window.conns.forEach(function(cn) {
            connections.push({
                from: cn.fromId,
                to: cn.toId,
                type: cn.type
            });
        });
    }

    return {
        version: '2.0',
        elements: elements,
        connections: connections
    };
}

// ===== СОХРАНИТЬ В JSON =====
function saveToJSON() {
    var project = compileProject();
    var json = JSON.stringify(project, null, 2);
    
    var blob = new Blob([json], { type: 'application/json' });
    var a = document.createElement('a');
    a.href = URL.createObjectURL(blob);
    a.download = 'element_project.json';
    a.click();
    
    return json;
}

// ===== КОПИРОВАТЬ В БУФЕР =====
function copyToClipboard() {
    var project = compileProject();
    var json = JSON.stringify(project, null, 2);
    
    if (navigator.clipboard) {
        navigator.clipboard.writeText(json);
    } else {
        var textarea = document.createElement('textarea');
        textarea.value = json;
        document.body.appendChild(textarea);
        textarea.select();
        document.execCommand('copy');
        document.body.removeChild(textarea);
    }
    
    return json;
}

// ===== ЗАГРУЗИТЬ ИЗ JSON =====
function loadFromJSON(jsonString) {
    try {
        var project = JSON.parse(jsonString);
        
        console.log('Loaded project:', project);
        console.log('Has elements:', !!project.elements);
        console.log('Has connections:', !!project.connections);

        if (!project.elements || !project.connections) {
            console.error('Missing elements or connections');
            throw new Error('Неверный формат проекта');
        }

        return project;
    } catch (err) {
        console.error('Ошибка загрузки проекта:', err);
        console.error('JSON string:', jsonString.substring(0, 200));
        return null;
    }
}

// ===== ПРИМЕНЕНИЕ ПРОЕКТА =====
function applyProject(project) {
    if (!project) return false;

    var ecWorkspace = document.getElementById('ec');
    var ecForm = document.getElementById('ec-form');
    var svg = document.getElementById('svg');

    // Очищаем текущие элементы из обоих контейнеров
    ecWorkspace.querySelectorAll('.element').forEach(function(el) {
        el.parentNode.removeChild(el);
    });
    ecForm.querySelectorAll('.element, .form-window').forEach(function(el) {
        el.parentNode.removeChild(el);
    });

    // Очищаем соединения из SVG
    svg.querySelectorAll('.line').forEach(function(line) {
        line.parentNode.removeChild(line);
    });

    // Очищаем массив соединений
    window.conns = [];

    // Сбрасываем счётчик
    window.elCnt = 0;
    window.connId = 0;

    // Создаём форму по умолчанию
    if (typeof window.createDefaultForm === 'function') {
        window.createDefaultForm();
    } else {
        // Если функция ещё не доступна, создаём форму вручную
        ecForm.innerHTML = '';
        var formWin = document.createElement('div');
        formWin.className = 'form-window';
        formWin.id = 'defaultForm';
        formWin.innerHTML = '<div class="form-content" id="formContent"></div>';
        ecForm.appendChild(formWin);
    }
    var formContent = document.getElementById('formContent');

    // Загружаем элементы
    project.elements.forEach(function(elData) {
        // Находим максимальный ID
        var num = parseInt(elData.id.replace('el_', ''));
        if (num > window.elCnt) window.elCnt = num;

        var baseId = elData.id;
        var caption = elData.caption;

        // === СОЗДАЁМ ЭЛЕМЕНТ ДЛЯ РАБОЧЕЙ ОБЛАСТИ ===
        var elWorkspace = document.createElement('div');
        elWorkspace.className = 'element';
        elWorkspace.id = baseId + '_ws';
        elWorkspace.setAttribute('data-base-id', baseId);
        elWorkspace.setAttribute('data-type', elData.type);
        elWorkspace.setAttribute('data-caption', caption);
        elWorkspace.setAttribute('data-ev', elData.ev || 0);
        elWorkspace.setAttribute('data-code', elData.code || '');
        elWorkspace.style.left = elData.x + 'px';
        elWorkspace.style.top = elData.y + 'px';

        // Вычисляем размер на основе портов
        var portCounts = { pl: 0, pr: 0, pt: 0, pb: 0 };
        elData.ports.forEach(function(p) { portCounts[p.pos]++; });

        var minWidth = 100, minHeight = 60;
        if (portCounts.pt > 0 || portCounts.pb > 0) {
            minWidth = Math.max(minWidth, 10 + portCounts.pt * 12 + 20, 10 + portCounts.pb * 12 + 20);
        }
        if (portCounts.pl > 0 || portCounts.pr > 0) {
            minHeight = Math.max(minHeight, 8 + portCounts.pl * 12 + 20, 8 + portCounts.pr * 12 + 20);
        }
        elWorkspace.style.minWidth = minWidth + 'px';
        elWorkspace.style.minHeight = minHeight + 'px';

        elWorkspace.innerHTML = '<div class="el-hdr">' + caption + '</div>' +
            '<div class="el-body"><div class="el-data">' + caption + '</div>' +
            '<div class="el-stats">⚡' + (elData.ev || 0) + '</div></div>';

        elWorkspace.onmousedown = window.onElDown || function() {};
        elWorkspace.ondblclick = window.onElDbl || function() {};
        elWorkspace.oncontextmenu = window.onElCtx || function() {};

        ecWorkspace.appendChild(elWorkspace);

        // Загружаем порты
        var portOffset = { pl: 0, pr: 0, pt: 0, pb: 0 };
        elData.ports.forEach(function(p) {
            var port = document.createElement('div');
            port.className = 'port ' + p.pos + ' ' + p.type;
            port.setAttribute('data-port', p.type);
            port.setAttribute('data-port-id', p.id);

            var offset = portOffset[p.pos] * 12;
            portOffset[p.pos]++;

            if (p.pos === 'pl' || p.pos === 'pr') {
                port.style.top = (8 + offset) + 'px';
            } else {
                port.style.left = (10 + offset) + 'px';
            }

            elWorkspace.appendChild(port);

            var s = document.createElement('span');
            s.className = 'port-label ' + p.pos;
            s.textContent = p.type;
            if (p.pos === 'pl') { s.style.left = '10px'; s.style.top = (8 + offset) + 'px'; }
            else if (p.pos === 'pr') { s.style.right = '10px'; s.style.top = (8 + offset) + 'px'; }
            else if (p.pos === 'pt') { s.style.left = (10 + offset) + 'px'; }
            else if (p.pos === 'pb') { s.style.left = (10 + offset) + 'px'; s.style.bottom = '10px'; }
            elWorkspace.appendChild(s);
        });

        // === СОЗДАЁМ КОНТРОЛ ВНУТРИ ФОРМЫ ===
        var elControl = document.createElement('div');
        elControl.className = 'form-control';
        elControl.id = baseId + '_ctrl';
        elControl.setAttribute('data-base-id', baseId);
        elControl.setAttribute('data-type', elData.type);
        elControl.setAttribute('data-caption', caption);
        // Относительные координаты внутри формы
        elControl.style.left = (elData.x - 50) + 'px';
        elControl.style.top = (elData.y - 50) + 'px';

        var size = ElementTemplates.getDefaultSize(elData.type);
        elControl.style.minWidth = size.width + 'px';
        elControl.style.minHeight = size.height + 'px';

        // Визуальное представление контрола
        if (elData.type === 'btn') {
            elControl.textContent = caption;
            elControl.style.width = size.width + 'px';
            elControl.style.height = size.height + 'px';
        } else if (elData.type === 'lbl') {
            elControl.textContent = caption;
        } else if (elData.type === 'edt') {
            elControl.textContent = '';
            elControl.style.justifyContent = 'flex-start';
        } else if (elData.type === 'chk') {
            elControl.textContent = caption;
            elControl.style.justifyContent = 'flex-start';
        } else if (elData.type === 'pnl') {
            elControl.textContent = '';
        } else if (elData.type === 'console') {
            elControl.textContent = '';
            elControl.style.justifyContent = 'flex-start';
            elControl.style.alignItems = 'flex-start';
        } else {
            elControl.textContent = caption;
        }

        elControl.onmousedown = window.onControlDown || function() {};
        elControl.ondblclick = window.onElDbl || function() {};
        elControl.oncontextmenu = window.onElCtx || function() {};

        formContent.appendChild(elControl);
    });

    // Загружаем соединения
    setTimeout(function() {
        project.connections.forEach(function(connData) {
            var fromPort = null, toPort = null;
            ecWorkspace.querySelectorAll('.port').forEach(function(p) {
                if (p.getAttribute('data-port-id') === connData.from) fromPort = p;
                if (p.getAttribute('data-port-id') === connData.to) toPort = p;
            });

            if (fromPort && toPort && typeof window.mkConn !== 'undefined') {
                window.mkConn(fromPort, toPort);
            }
        });

        setTimeout(function() {
            if (typeof window.updLines !== 'undefined') {
                window.updLines();
            }
        }, 100);

        var st = document.getElementById('st');
        if (st) st.textContent = 'Проект загружён: ' + project.version;
    }, 200);

    return true;
}

// ===== ОТКРЫТЬ ФАЙЛ =====
function openFile() {
    var input = document.createElement('input');
    input.type = 'file';
    input.accept = '.json';
    input.onchange = function(e) {
        var file = e.target.files[0];
        if (!file) return;
        
        var reader = new FileReader();
        reader.onload = function(e) {
            var project = loadFromJSON(e.target.result);
            if (project) {
                applyProject(project);
            }
        };
        reader.readAsText(file);
    };
    input.click();
}

// Экспорт функций
window.Compiler = {
    compile: compileProject,
    save: saveToJSON,
    copy: copyToClipboard,
    load: loadFromJSON,
    apply: applyProject,
    openFile: openFile
};

})();
