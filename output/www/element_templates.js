// Element IDE - Visual Element Templates
// Визуальные свойства и JavaScript код для каждого типа элемента

(function() {
'use strict';

var elementTemplates = {
    // ===== CONTROLS =====
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
        jsCode: '// Button Element\n// Обработчик клика\n\nelement.addEventListener("click", function() {\n    console.log("Button clicked!");\n    // Отправить событие на выход\n    triggerPort("pr", "click");\n});',
        // HTML шаблон
        htmlTemplate: '<button class="visual-element visual-btn">{caption}</button>'
    },
    
    'lbl': {
        caption: 'Label',
        ports: ['pl', 'pr', 'pt', 'pb'],
        width: 80,
        height: 20,
        backgroundColor: 'transparent',
        borderColor: 'transparent',
        color: '#cccccc',
        properties: {
            'Caption': 'Label',
            'Width': '80',
            'Height': '20'
        },
        jsCode: '// Label Element\n// Отображение текста\n\nelement.textContent = properties.Caption || "Label";',
        htmlTemplate: '<span class="visual-element visual-lbl">{caption}</span>'
    },
    
    'edt': {
        caption: 'Edit',
        ports: ['pl', 'pr', 'pt', 'pb'],
        width: 150,
        height: 25,
        backgroundColor: '#2d2d2d',
        borderColor: '#3c3c3c',
        color: '#ffffff',
        properties: {
            'Text': '',
            'Width': '150',
            'Height': '25',
            'MaxLength': '256'
        },
        jsCode: '// Edit Element\n// Поле ввода текста\n\nvar input = document.createElement("input");\ninput.type = "text";\ninput.value = properties.Text || "";\ninput.maxLength = properties.MaxLength || 256;\ninput.style.width = "100%";\ninput.style.background = "transparent";\ninput.style.border = "none";\ninput.style.color = "#fff";\n\ninput.addEventListener("input", function() {\n    triggerPort("pr", "change", input.value);\n});\n\nelement.appendChild(input);',
        htmlTemplate: '<input type="text" class="visual-element visual-edt" value="{text}" />'
    },
    
    'chk': {
        caption: 'CheckBox',
        ports: ['pl', 'pr', 'pt', 'pb'],
        width: 100,
        height: 20,
        backgroundColor: 'transparent',
        borderColor: 'transparent',
        color: '#cccccc',
        properties: {
            'Caption': 'CheckBox',
            'Checked': '0'
        },
        jsCode: '// CheckBox Element\n// Флажок\n\nvar wrapper = document.createElement("label");\nwrapper.style.display = "flex";\nwrapper.style.alignItems = "center";\nwrapper.style.gap = "5px";\n\nvar checkbox = document.createElement("input");\ncheckbox.type = "checkbox";\ncheckbox.checked = properties.Checked === "1";\n\ncheckbox.addEventListener("change", function() {\n    triggerPort("pr", "change", checkbox.checked);\n});\n\nvar label = document.createElement("span");\nlabel.textContent = properties.Caption || "CheckBox";\n\nwrapper.appendChild(checkbox);\nwrapper.appendChild(label);\nelement.appendChild(wrapper);',
        htmlTemplate: '<label class="visual-element visual-chk"><input type="checkbox" /> {caption}</label>'
    },
    
    'pnl': {
        caption: 'Panel',
        ports: ['pl', 'pr', 'pt', 'pb'],
        width: 200,
        height: 100,
        backgroundColor: '#252526',
        borderColor: '#3c3c3c',
        color: '#cccccc',
        properties: {
            'Width': '200',
            'Height': '100',
            'BorderStyle': 'None'
        },
        jsCode: '// Panel Element\n// Панель-контейнер\n\nelement.style.background = "#252526";\nelement.style.border = "1px solid #3c3c3c";\nelement.style.borderRadius = "2px";\n\n// Панель может содержать другие элементы\nelement.classList.add("visual-panel");',
        htmlTemplate: '<div class="visual-element visual-pnl" style="width:{width}px;height:{height}px;"></div>'
    },
    
    // ===== SYSTEM =====
    'console': {
        caption: 'Console',
        ports: ['pl', 'pr', 'pt', 'pb'],
        width: 300,
        height: 200,
        backgroundColor: '#1e1e1e',
        borderColor: '#3c3c3c',
        color: '#00ff00',
        properties: {
            'BufferSize': '4096',
            'Encoding': 'UTF-8'
        },
        jsCode: '// Console Element\n// Консоль вывода\n\nvar consoleDiv = document.createElement("div");\nconsoleDiv.style.background = "#1e1e1e";\nconsoleDiv.style.color = "#00ff00";\nconsoleDiv.style.fontFamily = "Consolas, monospace";\nconsoleDiv.style.fontSize = "12px";\nconsoleDiv.style.padding = "10px";\nconsoleDiv.style.height = "100%";\nconsoleDiv.style.overflow = "auto";\nconsoleDiv.style.whiteSpace = "pre-wrap";\n\nwindow.consoleOutput = function(msg) {\n    consoleDiv.textContent += msg + "\\n";\n    consoleDiv.scrollTop = consoleDiv.scrollHeight;\n};\n\nelement.appendChild(consoleDiv);',
        htmlTemplate: '<div class="visual-element visual-console"></div>'
    },

    // ===== OPENCODE =====
    'opencode': {
        caption: 'OpenCode',
        ports: ['pl', 'pr', 'pt', 'pb'],
        width: 280,
        height: 180,
        backgroundColor: '#1a1a2e',
        borderColor: '#e94560',
        color: '#0f3460',
        properties: {
            'Agent': 'opencode',
            'Model': 'big-pickle',
            'Status': 'idle'
        },
        jsCode: '// OpenCode Agent Element\n// AI-агент для выполнения задач\n\nvar terminal = document.createElement("div");\nterminal.style.background = "#0d0d1a";\nterminal.style.color = "#00ff88";\nterminal.style.fontFamily = "Consolas, monospace";\nterminal.style.fontSize = "11px";\nterminal.style.padding = "8px";\nterminal.style.height = "100%";\nterminal.style.overflow = "auto";\nterminal.style.whiteSpace = "pre-wrap";\nterminal.style.border = "1px solid #e94560";\nterminal.style.borderRadius = "4px";\nterminal.textContent = "> OpenCode Agent ready\\n> Type a task or connect ports\\n";\n\nwindow.opencodeOutput = function(msg) {\n    terminal.textContent += "> " + msg + "\\n";\n    terminal.scrollTop = terminal.scrollHeight;\n};\n\nelement.appendChild(terminal);\n\n// Прослушивание входящих портов\nelement.addEventListener("port_in", function(data) {\n    terminal.textContent += "> received: " + JSON.stringify(data) + "\\n";\n});',
        htmlTemplate: '<div class="visual-element visual-opencode"><div class="opencode-term">> OpenCode Agent</div></div>'
    },
    
    // ===== NETWORK =====
    'http_server': {
        caption: 'HTTP Server',
        ports: ['pl', 'pr', 'pt', 'pb'],
        width: 150,
        height: 80,
        backgroundColor: '#2d3e2d',
        borderColor: '#4a7c4a',
        color: '#8fca42',
        properties: {
            'Port': '8080',
            'MaxConnections': '10'
        },
        jsCode: '// HTTP Server Element\n// Веб-сервер (симуляция)\n\nvar statusDiv = document.createElement("div");\nstatusDiv.style.padding = "10px";\nstatusDiv.innerHTML = "🌐 HTTP Server<br>Port: " + (properties.Port || "8080");\n\nelement.appendChild(statusDiv);\n\n// Симуляция запуска\ntriggerPort("pr", "started", { port: properties.Port });',
        htmlTemplate: '<div class="visual-element visual-http"></div>'
    },
    
    // ===== LOGIC =====
    'and_gate': {
        caption: 'AND',
        ports: ['pl', 'pr', 'pt', 'pb'],
        width: 60,
        height: 60,
        backgroundColor: '#2d2d3e',
        borderColor: '#4a4a7c',
        color: '#9b8fca',
        properties: {
            'Inputs': '2'
        },
        jsCode: '// AND Gate Element\n// Логическое И\n\nvar inputs = [];\nvar output = null;\n\nelement.addEventListener("port_in", function(data) {\n    inputs.push(data);\n    if (inputs.length >= 2) {\n        var result = inputs[0] && inputs[1];\n        triggerPort("pr", "result", result);\n        inputs = [];\n    }\n});',
        htmlTemplate: '<div class="visual-element visual-gate">AND</div>'
    },
    
    'or_gate': {
        caption: 'OR',
        ports: ['pl', 'pr', 'pt', 'pb'],
        width: 60,
        height: 60,
        backgroundColor: '#3e2d2d',
        borderColor: '#7c4a4a',
        color: '#ca8f8f',
        properties: {
            'Inputs': '2'
        },
        jsCode: '// OR Gate Element\n// Логическое ИЛИ\n\nvar inputs = [];\n\nelement.addEventListener("port_in", function(data) {\n    inputs.push(data);\n    if (inputs.length >= 2) {\n        var result = inputs[0] || inputs[1];\n        triggerPort("pr", "result", result);\n        inputs = [];\n    }\n});',
        htmlTemplate: '<div class="visual-element visual-gate">OR</div>'
    },
    
    'not_gate': {
        caption: 'NOT',
        ports: ['pl', 'pr', 'pt', 'pb'],
        width: 50,
        height: 50,
        backgroundColor: '#3e3e2d',
        borderColor: '#7c7c4a',
        color: '#caca8f',
        properties: {},
        jsCode: '// NOT Gate Element\n// Логическое НЕ\n\nelement.addEventListener("port_in", function(data) {\n    var result = !data;\n    triggerPort("pr", "result", result);\n});',
        htmlTemplate: '<div class="visual-element visual-gate">NOT</div>'
    },
    
    'xor_gate': {
        caption: 'XOR',
        ports: ['pl', 'pr', 'pt', 'pb'],
        width: 60,
        height: 60,
        backgroundColor: '#2d3e3e',
        borderColor: '#4a7c7c',
        color: '#8fcaca',
        properties: {
            'Inputs': '2'
        },
        jsCode: '// XOR Gate Element\n// Исключающее ИЛИ\n\nvar inputs = [];\n\nelement.addEventListener("port_in", function(data) {\n    inputs.push(data);\n    if (inputs.length >= 2) {\n        var result = (inputs[0] !== inputs[1]);\n        triggerPort("pr", "result", result);\n        inputs = [];\n    }\n});',
        htmlTemplate: '<div class="visual-element visual-gate">XOR</div>'
    },
    
    // ===== SYSTEM =====
    'service': {
        caption: 'Service',
        ports: ['pl', 'pr', 'pt', 'pb'],
        width: 120,
        height: 60,
        backgroundColor: '#2d2d2d',
        borderColor: '#555555',
        color: '#aaaaaa',
        properties: {
            'ServiceName': 'MyService',
            'StartupType': 'Auto'
        },
        jsCode: '// Service Element\n// Служба Windows (симуляция)\n\nvar statusDiv = document.createElement("div");\nstatusDiv.style.padding = "10px";\nstatusDiv.innerHTML = "⚙️ Service<br>" + (properties.ServiceName || "MyService");\n\nelement.appendChild(statusDiv);',
        htmlTemplate: '<div class="visual-element visual-service"></div>'
    },
    
    'tcp_server': {
        caption: 'TCP Server',
        ports: ['pl', 'pr', 'pt', 'pb'],
        width: 150,
        height: 80,
        backgroundColor: '#2d3e2d',
        borderColor: '#4a7c4a',
        color: '#8fca42',
        properties: {
            'Port': '9090',
            'MaxClients': '5'
        },
        jsCode: '// TCP Server Element\n// TCP сервер (симуляция)\n\nvar statusDiv = document.createElement("div");\nstatusDiv.style.padding = "10px";\nstatusDiv.innerHTML = "🔌 TCP Server<br>Port: " + (properties.Port || "9090");\n\nelement.appendChild(statusDiv);',
        htmlTemplate: '<div class="visual-element visual-tcp"></div>'
    }
};

// ===== ФУНКЦИИ =====

function getTemplate(type) {
    return elementTemplates[type] || null;
}

function getCaption(type) {
    var tmpl = elementTemplates[type];
    return tmpl ? tmpl.caption : type;
}

function getProperties(type) {
    var tmpl = elementTemplates[type];
    return tmpl ? tmpl.properties : {};
}

function getCodeTemplate(type) {
    var tmpl = elementTemplates[type];
    return tmpl ? tmpl.jsCode : '';
}

function getPorts(type) {
    var tmpl = elementTemplates[type];
    return tmpl ? tmpl.ports : ['pl', 'pr', 'pt', 'pb'];
}

function getDefaultSize(type) {
    var tmpl = elementTemplates[type];
    return tmpl ? { width: tmpl.width, height: tmpl.height } : { width: 100, height: 60 };
}

function getVisualStyle(type) {
    var tmpl = elementTemplates[type];
    if (!tmpl) return {};
    return {
        backgroundColor: tmpl.backgroundColor,
        borderColor: tmpl.borderColor,
        color: tmpl.color
    };
}

// ===== ЭКСПОРТ =====
window.ElementTemplates = {
    get: getTemplate,
    getCaption: getCaption,
    getProperties: getProperties,
    getCodeTemplate: getCodeTemplate,
    getPorts: getPorts,
    getDefaultSize: getDefaultSize,
    getVisualStyle: getVisualStyle
};

})();
