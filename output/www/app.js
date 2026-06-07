// Element IDE v2.0 - Main Application Script
(function() {
'use strict';

// ===== ИНИЦИАЛИЗАЦИЯ =====
var cvs = document.getElementById('cvs'),
    x = cvs.getContext('2d'),
    ws = document.getElementById('ws'),
    ec = document.getElementById('ec'),
    svg = document.getElementById('svg'),
    st = document.getElementById('st'),
    ctx = document.getElementById('ctx'),
    ch = document.getElementById('ch'),
    ctxLn = document.getElementById('ctxLn'),
    lnTypeInfo = document.getElementById('lnTypeInfo'),
    compileModal = document.getElementById('compileModal'),
    compileOutput = document.getElementById('compileOutput'),
    compileElCnt = document.getElementById('compileElCnt'),
    compileConnCnt = document.getElementById('compileConnCnt'),
    sourceModal = document.getElementById('sourceModal'),
    sourceCodeEditor = document.getElementById('sourceCodeEditor'),
    sourceElId = document.getElementById('sourceElId'),
    sourceElType = document.getElementById('sourceElType'),
    sourceStatus = document.getElementById('sourceStatus'),
    overlay = document.getElementById('overlay');

// Переменные состояния - делаем глобальными для compiler.js
var dragType = null,
    dragEl = null,
    offX = 0,
    offY = 0,
    selEl = null,
    elCnt = 0,
    conns = [],
    connId = 0,
    connPort = null,
    tmpLine = null,
    ctxT = null,
    ctxType = null,
    ctxLnT = null;

// Переменные для перемещения контролов в форме
var controlDrag = null,
    controlOffX = 0,
    controlOffY = 0;

// Делаем переменные доступными из window
window.elCnt = 0;
window.conns = conns;
window.connId = 0;

// ===== РЕЖИМЫ: РАЗДЕЛЬНЫЕ ДАННЫЕ =====
var formEditorMode = false;
var ecWorkspace = document.getElementById('ec');
var ecForm = document.getElementById('ec-form');
var wsElement = document.getElementById('ws');

// Форма по умолчанию
var defaultForm = null;

// Получить текущий контейнер элементов
function getCurrentEC() {
    return formEditorMode ? ecForm : ecWorkspace;
}

// Переключение режима
function toggleFormEditor() {
    formEditorMode = !formEditorMode;

    // Переключаем видимость контейнеров
    if (formEditorMode) {
        ecWorkspace.classList.remove('active');
        ecForm.classList.add('active');
        wsElement.classList.add('form-editor');
        // Создаём форму по умолчанию если нет
        if (!defaultForm) {
            createDefaultForm();
        }
    } else {
        ecWorkspace.classList.add('active');
        ecForm.classList.remove('active');
        wsElement.classList.remove('form-editor');
    }

    // Обновляем кнопку
    var btnForm = document.getElementById('btnForm');
    if (btnForm) {
        btnForm.textContent = formEditorMode ? '🔲' : '📐';
        btnForm.title = formEditorMode ? 'Рабочая область' : 'Редактор форм';
    }

    // Скрываем/показываем SVG с линиями (только рабочая область)
    var svg = document.getElementById('svg');
    if (svg) {
        svg.style.display = formEditorMode ? 'none' : 'block';
    }

    // Сбрасываем выделение
    sel(null);
}

// Создание формы по умолчанию
function createDefaultForm() {
    ecForm.innerHTML = '';
    
    var formWin = document.createElement('div');
    formWin.className = 'form-window';
    formWin.id = 'defaultForm';
    
    formWin.innerHTML = '<div class="form-content" id="formContent"></div>';
    
    ecForm.appendChild(formWin);
    defaultForm = formWin;
}

document.getElementById('btnForm').onclick = toggleFormEditor;

// Панорамирование и выделение рамкой
var panX = 0, panY = 0,
    isPanning = false,
    panStartX = 0, panStartY = 0,
    isSelecting = false,
    selectStartX = 0, selectStartY = 0,
    selectRect = null,
    selectedElements = [],
    selectedLine = null;  // Выделенная линия

// ===== РАЗМЕР И СЕТКА =====
function resize() {
    var w = ws.clientWidth, h = ws.clientHeight;
    cvs.width = w;
    cvs.height = h;
    svg.style.width = w + 'px';
    svg.style.height = h + 'px';
    ecWorkspace.style.width = w + 'px';
    ecWorkspace.style.height = h + 'px';
    ecForm.style.width = w + 'px';
    ecForm.style.height = h + 'px';
    drawGrid();
}

function drawGrid() {
    x.fillStyle = '#1e1e1e';
    x.fillRect(0, 0, cvs.width, cvs.height);
    x.strokeStyle = '#2a2a2a';
    x.lineWidth = 0.5;
    for (var i = 0; i < cvs.width; i += 20) {
        x.beginPath();
        x.moveTo(i, 0);
        x.lineTo(i, cvs.height);
        x.stroke();
    }
    for (var j = 0; j < cvs.height; j += 20) {
        x.beginPath();
        x.moveTo(0, j);
        x.lineTo(cvs.width, j);
        x.stroke();
    }
}

window.onresize = resize;
setTimeout(resize, 100);

// ===== ПАНРАМИРОВАНИЕ (Средняя кнопка мыши) =====
// В режиме редактора форм панорамирование отключено
ws.addEventListener('mousedown', function(e) {
    if (e.button === 1 && !formEditorMode) {
        isPanning = true;
        panStartX = e.clientX - panX;
        panStartY = e.clientY - panY;
        ws.style.cursor = 'grabbing';
        e.preventDefault();
    }
});

ws.addEventListener('mousemove', function(e) {
    if (isPanning && !formEditorMode) {
        panX = e.clientX - panStartX;
        panY = e.clientY - panStartY;
        ec.style.transform = 'translate(' + panX + 'px,' + panY + 'px)';
        svg.style.transform = 'translate(' + panX + 'px,' + panY + 'px)';
    }
});

ws.addEventListener('mouseup', function(e) {
    isPanning = false;
    ws.style.cursor = 'default';
});

ws.addEventListener('mouseleave', function(e) {
    isPanning = false;
    ws.style.cursor = 'default';
});

// Двойной клик - возврат к началу координат (только в рабочем режиме)
ws.addEventListener('dblclick', function(e) {
    if (!formEditorMode) {
        panX = 0;
        panY = 0;
        ec.style.transform = 'translate(0px,0px)';
        svg.style.transform = 'translate(0px,0px)';
    }
});

// ===== ВЫДЕЛЕНИЕ РАМКОЙ =====
// В режиме редактора форм выделение рамкой отключено
ws.addEventListener('mousedown', function(e) {
    if (e.button === 0 && (e.ctrlKey || e.shiftKey) && !formEditorMode) {
        isSelecting = true;
        var ecRect = getCurrentEC().getBoundingClientRect();
        selectStartX = e.clientX - ecRect.left;
        selectStartY = e.clientY - ecRect.top;
        if (!selectRect) {
            selectRect = document.createElement('div');
            selectRect.className = 'select-rect';
            getCurrentEC().appendChild(selectRect);
        }
        selectRect.style.left = selectStartX + 'px';
        selectRect.style.top = selectStartY + 'px';
        selectRect.style.width = '0px';
        selectRect.style.height = '0px';
    }
});

ws.addEventListener('mousemove', function(e) {
    if (isSelecting && selectRect && !formEditorMode) {
        var ecRect = getCurrentEC().getBoundingClientRect();
        var currentX = e.clientX - ecRect.left;
        var currentY = e.clientY - ecRect.top;
        var left = Math.min(selectStartX, currentX);
        var top = Math.min(selectStartY, currentY);
        var width = Math.abs(currentX - selectStartX);
        var height = Math.abs(currentY - selectStartY);
        selectRect.style.left = left + 'px';
        selectRect.style.top = top + 'px';
        selectRect.style.width = width + 'px';
        selectRect.style.height = height + 'px';
    }
});

ws.addEventListener('mouseup', function(e) {
    if (isSelecting && !formEditorMode) {
        isSelecting = false;
        var rect = selectRect.getBoundingClientRect();
        var ecRect = getCurrentEC().getBoundingClientRect();
        var selectLeft = rect.left - ecRect.left;
        var selectTop = rect.top - ecRect.top;
        var selectWidth = rect.width;
        var selectHeight = rect.height;
        selectedElements = [];
        getCurrentEC().querySelectorAll('.element').forEach(function(el) {
            var elRect = el.getBoundingClientRect();
            var elLeft = elRect.left - ecRect.left;
            var elTop = elRect.top - ecRect.top;
            if (elLeft >= selectLeft && elLeft + elRect.width <= selectLeft + selectWidth &&
                elTop >= selectTop && elTop + elRect.height <= selectTop + selectHeight) {
                selectedElements.push(el);
                el.classList.add('selected');
            }
        });
        if (selectRect && selectRect.parentNode) {
            selectRect.parentNode.removeChild(selectRect);
        }
        selectRect = null;
        if (selectedElements.length > 0) {
            st.textContent = 'Выбрано элементов: ' + selectedElements.length;
        }
    }
});

// ===== DRAG AND DROP =====
// Поддержка старых tree-item и новых element-item
var dragAndDropInitialized = false;

window.initDragAndDrop = function initDragAndDrop() {
    if (dragAndDropInitialized) return;
    
    // Старые tree-item
    document.querySelectorAll('.tree-item[draggable]').forEach(function(it) {
        it.ondragstart = function(e) {
            dragType = this.getAttribute('data-type');
        };
    });

    // Новые element-item из elements.js
    document.querySelectorAll('.element-item').forEach(function(item) {
        item.ondragstart = function(e) {
            dragType = this.getAttribute('data-type');
        };
    });
    
    dragAndDropInitialized = true;
}

// Вызываем один раз после загрузки элементов
setTimeout(function() { window.initDragAndDrop(); }, 500);

// Глобальный обработчик через делегирование
document.addEventListener('dragstart', function(e) {
    if (e.target.classList.contains('element-item') || e.target.classList.contains('tree-item')) {
        dragType = e.target.getAttribute('data-type');
        e.dataTransfer.setData('text/plain', dragType);
        e.dataTransfer.effectAllowed = 'copy';
    }
});

// Обработчики dragover и drop для обоих контейнеров
function handleDragOver(e) {
    e.preventDefault();
    e.dataTransfer.dropEffect = 'copy';
}

function handleDrop(e, container) {
    e.preventDefault();
    e.stopPropagation();
    
    if (formEditorMode) {
        // В режиме редактора форм размещаем контрол внутри формы
        var formContent = document.getElementById('formContent');
        if (!formContent) {
            createDefaultForm();
            formContent = document.getElementById('formContent');
        }
        var r = formContent.getBoundingClientRect();
        if (dragType) {
            var x = e.clientX - r.left;
            var y = e.clientY - r.top;
            mkEl(dragType, x + 50, y + 50);  // Смещение для координат рабочей области
        }
    } else {
        // В рабочем режиме размещаем элемент на рабочем поле
        var r = ecWorkspace.getBoundingClientRect();
        if (dragType) {
            var x = e.clientX - r.left - panX;
            var y = e.clientY - r.top - panY;
            mkEl(dragType, x, y);
        }
    }
    dragType = null;
}

ecWorkspace.ondragover = handleDragOver;
ecWorkspace.ondrop = function(e) { handleDrop(e, ecWorkspace); };

ecForm.ondragover = handleDragOver;
ecForm.ondrop = function(e) { handleDrop(e, ecForm); };

// ===== СОЗДАНИЕ ЭЛЕМЕНТОВ =====
// Элементы создаются в ОБОИХ контейнерах одновременно
function mkEl(type, x, y) {
    // В режиме редактора форм разрешены только контролы
    if (formEditorMode) {
        var allowedControls = ['btn', 'lbl', 'edt', 'chk', 'pnl', 'console', 'opencode'];
        if (allowedControls.indexOf(type) === -1) {
            alert('Элемент "' + type + '" недоступен в режиме редактора форм.\nПереключитесь в рабочий режим для размещения этого элемента.');
            return;
        }
    }

    elCnt++;
    window.elCnt = elCnt;

    var baseId = 'el_' + elCnt;
    var caption = ElementTemplates.getCaption(type);

    // === СОЗДАЁМ ЭЛЕМЕНТ ДЛЯ РАБОЧЕЙ ОБЛАСТИ (с портами) ===
    var elWorkspace = document.createElement('div');
    elWorkspace.className = 'element';
    elWorkspace.id = baseId + '_ws';
    elWorkspace.setAttribute('data-base-id', baseId);
    elWorkspace.setAttribute('data-type', type);
    elWorkspace.setAttribute('data-ev', 0);
    elWorkspace.setAttribute('data-code', '');
    elWorkspace.setAttribute('data-caption', caption);
    elWorkspace.style.left = x + 'px';
    elWorkspace.style.top = y + 'px';
    elWorkspace.style.minWidth = '100px';
    elWorkspace.style.minHeight = '50px';

    // Содержимое для рабочей области
    var contentWs = document.createElement('div');
    contentWs.className = 'el-content';
    contentWs.style.padding = '5px';
    contentWs.style.pointerEvents = 'none';

    var title = document.createElement('div');
    title.className = 'el-hdr';
    title.textContent = caption;
    title.style.fontSize = '11px';
    title.style.fontWeight = 'bold';
    title.style.marginBottom = '5px';
    title.style.pointerEvents = 'none';
    contentWs.appendChild(title);

    elWorkspace.appendChild(contentWs);

    // Добавляем порты
    var ports = ElementTemplates.getPorts(type);
    ports.forEach(function(pos) {
        var lbl = getPortLabel(pos);
        addPort(elWorkspace, pos, lbl);
    });

    elWorkspace.onmousedown = onElDown;
    elWorkspace.ondblclick = onElDbl;
    elWorkspace.oncontextmenu = onElCtx;

    // === СОЗДАЁМ КОНТРОЛ ДЛЯ РЕДАКТОРА ФОРМ (внутри формы) ===
    var formContent = document.getElementById('formContent');
    if (!formContent) {
        createDefaultForm();
        formContent = document.getElementById('formContent');
    }

    var elControl = document.createElement('div');
    elControl.className = 'form-control';
    elControl.id = baseId + '_ctrl';
    elControl.setAttribute('data-base-id', baseId);
    elControl.setAttribute('data-type', type);
    elControl.setAttribute('data-caption', caption);
    // Относительные координаты внутри формы
    elControl.style.left = (x - 50) + 'px';
    elControl.style.top = (y - 50) + 'px';

    // Размеры и содержимое контрола
    var size = ElementTemplates.getDefaultSize(type);
    elControl.style.minWidth = size.width + 'px';
    elControl.style.minHeight = size.height + 'px';

    // Визуальное представление контрола
    if (type === 'btn') {
        elControl.textContent = caption;
        elControl.style.width = size.width + 'px';
        elControl.style.height = size.height + 'px';
    } else if (type === 'lbl') {
        elControl.textContent = caption;
    } else if (type === 'edt') {
        elControl.textContent = '';
        elControl.style.justifyContent = 'flex-start';
    } else if (type === 'chk') {
        elControl.textContent = caption;
        elControl.style.justifyContent = 'flex-start';
    } else if (type === 'pnl') {
        elControl.textContent = '';
    } else if (type === 'console') {
        elControl.textContent = '';
        elControl.style.justifyContent = 'flex-start';
        elControl.style.alignItems = 'flex-start';
    } else if (type === 'opencode') {
        elControl.textContent = '';
        elControl.style.justifyContent = 'flex-start';
        elControl.style.alignItems = 'flex-start';
        elControl.style.background = '#1a1a2e';
        elControl.style.border = '2px solid #e94560';
        elControl.style.color = '#00ff88';
        elControl.style.fontFamily = 'Consolas, monospace';
        elControl.innerHTML = '<div style="padding:4px;font-size:10px;">🤖 OpenCode Agent</div>';
    } else {
        elControl.textContent = caption;
    }

    elControl.onmousedown = onControlDown;
    elControl.ondblclick = onElDbl;
    elControl.oncontextmenu = onElCtx;

    formContent.appendChild(elControl);

    // Добавляем в рабочую область
    ecWorkspace.appendChild(elWorkspace);

    sel(elWorkspace);
    updProps(elWorkspace);

    if (window.Console) {
        window.Console.cmd('mkEl', type + ' ' + baseId + ' at (' + x + ', ' + y + ')');
    }
}

function getPortLabel(pos) {
    var labels = { 'pl': 'in', 'pr': 'out', 'pt': 'id', 'pb': 'data' };
    return labels[pos] || pos;
}

// ===== ПОРТЫ =====
function addPort(el, pos, lbl) {
    var ports = el.querySelectorAll('.port.' + pos).length;
    var p = document.createElement('div');
    p.className = 'port ' + pos + ' ' + lbl;
    p.setAttribute('data-port', lbl);
    var portId = el.id + '_port_' + pos + '_' + ports;
    p.setAttribute('data-port-id', portId);
    p.onmousedown = onPortDown;
    p.onmouseup = onPortUp;
    p.oncontextmenu = onPortCtx;

    // Позиционирование через CSS + offset
    var offset = ports * 12;
    var elHeight = parseInt(el.style.minHeight) || 60;
    var elWidth = parseInt(el.style.minWidth) || 100;

    if (pos === 'pl') {
        p.style.top = (8 + offset) + 'px';
        if (offset > elHeight - 20) { el.style.minHeight = (offset + 20) + 'px'; }
    } else if (pos === 'pr') {
        p.style.top = (8 + offset) + 'px';
        if (offset > elHeight - 20) { el.style.minHeight = (offset + 20) + 'px'; }
    } else if (pos === 'pt') {
        p.style.left = (10 + offset) + 'px';
        if (offset > elWidth - 20) { el.style.minWidth = (offset + 20) + 'px'; }
    } else if (pos === 'pb') {
        p.style.left = (10 + offset) + 'px';
        if (offset > elWidth - 20) { el.style.minWidth = (offset + 20) + 'px'; }
    }

    el.appendChild(p);

    var s = document.createElement('span');
    s.className = 'port-label ' + pos;
    s.textContent = lbl;
    if (pos === 'pl') { s.style.left = '10px'; s.style.top = (8 + offset) + 'px'; }
    else if (pos === 'pr') { s.style.right = '10px'; s.style.top = (8 + offset) + 'px'; }
    else if (pos === 'pt') { s.style.left = (10 + offset) + 'px'; }
    else if (pos === 'pb') { s.style.left = (10 + offset) + 'px'; s.style.bottom = '10px'; }
    el.appendChild(s);
}

// ===== ВЫДЕЛЕНИЕ =====
function sel(el) {
    // Снимаем выделение со всех элементов
    if (selEl && !Array.isArray(selEl)) {
        selEl.classList.remove('selected');
        // Снимаем выделение с парного элемента
        var baseId = selEl.getAttribute('data-base-id');
        if (baseId) {
            var pair = document.querySelector('[data-base-id="' + baseId + '"]:not(#' + selEl.id + ')');
            if (pair) pair.classList.remove('selected');
        }
    }
    selectedElements.forEach(function(e) { e.classList.remove('selected'); });
    selectedElements = [];

    if (el) {
        if (Array.isArray(el)) {
            selectedElements = el;
            el.forEach(function(e) { e.classList.add('selected'); });
            st.textContent = 'Выбрано элементов: ' + el.length;
        } else {
            selEl = el;
            window.selEl = el;  // Экспортируем в window для доступа из других модулей
            el.classList.add('selected');
            // Выделяем парный элемент
            var baseId = el.getAttribute('data-base-id');
            if (baseId) {
                var pair = document.querySelector('[data-base-id="' + baseId + '"]:not(#' + el.id + ')');
                if (pair) {
                    pair.classList.add('selected');
                }
            }
            st.textContent = el.id;
            updProps(el);
        }
    } else {
        selEl = null;
        window.selEl = null;  // Сбрасываем при снятии выделения
        st.textContent = 'ROOT';
        clrProps();
    }
}

// ===== ПЕРЕМЕЩЕНИЕ ЭЛЕМЕНТОВ =====
function onElDown(e) {
    if (e.button !== 0 || e.target.className.indexOf('port') >= 0) return;
    e.stopPropagation();
    dragEl = this;
    var r = ws.getBoundingClientRect();

    // В режиме редактора форм не учитываем панорамирование
    if (formEditorMode) {
        offX = (e.clientX - r.left) - parseInt(dragEl.style.left || 0);
        offY = (e.clientY - r.top) - parseInt(dragEl.style.top || 0);
    } else {
        offX = (e.clientX - r.left - panX) - parseInt(dragEl.style.left || 0);
        offY = (e.clientY - r.top - panY) - parseInt(dragEl.style.top || 0);
    }

    sel(this);
    document.onmousemove = onElMove;
    document.onmouseup = onElUp;
}

// Перемещение контролов внутри формы
function onControlDown(e) {
    if (e.button !== 0) return;
    e.stopPropagation();
    
    var control = this;
    var formContent = document.getElementById('formContent');
    
    controlDrag = control;
    var r = formContent.getBoundingClientRect();
    controlOffX = e.clientX - r.left - parseInt(control.style.left || 0);
    controlOffY = e.clientY - r.top - parseInt(control.style.top || 0);
    
    // Выделяем контрол и соответствующий элемент в рабочей области
    var baseId = control.getAttribute('data-base-id');
    var wsEl = document.getElementById(baseId + '_ws');
    if (wsEl) sel(wsEl);
    
    document.onmousemove = onControlMove;
    document.onmouseup = onControlUp;
}

function onControlMove(e) {
    if (!controlDrag) return;
    var r = document.getElementById('formContent').getBoundingClientRect();
    
    var newLeft = e.clientX - r.left - controlOffX;
    var newTop = e.clientY - r.top - controlOffY;
    
    controlDrag.style.left = newLeft + 'px';
    controlDrag.style.top = newTop + 'px';
}

function onControlUp(e) {
    controlDrag = null;
    document.onmousemove = null;
    document.onmouseup = null;
}

function onElMove(e) {
    if (!dragEl) return;
    var r = ws.getBoundingClientRect();
    var baseId = dragEl.getAttribute('data-base-id');

    // В режиме редактора форм не учитываем панорамирование
    if (formEditorMode) {
        dragEl.style.left = (e.clientX - r.left - offX) + 'px';
        dragEl.style.top = (e.clientY - r.top - offY) + 'px';
        syncElementPosition(baseId, dragEl.style.left, dragEl.style.top);
    } else {
        dragEl.style.left = (e.clientX - r.left - panX - offX) + 'px';
        dragEl.style.top = (e.clientY - r.top - panY - offY) + 'px';
        syncElementPosition(baseId, dragEl.style.left, dragEl.style.top);
        updLines();
    }
}

// Синхронизация позиции между элементом и контролом
function syncElementPosition(baseId, left, top) {
    if (!baseId) return;
    var pairWs = document.getElementById(baseId + '_ws');
    var pairCtrl = document.getElementById(baseId + '_ctrl');
    
    if (pairWs && pairWs !== dragEl) {
        pairWs.style.left = left;
        pairWs.style.top = top;
    }
    if (pairCtrl && pairCtrl !== controlDrag) {
        // Конвертируем координаты для контрола (относительно формы)
        pairCtrl.style.left = (parseInt(left) - 50) + 'px';
        pairCtrl.style.top = (parseInt(top) - 50) + 'px';
    }
}

function onElUp(e) {
    if (dragEl && window.Console) {
        var baseId = dragEl.getAttribute('data-base-id');
        var x = parseInt(dragEl.style.left) || 0;
        var y = parseInt(dragEl.style.top) || 0;
        window.Console.cmd('mvEl', (baseId || dragEl.id) + ' to (' + x + ', ' + y + ')');
    }
    dragEl = null;
    document.onmousemove = null;
    document.onmouseup = null;
}

function onElDbl(e) {
    if (this.parentNode) { delEl(this); }
}

function onElCtx(e) {
    e.preventDefault();
    e.stopPropagation();
    ctxT = this;
    ctxType = 'el';

    // В режиме редактора форм не показываем опции для портов
    var portGrid = ctx.querySelector('.port-grid');
    if (portGrid) {
        portGrid.style.display = formEditorMode ? 'none' : 'grid';
    }

    // Получаем количество портов из парного элемента рабочей области
    var pc = 0;
    var baseId = this.getAttribute('data-base-id');
    if (baseId) {
        var wsPair = document.getElementById(baseId + '_ws');
        if (wsPair) {
            pc = wsPair.querySelectorAll('.port').length;
        }
    }

    ch.textContent = this.id + (formEditorMode ? ' (редактор форм)' : ' (портов: ' + pc + ')');
    ctx.style.left = e.clientX + 'px';
    ctx.style.top = e.clientY + 'px';
    ctx.style.display = 'block';
    document.onclick = hideCtx;
}

// ===== СОЗДАНИЕ СОЕДИНЕНИЙ =====
function onPortDown(e) {
    if (e.button !== 0) return;
    e.stopPropagation();
    connPort = this;
    var r = ws.getBoundingClientRect(),
        pr = connPort.getBoundingClientRect();
    var x1 = pr.left + pr.width / 2 - r.left - panX,
        y1 = pr.top + pr.height / 2 - r.top - panY;

    tmpLine = document.createElementNS('http://www.w3.org/2000/svg', 'path');
    tmpLine.setAttribute('class', 'line-temp');
    tmpLine.setAttribute('stroke', '#00a8cc');
    tmpLine.setAttribute('stroke-width', '1.5');
    tmpLine.setAttribute('stroke-dasharray', '4,3');
    tmpLine.setAttribute('fill', 'none');
    tmpLine.setAttribute('opacity', '0.7');
    tmpLine.style.pointerEvents = 'none';
    svg.appendChild(tmpLine);

    document.onmousemove = onConnMove;
    document.onmouseup = onConnUp;
}

function onConnMove(e) {
    if (!connPort || !tmpLine) return;
    var ec = document.getElementById('ec');
    var ecRect = ec.getBoundingClientRect();
    var pr = connPort.getBoundingClientRect();
    var x1 = pr.left + pr.width / 2 - ecRect.left,
        y1 = pr.top + pr.height / 2 - ecRect.top,
        x2 = e.clientX - ecRect.left,
        y2 = e.clientY - ecRect.top;
    tmpLine.setAttribute('d', 'M' + x1 + ',' + y1 + ' L' + x2 + ',' + y2);
}

function onConnUp(e) {
    if (connPort && tmpLine) {
        tmpLine.parentNode.removeChild(tmpLine);
    }
    connPort = null;
    document.onmousemove = null;
    document.onmouseup = null;
}

function onPortUp(e) {
    e.stopPropagation();
    if (!connPort || connPort === this) return;
    var fEl = connPort.closest('.element'),
        tEl = this.closest('.element');
    if (fEl === tEl) return;
    
    var hasConn = false;
    conns.forEach(function(c) {
        if (c.from === this || c.to === this) { hasConn = true; }
    }.bind(this));
    
    if (hasConn) { delLineByPort(this); }
    mkConn(connPort, this);
    connPort = null;
    if (tmpLine) { tmpLine.parentNode.removeChild(tmpLine); }
}

function mkConn(from, to) {
    connId++;
    window.connId = connId;

    var ln = document.createElementNS('http://www.w3.org/2000/svg', 'path');
    var cls = 'line';
    
    // Определяем тип соединения по позиции порта
    var fromPos = from.className.indexOf('pl') >= 0 ? 'left' :
                  from.className.indexOf('pr') >= 0 ? 'right' :
                  from.className.indexOf('pt') >= 0 ? 'top' : 'bottom';
    var toPos = to.className.indexOf('pl') >= 0 ? 'left' :
                to.className.indexOf('pr') >= 0 ? 'right' :
                to.className.indexOf('pt') >= 0 ? 'top' : 'bottom';
    
    // pl (left) = Методы, pr (right) = События, pt (top) = Данные, pb (bottom) = Свойства
    if (fromPos === 'right' || toPos === 'right') { 
        cls += ' event';  // События - фиолетовый
    } else if (fromPos === 'left' || toPos === 'left') { 
        cls += ' method';  // Методы - зелёный
    } else if (fromPos === 'top' || toPos === 'top') { 
        cls += ' id';  // Данные - оранжевый
    } else if (fromPos === 'bottom' || toPos === 'bottom') { 
        cls += ' data';  // Свойства - голубой
    } else {
        cls += ' event';
    }

    ln.setAttribute('class', cls);
    ln.setAttribute('pointer-events', 'all');
    
    // Двойной клик - удаление
    ln.ondblclick = function(e) {
        e.stopPropagation();
        e.preventDefault();
        delLine(ln);
    };
    // Контекстное меню линии
    ln.oncontextmenu = function(e) {
        e.preventDefault();
        e.stopPropagation();
        ctxLnT = ln;
        var cn = conns.find(function(c) { return c.line === ln; });
        if (cn && ctxLn) {
            ctxLn.style.left = e.clientX + 'px';
            ctxLn.style.top = e.clientY + 'px';
            ctxLn.style.display = 'block';
            lnTypeInfo.textContent = cn.type;
        }
        document.onclick = hideCtxAll;
    };
    svg.appendChild(ln);

    var cn = {
        id: 'c_' + connId,
        from: from,
        to: to,
        line: ln,
        type: cls.split(' ')[1],
        fromId: from.getAttribute('data-port-id'),
        toId: to.getAttribute('data-port-id')
    };
    conns.push(cn);
    window.conns = conns;
    from.classList.add('connected');
    to.classList.add('connected');
    updLines();

    if (window.Console) {
        var fromId = from.getAttribute('data-port-id') || from.className;
        var toId = to.getAttribute('data-port-id') || to.className;
        window.Console.cmd('conn', fromId + ' -> ' + toId + ' (' + (cn.type || 'event') + ')');
    }

    var fEv = from.closest('.element');
    if (fEv) {
        var n = parseInt(fEv.getAttribute('data-ev') || 0) + 1;
        fEv.setAttribute('data-ev', n);
        updStats(fEv);
    }
}

// ===== ОБНОВЛЕНИЕ ЛИНИЙ =====
function portCenter(port) {
    var el = port.closest('.element');
    if (!el) return { x: 0, y: 0 };

    var elLeft = parseInt(el.style.left) || 0;
    var elTop = parseInt(el.style.top) || 0;
    var elW = el.offsetWidth || parseInt(el.style.minWidth) || 100;
    var elH = el.offsetHeight || parseInt(el.style.minHeight) || 60;

    var isPl = port.classList.contains('pl');
    var isPr = port.classList.contains('pr');
    var isPt = port.classList.contains('pt');
    var isPb = port.classList.contains('pb');

    // Port size: 8px + 2*2px border = 12px; center offset = 6
    var portHalf = 6;

    if (isPl) {
        var pTop = parseInt(port.style.top) || 0;
        return { x: elLeft + 2, y: elTop + pTop + portHalf };
    }
    if (isPr) {
        var pTop = parseInt(port.style.top) || 0;
        return { x: elLeft + elW - 2, y: elTop + pTop + portHalf };
    }
    if (isPt) {
        var pLeft = parseInt(port.style.left) || 0;
        return { x: elLeft + pLeft + portHalf, y: elTop + 2 };
    }
    if (isPb) {
        var pLeft = parseInt(port.style.left) || 0;
        return { x: elLeft + pLeft + portHalf, y: elTop + elH - 2 };
    }

    return { x: elLeft, y: elTop };
}

function updLines() {
    var connections = window.conns || conns;

    connections.forEach(function(cn) {
        var fromPort = cn.from,
            toPort = cn.to;
        if (!fromPort || !toPort || !fromPort.parentNode || !toPort.parentNode) {
            if (cn.line && cn.line.parentNode) { cn.line.parentNode.removeChild(cn.line); }
            return;
        }

        var p1 = portCenter(fromPort);
        var p2 = portCenter(toPort);

        var x1 = p1.x, y1 = p1.y,
            x2 = p2.x, y2 = p2.y;

        var pos1 = fromPort.className.indexOf('pl') >= 0 ? 'l' :
                   fromPort.className.indexOf('pr') >= 0 ? 'r' :
                   fromPort.className.indexOf('pt') >= 0 ? 't' : 'b';
        var pos2 = toPort.className.indexOf('pl') >= 0 ? 'l' :
                   toPort.className.indexOf('pr') >= 0 ? 'r' :
                   toPort.className.indexOf('pt') >= 0 ? 't' : 'b';

        var out = 10;
        var sx1 = x1, sy1 = y1, sx2 = x2, sy2 = y2;
        if (pos1 === 'l') { sx1 -= out; } else if (pos1 === 'r') { sx1 += out; }
        else if (pos1 === 't') { sy1 -= out; } else if (pos1 === 'b') { sy1 += out; }
        if (pos2 === 'l') { sx2 -= out; } else if (pos2 === 'r') { sx2 += out; }
        else if (pos2 === 't') { sy2 -= out; } else if (pos2 === 'b') { sy2 += out; }

        var path = 'M' + x1 + ',' + y1 + ' L' + sx1 + ',' + sy1;
        var midX = (sx1 + sx2) / 2, midY = (sy1 + sy2) / 2;

        var route;
        if (Math.abs(sx2 - sx1) > Math.abs(sy2 - sy1)) {
            route = [{ x: sx1, y: sy1 }, { x: midX, y: sy1 }, { x: midX, y: sy2 }, { x: sx2, y: sy2 }];
        } else {
            route = [{ x: sx1, y: sy1 }, { x: sx1, y: midY }, { x: sx2, y: midY }, { x: sx2, y: sy2 }];
        }

        for (var i = 1; i < route.length; i++) {
            path += ' L' + route[i].x + ',' + route[i].y;
        }
        path += ' L' + x2 + ',' + y2;

        if (cn.line) { cn.line.setAttribute('d', path); }
    });

    window.conns = connections;
}

function lineIntersectsRect(x1, y1, x2, y2, rect) {
    function lineLine(x1, y1, x2, y2, x3, y3, x4, y4) {
        var uA = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
        var uB = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
        return (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1);
    }
    var rx = rect.x, ry = rect.y, rw = rect.w, rh = rect.h;
    return lineLine(x1, y1, x2, y2, rx, ry, rx + rw, ry) ||
           lineLine(x1, y1, x2, y2, rx + rw, ry, rx + rw, ry + rh) ||
           lineLine(x1, y1, x2, y2, rx + rw, ry + rh, rx, ry + rh) ||
           lineLine(x1, y1, x2, y2, rx, ry + rh, rx, ry);
}

function updStats(el) {
    var s = el.querySelector('.el-stats');
    if (s) { s.textContent = '⚡' + (el.getAttribute('data-ev') || 0); }
}

// ===== КОНТЕКСТНЫЕ МЕНЮ =====
function onPortCtx(e) {
    e.preventDefault();
    e.stopPropagation();
    ctxT = this;
    ctxType = 'port';
    var portId = this.getAttribute('data-port-id');
    ch.textContent = 'Port: ' + portId;
    ctx.style.left = e.clientX + 'px';
    ctx.style.top = e.clientY + 'px';
    ctx.style.display = 'block';
    document.onclick = hideCtx;
}

function onLineCtx(e) {
    e.preventDefault();
    e.stopPropagation();
    var line = this;
    ctxLnT = line;
    var cn = conns.find(function(c) { return c.line === line; });
    if (cn && ctxLn) {
        ctxLn.style.left = e.clientX + 'px';
        ctxLn.style.top = e.clientY + 'px';
        ctxLn.style.display = 'block';
        lnTypeInfo.textContent = cn.type;
    }
    document.onclick = hideCtxAll;
}

function hideCtx() { ctx.style.display = 'none'; }
function hideCtxLn() { ctxLn.style.display = 'none'; }
function hideCtxAll() {
    ctx.style.display = 'none';
    ctxLn.style.display = 'none';
    document.onclick = null;
}

// ===== ОБРАБОТКА КОНТЕКСТНОГО МЕНЮ =====
ctx.onclick = function(e) {
    e.stopPropagation();
    if (e.target.className.indexOf('btn-plus') >= 0 || e.target.className.indexOf('btn-minus') >= 0) {
        var a = e.target.getAttribute('data-a');
        var elId = ctxT && (ctxT.getAttribute('data-base-id') || ctxT.id);
        if (a === 'add-pl') { addPort(ctxT, 'pl', 'in'); if (window.Console) window.Console.cmd('addPort', elId + ' pl'); }
        if (a === 'add-pt') { addPort(ctxT, 'pt', 'in'); if (window.Console) window.Console.cmd('addPort', elId + ' pt'); }
        if (a === 'add-pr') { addPort(ctxT, 'pr', 'out'); if (window.Console) window.Console.cmd('addPort', elId + ' pr'); }
        if (a === 'add-pb') { addPort(ctxT, 'pb', 'out'); if (window.Console) window.Console.cmd('addPort', elId + ' pb'); }
        if (a === 'del-pl') { delPortByPos(ctxT, 'pl'); if (window.Console) window.Console.cmd('delPort', elId + ' pl'); }
        if (a === 'del-pt') { delPortByPos(ctxT, 'pt'); if (window.Console) window.Console.cmd('delPort', elId + ' pt'); }
        if (a === 'del-pr') { delPortByPos(ctxT, 'pr'); if (window.Console) window.Console.cmd('delPort', elId + ' pr'); }
        if (a === 'del-pb') { delPortByPos(ctxT, 'pb'); if (window.Console) window.Console.cmd('delPort', elId + ' pb'); }
        hideCtx();
    }
    if (e.target.className.indexOf('ctx-item') >= 0) {
        var a = e.target.getAttribute('data-a');
        if (a === 'dup') { dupEl(ctxT); }
        if (a === 'del' && ctxType === 'el') { delEl(ctxT); }
        if (a === 'del' && ctxType === 'port') { delPort(ctxT); }
        if (a === 'del-ln') { delLine(ctxLnT); }
        hideCtxAll();
    }
};

// Обработчик для контекстного меню линии
if (ctxLn) {
    ctxLn.onclick = function(e) {
        if (e.target && e.target.className && e.target.className.indexOf('ctx-item') >= 0) {
            var a = e.target.getAttribute('data-a');
            if (a === 'del-ln') {
                delLine(ctxLnT);
            }
            hideCtxAll();
        }
    };
}

// ===== УДАЛЕНИЕ И ДУБЛИРОВАНИЕ =====
function delEl(el) {
    // Получаем base-id для удаления парного элемента
    var baseId = el.getAttribute('data-base-id');

    // Удаляем все соединения элемента (только для рабочей области)
    if (baseId && baseId.indexOf('_ws') >= 0) {
        conns.slice().forEach(function(c) {
            if (c.from.closest('.element') === el || c.to.closest('.element') === el) {
                delLine(c.line);
            }
        });
    }

    // Удаляем подписи портов перед удалением элемента
    var labels = el.querySelectorAll('.port-label');
    for (var i = 0; i < labels.length; i++) {
        if (labels[i].parentNode) {
            labels[i].parentNode.removeChild(labels[i]);
        }
    }

    // Удаляем сам элемент
    if (el.parentNode) el.parentNode.removeChild(el);

    // Удаляем контрол из формы
    if (baseId) {
        var ctrl = document.getElementById(baseId + '_ctrl');
        if (ctrl && ctrl.parentNode) {
            ctrl.parentNode.removeChild(ctrl);
        }
    }

    sel(null);

    if (window.Console) {
        window.Console.cmd('delEl', (baseId || el.id) + ' removed');
    }
}

function dupEl(el) {
    var x = parseInt(el.style.left) || 0,
        y = parseInt(el.style.top) || 0;
    var type = el.getAttribute('data-type');
    if (window.Console) {
        window.Console.cmd('dupEl', type + ' from (' + x + ', ' + y + ')');
    }
    mkEl(type, x + 20, y + 20);
}

function delPort(p) {
    conns.slice().forEach(function(c) {
        if (c.from === p || c.to === p) { delLine(c.line); }
    });
    var nxt = p.nextSibling;
    if (nxt && nxt.className === 'port-label') nxt.parentNode.removeChild(nxt);
    if (p.parentNode) p.parentNode.removeChild(p);
}

function delPortByPos(el, pos) {
    var ports = el.querySelectorAll('.port.' + pos);
    if (ports.length > 0) { delPort(ports[ports.length - 1]); }
}

function delLineByPort(p) {
    conns.slice().forEach(function(c) {
        if (c.from === p || c.to === p) { delLine(c.line); }
    });
}

function delLine(ln) {
    var idx = conns.findIndex(function(c) { return c.line === ln; });
    if (idx >= 0) {
        var cn = conns[idx];
        var fromId = cn.fromId || '?';
        var toId = cn.toId || '?';
        cn.from.classList.remove('connected');
        cn.to.classList.remove('connected');
        if (selectedLine === ln) {
            selectedLine = null;
        }
        if (ln.parentNode) ln.parentNode.removeChild(ln);
        conns.splice(idx, 1);
        window.conns = conns;

        if (window.Console) {
            window.Console.cmd('delConn', cn.id + ' (' + fromId + ' -> ' + toId + ')');
        }
    }
}

// ===== СВОЙСТВА =====
function updProps(el) {
    if (!el) return;
    
    document.getElementById('pName').value = el.getAttribute('data-caption') || el.getAttribute('data-type') || '';
    document.getElementById('pLeft').value = parseInt(el.style.left) || 0;
    document.getElementById('pTop').value = parseInt(el.style.top) || 0;
    document.getElementById('pWidth').value = el.offsetWidth;
    document.getElementById('pHeight').value = el.offsetHeight;
    document.getElementById('pType').value = el.getAttribute('data-type') || '';
    document.getElementById('pId').value = el.id;
    document.getElementById('pEv').value = el.getAttribute('data-ev') || 0;
    
    if (ctxType === 'port' && ctxT) {
        document.getElementById('pId').value = ctxT.getAttribute('data-port-id');
    }

    // Индикатор наличия кода
    var code = el.getAttribute('data-code') || '';
    var btnEditCode = document.getElementById('btnEditCode');
    if (btnEditCode) {
        if (code.length > 0) {
            btnEditCode.textContent = '✏️ Код (' + code.length + ' зн.)';
            btnEditCode.style.borderColor = 'var(--accent-cyan)';
        } else {
            btnEditCode.textContent = '✏️ Редактировать';
            btnEditCode.style.borderColor = 'var(--border-color)';
        }
    }

    updPortInfo(el);
}

// ===== ИЗМЕНЕНИЕ СВОЙСТВ =====
// Caption
document.getElementById('pName').onchange = function() {
    var el = selEl;
    if (!el) return;
    var old = el.getAttribute('data-caption');
    el.setAttribute('data-caption', this.value);
    var hdr = el.querySelector('.el-hdr');
    if (hdr) hdr.textContent = this.value;
    if (window.Console) {
        window.Console.cmd('setProp', el.id + ' caption "' + (old || '') + '" -> "' + this.value + '"');
    }
};

// Left
document.getElementById('pLeft').onchange = function() {
    var el = selEl;
    if (!el) return;
    el.style.left = this.value + 'px';
    if (window.Console) {
        window.Console.cmd('setProp', el.id + ' left = ' + this.value);
    }
};

// Top
document.getElementById('pTop').onchange = function() {
    var el = selEl;
    if (!el) return;
    el.style.top = this.value + 'px';
    if (window.Console) {
        window.Console.cmd('setProp', el.id + ' top = ' + this.value);
    }
};

// Width
document.getElementById('pWidth').onchange = function() {
    var el = selEl;
    if (!el) return;
    el.style.minWidth = this.value + 'px';
    el.style.width = this.value + 'px';
    if (window.Console) {
        window.Console.cmd('setProp', el.id + ' width = ' + this.value);
    }
};

// Height
document.getElementById('pHeight').onchange = function() {
    var el = selEl;
    if (!el) return;
    el.style.minHeight = this.value + 'px';
    el.style.height = this.value + 'px';
    if (window.Console) {
        window.Console.cmd('setProp', el.id + ' height = ' + this.value);
    }
};

function clrProps() {
    document.getElementById('pName').value = '';
    document.getElementById('pLeft').value = '';
    document.getElementById('pTop').value = '';
    document.getElementById('pWidth').value = '';
    document.getElementById('pHeight').value = '';
    document.getElementById('pType').value = '';
    document.getElementById('pId').value = '';
    document.getElementById('pEv').value = '';
    clrPortInfo();
}

// ===== ПОРТЫ (инфо в панели свойств) =====
var PORT_DESC = {
    pl: { label: 'in', cls: 'in',    desc: 'Вход — принимает вызовы и сигналы' },
    pr: { label: 'out',cls: 'out',   desc: 'Выход — исходящие события и данные' },
    pt: { label: 'id', cls: 'id',     desc: 'Идентификатор — ключи и ссылки' },
    pb: { label: 'data',cls: 'data',  desc: 'Свойство — конфигурация и состояние' }
};

function updPortInfo(el) {
    var cont = document.getElementById('portInfoList');
    if (!cont) return;
    var ports = el.querySelectorAll('.port');
    if (ports.length === 0) {
        cont.innerHTML = '<div class="port-info-item" style="color:var(--text-dim);font-size:10px;padding:8px 12px;">Нет портов</div>';
        return;
    }
    var html = '';
    ports.forEach(function(p) {
        var cls = p.className;
        var type = 'pl';
        if (cls.indexOf('pr') >= 0) type = 'pr';
        else if (cls.indexOf('pt') >= 0) type = 'pt';
        else if (cls.indexOf('pb') >= 0) type = 'pb';
        var info = PORT_DESC[type] || { label: '?', cls: 'in', desc: 'Неизвестный тип' };
        var portId = p.getAttribute('data-port') || p.getAttribute('data-port-id') || '?';
        var connected = p.classList.contains('connected');
        html += '<div class="port-info-item">'
            + '<span class="port-badge ' + info.cls + '">' + info.label + '</span>'
            + '<span class="port-label">' + portId + '</span>'
            + '<span class="port-desc">' + info.desc + '</span>'
            + '<span class="port-connected ' + (connected ? 'yes' : 'no') + '">'
            + (connected ? '✓' : '○') + '</span>'
            + '</div>';
    });
    cont.innerHTML = html;
}

function clrPortInfo() {
    var cont = document.getElementById('portInfoList');
    if (cont) cont.innerHTML = '';
}

cvs.onclick = function() {
    sel(null);
    hideCtxAll();
    // Сбрасываем выделение линии
    if (selectedLine) {
        selectedLine.classList.remove('selected');
        selectedLine = null;
    }
};

// ===== ГОРЯЧИЕ КЛАВИШИ =====
document.addEventListener('keydown', function(e) {
    if (e.key === 'Delete' || e.keyCode === 46) {
        // Если есть выделенные элементы
        if (selectedElements.length > 0) {
            // Копируем массив, чтобы избежать проблем при удалении
            selectedElements.slice().forEach(function(el) { delEl(el); });
            selectedElements = [];
        } else if (selEl) {
            delEl(selEl);
            selEl = null;
        } else if (selectedLine) {
            delLine(selectedLine);
            selectedLine = null;
        }
    }
    if (e.key === 'Escape' || e.keyCode === 27) {
        sel(null);
        selectedLine = null;
        hideCtxAll();
    }
    if (e.ctrlKey && (e.key === 'a' || e.keyCode === 65)) {
        e.preventDefault();
        var allEls = [];
        getCurrentEC().querySelectorAll('.element').forEach(function(el) { allEls.push(el); });
        sel(allEls);
    }
});

// ===== КНОПКИ ПАНЕЛИ ИНСТРУМЕНТОВ =====
// Используем Compiler из compiler.js
document.getElementById('btnCompile').onclick = function() {
    var project = Compiler.compile();
    var json = JSON.stringify(project, null, 2);
    compileOutput.value = json;
    compileElCnt.textContent = project.elements.length;
    compileConnCnt.textContent = project.connections.length;
    compileModal.style.display = 'block';
    overlay.style.display = 'block';
    if (window.Console) {
        window.Console.cmd('compile', project.elements.length + ' elements, ' + project.connections.length + ' connections');
    }
};

document.getElementById('btnCloseModal').onclick = function() {
    compileModal.style.display = 'none';
    overlay.style.display = 'none';
};

document.getElementById('btnCopyJson').onclick = function() {
    Compiler.copy();
    var btn = document.getElementById('btnCopyJson');
    var origText = btn.textContent;
    btn.textContent = '✅ Скопировано!';
    setTimeout(function() { btn.textContent = origText; }, 1500);
};

document.getElementById('btnDownloadJson').onclick = function() {
    Compiler.save();
};

// ===== ОТКРЫТЬ ПРОЕКТ =====
document.getElementById('btnOpen').onclick = function() {
    Compiler.openFile();
};

// ===== ИМПОРТ ПРОЕКТА =====
document.getElementById('btnImport').onclick = function() {
    Compiler.openFile();
};

// ===== РЕДАКТОР ФОРМ =====
document.getElementById('btnForm').onclick = toggleFormEditor;

// ===== СОХРАНИТЬ ПРОЕКТ =====
document.getElementById('btnSave').onclick = function() {
    Compiler.save();
};

// ===== РЕДАКТОР КОДА =====
var currentSourceEl = null;

// Генерация шаблонного кода для элемента
function generateTemplate(el) {
    if (!el) return '';
    
    var type = el.getAttribute('data-type');
    var id = el.id;
    
    // Считаем порты по сторонам
    var ports = {
        methods: [],    // pl (left) - методы
        events: [],     // pr (right) - события
        data: [],       // pt (top) - данные
        properties: []  // pb (bottom) - свойства
    };
    
    el.querySelectorAll('.port').forEach(function(p, idx) {
        var pos = p.className.indexOf('pl') >= 0 ? 'methods' :
                  p.className.indexOf('pr') >= 0 ? 'events' :
                  p.className.indexOf('pt') >= 0 ? 'data' : 'properties';
        var portNum = ports[pos].length;
        ports[pos].push({
            id: p.getAttribute('data-port-id'),
            name: type + '_' + pos.slice(0, 3) + portNum,
            num: portNum
        });
    });
    
    // Генерируем шаблон
    var template = '; Element IDE - FASM Template\n';
    template += '; Элемент: ' + id + ' (' + type + ')\n\n';
    
    // Методы (вход слева)
    if (ports.methods.length > 0) {
        template += '; ===== МЕТОДЫ (вход) =====\n';
        ports.methods.forEach(function(p) {
            template += '; ' + p.name + ' - порт #' + p.num + '\n';
            template += p.name + ':\n';
            template += '    ; Код метода\n';
            template += '    ret\n\n';
        });
    }
    
    // События (выход справа)
    if (ports.events.length > 0) {
        template += '; ===== СОБЫТИЯ (выход) =====\n';
        ports.events.forEach(function(p) {
            template += '; ' + p.name + ' - порт #' + p.num + '\n';
            template += 'invoke ' + p.name + '  ; Вызов события\n\n';
        });
    }
    
    // Данные (вход сверху)
    if (ports.data.length > 0) {
        template += '; ===== ДАННЫЕ (вход) =====\n';
        ports.data.forEach(function(p) {
            template += p.name + ' dd ?  ; Данные #' + p.num + '\n';
        });
        template += '\n';
    }
    
    // Свойства (вход снизу)
    if (ports.properties.length > 0) {
        template += '; ===== СВОЙСТВА (вход) =====\n';
        ports.properties.forEach(function(p) {
            template += p.name + ' dd ?  ; Свойство #' + p.num + '\n';
        });
        template += '\n';
    }
    
    return template;
}

// Функция обновления подсветки
function updateHighlight() {
    var editor = document.getElementById('sourceCodeEditor');
    var highlight = document.getElementById('sourceCodeHighlight');
    if (!editor || !highlight || !window.FasmHighlighter) return;
    
    var code = editor.value;
    highlight.innerHTML = window.FasmHighlighter.highlight(code);
    
    // Синхронизация скролла
    highlight.scrollTop = editor.scrollTop;
    highlight.scrollLeft = editor.scrollLeft;
}

// Открытие редактора по кнопке в панели свойств
document.getElementById('btnEditCode') && (document.getElementById('btnEditCode').onclick = function() {
    openElementSourceCode();
});

// Кнопка "Код" - открывает код всего проекта
document.getElementById('btnSource').onclick = function() {
    openProjectCode();
};

function openProjectCode() {
    var ec = document.getElementById('ec');
    if (!ec) return;

    var elements = ec.querySelectorAll('.element[id$="_ws"]');
    if (elements.length === 0) {
        alert('Нет элементов в проекте');
        return;
    }

    // Формируем код проекта - все элементы с их кодом
    var projectCode = '// ===== PROJECT CODE =====\n';
    projectCode += '// Elements: ' + elements.length + '\n\n';

    elements.forEach(function(el, index) {
        var type = el.getAttribute('data-type');
        var caption = el.getAttribute('data-caption');
        var id = el.id;
        var code = el.getAttribute('data-code') || '';

        projectCode += '// -------------------------------------------\n';
        projectCode += '// Element ' + (index + 1) + ': ' + id + ' (' + type + ')\n';
        projectCode += '// Caption: ' + caption + '\n';
        projectCode += '// Position: (' + (el.style.left || 0) + ', ' + (el.style.top || 0) + ')\n';
        projectCode += '// -------------------------------------------\n\n';

        if (code) {
            projectCode += code + '\n\n';
        } else {
            // Генерируем шаблон если код пустой
            var template = generateTemplate(el);
            projectCode += template + '\n\n';
        }
    });

    projectCode += '// ===== END OF PROJECT CODE =====\n';

    // Показываем в модальном окне
    currentSourceEl = null;
    sourceElId.textContent = 'Проект (' + elements.length + ' эл.)';
    sourceElType.textContent = 'project';
    sourceCodeEditor.value = projectCode;
    updateHighlight();
    sourceModal.style.display = 'block';
    overlay.style.display = 'block';
    sourceStatus.textContent = '';
}

// Кнопка "Код элемента" - для панели свойств
function openElementSourceCode() {
    if (!selEl) {
        alert('Сначала выберите элемент!');
        return;
    }
    currentSourceEl = selEl;
    sourceElId.textContent = selEl.id;
    sourceElType.textContent = selEl.getAttribute('data-type');

    // Загружаем код или генерируем шаблон
    var code = selEl.getAttribute('data-code') || '';
    if (code === '') {
        code = generateTemplate(selEl);
    }

    sourceCodeEditor.value = code;
    updateHighlight();
    sourceModal.style.display = 'block';
    overlay.style.display = 'block';
    sourceStatus.textContent = '';
}


// Синхронизация скролла и подсветка при вводе
setTimeout(function() {
    var editor = document.getElementById('sourceCodeEditor');
    var highlight = document.getElementById('sourceCodeHighlight');
    if (editor && highlight) {
        editor.addEventListener('scroll', function() {
            highlight.scrollTop = editor.scrollTop;
            highlight.scrollLeft = editor.scrollLeft;
        });
        editor.addEventListener('input', updateHighlight);
        editor.addEventListener('keydown', function() {
            setTimeout(updateHighlight, 0);
        });
    }
}, 100);

document.getElementById('btnCloseSource').onclick = function() {
    sourceModal.style.display = 'none';
    overlay.style.display = 'none';
    currentSourceEl = null;
};

document.getElementById('btnSaveSource').onclick = function() {
    if (!currentSourceEl) {
        sourceStatus.textContent = '❌ Ошибка: элемент не выбран';
        return;
    }
    var code = sourceCodeEditor.value;
    currentSourceEl.setAttribute('data-code', code);
    sourceStatus.textContent = '✅ Код сохранён!';
    if (window.Console) {
        window.Console.cmd('setCode', currentSourceEl.id + ' (' + code.length + ' chars)');
    }
    setTimeout(function() { sourceStatus.textContent = ''; }, 2000);
};

document.getElementById('btnClearSource').onclick = function() {
    sourceCodeEditor.value = '';
    if (currentSourceEl) {
        currentSourceEl.setAttribute('data-code', '');
        sourceStatus.textContent = '✅ Код очищен!';
        setTimeout(function() { sourceStatus.textContent = ''; }, 2000);
    }
};

// Генерация шаблона по кнопке
document.getElementById('btnGenerateTemplate') && (document.getElementById('btnGenerateTemplate').onclick = function() {
    if (!currentSourceEl) {
        sourceStatus.textContent = '❌ Ошибка: элемент не выбран';
        return;
    }
    var code = generateTemplate(currentSourceEl);
    sourceCodeEditor.value = code;
    updateHighlight();
    sourceStatus.textContent = '✅ Шаблон сгенерирован!';
    setTimeout(function() { sourceStatus.textContent = ''; }, 2000);
});

// ===== ЗАКРЫТИЕ МОДАЛОК =====
overlay.onclick = function() {
    compileModal.style.display = 'none';
    sourceModal.style.display = 'none';
    overlay.style.display = 'none';
};

// ===== СВОРАЧИВАНИЕ ЛЕВОЙ ПАНЕЛИ =====
var toggleLeft = document.getElementById('toggleLeft');
var leftPanel = document.getElementById('leftPanel');
if (toggleLeft && leftPanel) {
    toggleLeft.onclick = function() {
        leftPanel.classList.toggle('collapsed');
        // Меняем направление стрелки
        this.textContent = leftPanel.classList.contains('collapsed') ? '▸' : '◂';
        // Перерисовываем сетку после анимации
        setTimeout(resize, 300);
    };
}

// ===== ЗАГРУЗКА ПРОЕКТА =====
function loadProject(project) {
    // Используем Compiler.apply для применения проекта
    Compiler.apply(project);
}

// Экспорт функций для compiler.js и elements.js
window.mkEl = mkEl;
window.mkConn = mkConn;
window.updLines = updLines;
window.onElDown = onElDown;
window.onElDbl = onElDbl;
window.onElCtx = onElCtx;
window.onPortDown = onPortDown;
window.onPortUp = onPortUp;
window.onPortCtx = onPortCtx;
window.onLineCtx = onLineCtx;
window.sel = sel;
window.delLine = delLine;
window.delLineByPort = delLineByPort;
window.onControlDown = onControlDown;
window.createDefaultForm = createDefaultForm;

// Для database.js - открытие редактора кода по типу элемента
window.openSourceForType = function(type) {
    currentSourceEl = null;
    sourceElId.textContent = 'Шаблон: ' + type;
    sourceElType.textContent = type;

    // Загружаем код из шаблона
    var code = '';
    if (window.ElementTemplates) {
        code = window.ElementTemplates.getCodeTemplate(type) || '';
    }

    sourceCodeEditor.value = code;
    updateHighlight();
    sourceModal.style.display = 'block';
    overlay.style.display = 'block';
    sourceStatus.textContent = '';
};

// Кнопка центрирования элементов
var btnCenter = document.getElementById('btnCenter');
if (btnCenter) {
    btnCenter.onclick = function() {
        var ec = document.getElementById('ec');
        if (!ec) return;

        var elements = ec.querySelectorAll('.element');
        if (elements.length === 0) {
            if (window.Console) window.Console.log('Нет элементов', 'info');
            return;
        }

        // Размещаем элементы лесенкой начиная с (50, 50)
        var x = 50, y = 50;
        elements.forEach(function(el, i) {
            el.style.left = (x + (i * 30)) + 'px';
            el.style.top = (y + (i * 30)) + 'px';

            // Синхронизируем с парным элементом в форме
            var baseId = el.getAttribute('data-base-id');
            if (baseId) {
                var pair = document.querySelector('[data-base-id="' + baseId + '"]:not(#' + el.id + ')');
                if (pair) {
                    pair.style.left = el.style.left;
                    pair.style.top = el.style.top;
                }
            }
        });

        if (window.Console) {
            window.Console.success('Элементы размещены в центре: ' + elements.length);
        }

        // Сбрасываем панорамирование
        panX = 0;
        panY = 0;
        ec.style.transform = 'translate(0px,0px)';
        svg.style.transform = 'translate(0px,0px)';
    };
}

// Для elements.js
window.createElementFromPalette = function(type, x, y) {
    mkEl(type, x, y);
};

})();
