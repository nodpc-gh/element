// Element IDE - Elements Module
// Загрузка элементов из API и отображение в левой панели

(function() {
'use strict';

var elementsData = [];
var currentCategory = 'system';

// ===== ЗАГРУЗКА ЭЛЕМЕНТОВ ИЗ API =====
function init() {
    loadElements();
}

function loadElements() {
    fetch('/api/elements')
        .then(function(response) { return response.json(); })
        .then(function(data) {
            // Получаем массив элементов
            if (data.elements && data.elements.length > 0) {
                elementsData = data.elements;
                renderElements();
                initTabs();
            } else {
                console.error('No elements in API response');
                useFallbackElements();
            }
        })
        .catch(function(err) {
            console.error('Error loading elements:', err);
            useFallbackElements();
        });
}

// ===== ЗАПАСНЫЕ ЭЛЕМЕНТЫ (если API недоступно) =====
function useFallbackElements() {
    elementsData = [
        {id:'console',name:'Console',category:'system',icon:'C',type:'console'},
        {id:'service',name:'Service',category:'system',icon:'S',type:'service'},
        {id:'http_server',name:'HTTP Server',category:'network',icon:'H',type:'http_server'},
        {id:'tcp_server',name:'TCP Server',category:'network',icon:'T',type:'tcp_server'},
        {id:'and_gate',name:'AND',category:'logic',icon:'A',type:'and_gate'},
        {id:'or_gate',name:'OR',category:'logic',icon:'O',type:'or_gate'},
        {id:'not_gate',name:'NOT',category:'logic',icon:'N',type:'not_gate'},
        {id:'xor_gate',name:'XOR',category:'logic',icon:'X',type:'xor_gate'},
        {id:'btn',name:'Button',category:'controls',icon:'B',type:'btn'},
        {id:'lbl',name:'Label',category:'controls',icon:'L',type:'lbl'},
        {id:'edt',name:'Edit',category:'controls',icon:'E',type:'edt'},
        {id:'chk',name:'CheckBox',category:'controls',icon:'K',type:'chk'},
        {id:'pnl',name:'Panel',category:'controls',icon:'P',type:'pnl'}
    ];
    renderElements();
    initTabs();
}

// ===== РЕНДЕРИНГ ЭЛЕМЕНТОВ =====
function renderElements() {
    // Очищаем все сетки
    clearGrid('elementsSystem');
    clearGrid('elementsNetwork');
    clearGrid('elementsLogic');
    clearGrid('elementsControls');
    
    // Рендерим каждый элемент в свою категорию
    elementsData.forEach(function(el) {
        var gridId = 'elements' + el.category.charAt(0).toUpperCase() + el.category.slice(1);
        var grid = document.getElementById(gridId);
        if (grid) {
            var item = document.createElement('div');
            item.className = 'element-item';
            item.setAttribute('draggable', 'true');
            item.setAttribute('data-type', el.type);
            item.setAttribute('data-category', el.category);
            item.innerHTML = '<span class="icon">' + (el.icon || '?') + '</span>' +
                           '<span class="name">' + el.name + '</span>';
            grid.appendChild(item);
        }
    });
}

function clearGrid(id) {
    var grid = document.getElementById(id);
    if (grid) {
        grid.innerHTML = '';
    }
}

// ===== ВКЛАДКИ КАТЕГОРИЙ =====
function initTabs() {
    var tabs = document.querySelectorAll('.category-tab');
    tabs.forEach(function(tab) {
        tab.addEventListener('click', function() {
            tabs.forEach(function(t) { t.classList.remove('active'); });
            this.classList.add('active');
            currentCategory = this.getAttribute('data-category');
            showCategory(currentCategory);
        });
    });
}

function showCategory(category) {
    document.getElementById('elementsSystem').style.display = 'none';
    document.getElementById('elementsNetwork').style.display = 'none';
    document.getElementById('elementsLogic').style.display = 'none';
    document.getElementById('elementsControls').style.display = 'none';
    
    var grid = document.getElementById('elements' + category.charAt(0).toUpperCase() + category.slice(1));
    if (grid) {
        grid.style.display = 'grid';
    }
}

// ===== ЭКСПОРТ =====
window.ElementsModule = {
    init: init,
    loadElements: loadElements,
    showCategory: showCategory
};

// Автозапуск
init();

})();
