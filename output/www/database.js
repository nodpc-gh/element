// Element IDE - Database Viewer Module
// Просмотр таблицы элементов базы данных

(function() {
'use strict';

var databaseData = null;
var currentView = 'elements';

// ===== ИНИЦИАЛИЗАЦИЯ =====
function init() {
    loadDatabase();
    setupEventListeners();
}

// ===== ЗАГРУЗКА БАЗЫ ДАННЫХ =====
function loadDatabase() {
    // Загружаем из /api/elements (основной endpoint)
    fetch('/api/elements')
        .then(function(response) {
            if (!response.ok) {
                throw new Error('HTTP error: ' + response.status);
            }
            return response.json();
        })
        .then(function(data) {
            // Формируем полную структуру БД
            var elements = data.elements || data || [];
            databaseData = {
                database: 'Element IDE Components Database',
                version: '2.0',
                elements: elements,
                categories: [
                    {id:'system',name:'System',icon:'⚙️'},
                    {id:'network',name:'Network',icon:'🌐'},
                    {id:'logic',name:'Logic',icon:'🔷'},
                    {id:'controls',name:'Controls',icon:'🎛️'}
                ]
            };
            renderDatabaseTable();
            renderCategories();
            renderStructure();
        })
        .catch(function(err) {
            console.error('Error loading from /api/elements:', err);
            useFallbackDatabase();
        });
}

// ===== ЗАПАСНАЯ БАЗА ДАННЫХ =====
function useFallbackDatabase() {
    databaseData = {
        database: 'Element IDE Components Database',
        version: '2.0',
        elements: [
            {id:'comp_console',name:'Console',category:'system',icon:'C',type:'console'},
            {id:'comp_service',name:'Service',category:'system',icon:'S',type:'service'},
            {id:'comp_opencode',name:'OpenCode',category:'system',icon:'🤖',type:'opencode'},
            {id:'comp_http_server',name:'HTTP Server',category:'network',icon:'H',type:'http_server'},
            {id:'comp_tcp_server',name:'TCP Server',category:'network',icon:'T',type:'tcp_server'},
            {id:'comp_and',name:'AND',category:'logic',icon:'A',type:'and_gate'},
            {id:'comp_or',name:'OR',category:'logic',icon:'O',type:'or_gate'},
            {id:'comp_not',name:'NOT',category:'logic',icon:'N',type:'not_gate'},
            {id:'comp_xor',name:'XOR',category:'logic',icon:'X',type:'xor_gate'},
            {id:'comp_btn',name:'Button',category:'controls',icon:'B',type:'btn'},
            {id:'comp_lbl',name:'Label',category:'controls',icon:'L',type:'lbl'},
            {id:'comp_edt',name:'Edit',category:'controls',icon:'E',type:'edt'},
            {id:'comp_chk',name:'CheckBox',category:'controls',icon:'K',type:'chk'},
            {id:'comp_pnl',name:'Panel',category:'controls',icon:'P',type:'pnl'}
        ],
        categories: [
            {id:'system',name:'System',icon:'⚙️'},
            {id:'network',name:'Network',icon:'🌐'},
            {id:'logic',name:'Logic',icon:'🔷'},
            {id:'controls',name:'Controls',icon:'🎛️'}
        ]
    };
    renderDatabaseTable();
    renderCategories();
}

// ===== НАСТРОЙКА СОБЫТИЙ =====
function setupEventListeners() {
    // Кнопка просмотра БД
    var btnDatabase = document.getElementById('btnDatabase');
    if (!btnDatabase) {
        // Создаём кнопку если нет
        var toolbar = document.querySelector('.toolbar');
        if (toolbar) {
            btnDatabase = document.createElement('button');
            btnDatabase.id = 'btnDatabase';
            btnDatabase.title = 'База данных';
            btnDatabase.textContent = '🗄️';
            toolbar.insertBefore(btnDatabase, toolbar.firstChild);
            btnDatabase.onclick = toggleDatabaseViewer;
        }
    } else {
        btnDatabase.onclick = toggleDatabaseViewer;
    }

    // Закрытие модального окна
    var btnCloseDb = document.getElementById('btnCloseDatabase');
    if (btnCloseDb) {
        btnCloseDb.onclick = closeDatabaseViewer;
    }

    var overlay = document.getElementById('overlay');
    if (overlay) {
        overlay.addEventListener('click', function() {
            closeDatabaseViewer();
        });
    }

    // Вкладки в просмотре БД
    document.addEventListener('click', function(e) {
        if (e.target.classList.contains('db-tab')) {
            var tab = e.target;
            document.querySelectorAll('.db-tab').forEach(function(t) {
                t.classList.remove('active');
            });
            tab.classList.add('active');
            currentView = tab.getAttribute('data-view');
            switchView(currentView);
        }
    });

    // Поиск по элементам
    var searchInput = document.getElementById('dbSearch');
    if (searchInput) {
        searchInput.addEventListener('input', function() {
            filterElements(this.value);
        });
    }
}

// ===== ОТКРЫТЬ/ЗАКРЫТЬ ПРОСМОТР БД =====
function toggleDatabaseViewer() {
    var modal = document.getElementById('databaseModal');
    if (modal) {
        modal.style.display = 'block';
        document.getElementById('overlay').style.display = 'block';
        loadDatabase();
    }
}

function closeDatabaseViewer() {
    var modal = document.getElementById('databaseModal');
    if (modal) {
        modal.style.display = 'none';
        document.getElementById('overlay').style.display = 'none';
    }
}

// ===== ПЕРЕКЛЮЧЕНИЕ ВИДОВ =====
function switchView(view) {
    document.getElementById('dbElementsView').style.display = (view === 'elements') ? 'block' : 'none';
    document.getElementById('dbCategoriesView').style.display = (view === 'categories') ? 'block' : 'none';
    document.getElementById('dbStructureView').style.display = (view === 'structure') ? 'block' : 'none';
}

// ===== РЕНДЕР ТАБЛИЦЫ ЭЛЕМЕНТОВ =====
function renderDatabaseTable() {
    var tbody = document.getElementById('dbElementsBody');
    if (!tbody || !databaseData) return;

    tbody.innerHTML = '';

    databaseData.elements.forEach(function(el, index) {
        var row = document.createElement('tr');
        row.className = 'db-row';
        row.setAttribute('data-index', index);
        row.onclick = function() { selectElement(index); };

        var ports = el.defaultPorts || el.ports || [];
        var portsHtml = ports.map(function(p) {
            return '<span class="port-badge port-' + p + '">' + p + '</span>';
        }).join('');

        row.innerHTML = 
            '<td class="db-cell">' + (el.icon || '?') + '</td>' +
            '<td class="db-cell db-name">' + el.name + '</td>' +
            '<td class="db-cell">' + el.type + '</td>' +
            '<td class="db-cell">' + el.category + '</td>' +
            '<td class="db-cell db-ports">' + portsHtml + '</td>' +
            '<td class="db-cell db-code-btn"><button class="btn-view-code" data-type="' + el.type + '">📝 Код</button></td>';

        tbody.appendChild(row);
    });

    // Обновляем счётчик
    var countEl = document.getElementById('dbElementCount');
    if (countEl) {
        countEl.textContent = databaseData.elements.length;
    }
}

// ===== РЕНДЕР КАТЕГОРИЙ =====
function renderCategories() {
    var container = document.getElementById('dbCategoriesList');
    if (!container || !databaseData) return;

    container.innerHTML = '';

    var categories = databaseData.categories || [];
    categories.forEach(function(cat) {
        var elementsInCategory = databaseData.elements.filter(function(el) {
            return el.category === cat.id;
        });

        var catDiv = document.createElement('div');
        catDiv.className = 'db-category-item';
        catDiv.innerHTML = 
            '<div class="db-cat-header">' +
                '<span class="db-cat-icon">' + (cat.icon || '📁') + '</span>' +
                '<span class="db-cat-name">' + cat.name + '</span>' +
                '<span class="db-cat-count">' + elementsInCategory.length + '</span>' +
            '</div>' +
            '<div class="db-cat-elements">' +
                elementsInCategory.map(function(el) {
                    return '<div class="db-cat-element" data-type="' + el.type + '">' + el.name + '</div>';
                }).join('') +
            '</div>';

        // Клик на элемент категории
        catDiv.querySelectorAll('.db-cat-element').forEach(function(elDiv) {
            elDiv.onclick = function() {
                var type = this.getAttribute('data-type');
                viewElementCode(type);
            };
        });

        container.appendChild(catDiv);
    });
}

// ===== ВЫДЕЛЕНИЕ ЭЛЕМЕНТА =====
function selectElement(index) {
    document.querySelectorAll('.db-row').forEach(function(row) {
        row.classList.remove('selected');
    });

    var row = document.querySelector('.db-row[data-index="' + index + '"]');
    if (row) {
        row.classList.add('selected');

        var el = databaseData.elements[index];
        showElementDetails(el);
    }
}

// ===== ПОКАЗАТЬ ДЕТАЛИ ЭЛЕМЕНТА =====
function showElementDetails(el) {
    var details = document.getElementById('dbElementDetails');
    if (!details) return;

    var ports = el.defaultPorts || el.ports || [];
    var portsHtml = ports.map(function(p) {
        return '<span class="port-badge port-' + p + '">' + p + '</span>';
    }).join('');

    var propertiesHtml = '';
    if (el.properties) {
        Object.keys(el.properties).forEach(function(key) {
            propertiesHtml += '<div class="db-prop-row"><span class="db-prop-name">' + key + '</span><span class="db-prop-value">' + el.properties[key] + '</span></div>';
        });
    }

    details.innerHTML = 
        '<div class="db-detail-header">' +
            '<span class="db-detail-icon">' + (el.icon || '📦') + '</span>' +
            '<div class="db-detail-info">' +
                '<h3>' + el.name + '</h3>' +
                '<span class="db-detail-type">' + el.type + '</span>' +
            '</div>' +
        '</div>' +
        '<div class="db-detail-section">' +
            '<h4>Порты</h4>' +
            '<div class="db-ports-list">' + portsHtml + '</div>' +
        '</div>' +
        '<div class="db-detail-section">' +
            '<h4>Свойства</h4>' +
            propertiesHtml +
        '</div>' +
        '<div class="db-detail-section">' +
            '<button class="btn-view-source" data-type="' + el.type + '">📝 Просмотреть код</button>' +
        '</div>';

    // Обработчик просмотра кода
    var btnViewSource = details.querySelector('.btn-view-source');
    if (btnViewSource) {
        btnViewSource.onclick = function() {
            viewElementCode(el.type);
        };
    }
}

// ===== ФИЛЬТРАЦИЯ ЭЛЕМЕНТОВ =====
function filterElements(query) {
    query = query.toLowerCase();
    document.querySelectorAll('.db-row').forEach(function(row) {
        var name = row.querySelector('.db-name');
        var type = row.children[2];
        var category = row.children[3];
        
        if (name.textContent.toLowerCase().indexOf(query) >= 0 ||
            type.textContent.toLowerCase().indexOf(query) >= 0 ||
            category.textContent.toLowerCase().indexOf(query) >= 0) {
            row.style.display = '';
        } else {
            row.style.display = 'none';
        }
    });
}

// ===== ПРОСМОТР КОДА ЭЛЕМЕНТА =====
function viewElementCode(type) {
    closeDatabaseViewer();

    // Открываем редактор кода
    // Пробуем несколько способов
    function tryOpen() {
        if (window.SourceEditor && window.SourceEditor.openForType) {
            window.SourceEditor.openForType(type);
            return true;
        }
        // Если SourceEditor ещё не готов, используем app.js функции
        if (window.openSourceForType) {
            window.openSourceForType(type);
            return true;
        }
        return false;
    }

    if (!tryOpen()) {
        // Пробуем через небольшую задержку
        setTimeout(function() {
            if (!tryOpen()) {
                console.error('Редактор кода не доступен');
                alert('Редактор кода ещё не загружен. Попробуйте позже.');
            }
        }, 300);
    }
}

// ===== ОТРИСОВКА СТРУКТУРЫ БД =====
function renderStructure() {
    var pre = document.getElementById('dbStructureJson');
    if (!pre || !databaseData) return;
    
    pre.textContent = JSON.stringify(databaseData, null, 2);
}

// ===== ЭКСПОРТ =====
window.DatabaseViewer = {
    init: init,
    load: loadDatabase,
    open: toggleDatabaseViewer,
    close: closeDatabaseViewer,
    getData: function() { return databaseData; }
};

// Автозапуск
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', init);
} else {
    init();
}

})();
