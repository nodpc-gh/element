// FASM Syntax Highlighter для Element IDE
// Подсветка синтаксиса ассемблера FASM

(function() {
'use strict';

// Ключевые слова FASM
var keywords = {
    instructions: [
        'mov', 'add', 'sub', 'mul', 'imul', 'div', 'idiv', 'inc', 'dec',
        'and', 'or', 'xor', 'not', 'neg', 'test',
        'jmp', 'je', 'jne', 'jz', 'jnz', 'ja', 'jb', 'jae', 'jbe', 'jg', 'jl', 'jge', 'jle',
        'call', 'ret', 'retn', 'retf', 'int', 'iret',
        'push', 'pop', 'pushad', 'popad', 'pushfd', 'popfd',
        'cmp', 'lea', 'nop', 'xchg', 'bswap',
        'movzx', 'movsx', 'cbw', 'cwde', 'cdq', 'cwd',
        'loop', 'loope', 'loopne', 'loopz', 'loopnz',
        'stosb', 'stosw', 'stosd', 'movsb', 'movsw', 'movsd', 'lodsb', 'lodsw', 'lodsd',
        'in', 'out', 'cli', 'sti', 'clc', 'stc', 'cmc',
        'shl', 'shr', 'sal', 'sar', 'rol', 'ror', 'rcl', 'rcr',
        'enter', 'leave',
        'invlpg', 'cpuid', 'rdtsc', 'rdmsr', 'wrmsr',
        'fwait', 'wait', 'finit', 'fninit',
        'fadd', 'fsub', 'fmul', 'fdiv', 'fcom', 'fcomp', 'fucom', 'fucomp',
        'fld', 'fst', 'fstp', 'fxch',
        'fsin', 'fcos', 'fsincos', 'ftan', 'fpatan', 'fptan',
        'fexp', 'f2xm1', 'fyl2x', 'fyl2xp1', 'fsqrt', 'frndint',
        'fabs', 'fchs', 'frndint', 'fprem', 'fprem1',
        'cmps', 'cmpsb', 'cmpsw', 'cmpsd', 'scas', 'scasb', 'scasw', 'scasd'
    ],
    directives: [
        'db', 'dw', 'dd', 'dq', 'dt', 'ddq', 'dy', 'dz',
        'resb', 'resw', 'resd', 'resq', 'rest', 'resdq', 'resy', 'resz',
        'use16', 'use32', 'use64',
        'align', 'even', 'odd',
        'org', 'section', 'segment',
        'extrn', 'public', 'include', 'includelib',
        'label', 'equ', '=','restruc',
        'if', 'else', 'endif', 'ifn', 'ifne', 'ife', 'ifl', 'ifb', 'ifidn', 'ifidni',
        'macro', 'endm', 'local', 'forward', 'reverse', 'common',
        'struct', 'ends', 'match', 'rept', 'endr', 'times', 'repeat', 'while', 'endw',
        'invoke', 'cinvoke', 'sinvoke', 'stdcall', 'cdecl', 'fastcall', 'thiscall',
        'proc', 'endp', 'arg', 'locals', 'endlocals',
        'data', 'code', 'enddata', 'endcode',
        'virtual', 'endv', 'file', 'display', 'err', 'assert',
        'format', 'entry', 'stack'
    ],
    registers: [
        'eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'esp', 'eip',
        'ax', 'bx', 'cx', 'dx', 'si', 'di', 'bp', 'sp', 'ip',
        'al', 'ah', 'bl', 'bh', 'cl', 'ch', 'dl', 'dh',
        'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15',
        'r8d', 'r9d', 'r10d', 'r11d', 'r12d', 'r13d', 'r14d', 'r15d',
        'r8w', 'r9w', 'r10w', 'r11w', 'r12w', 'r13w', 'r14w', 'r15w',
        'r8b', 'r9b', 'r10b', 'r11b', 'r12b', 'r13b', 'r14b', 'r15b',
        'cs', 'ds', 'es', 'fs', 'gs', 'ss',
        'st0', 'st1', 'st2', 'st3', 'st4', 'st5', 'st6', 'st7', 'st',
        'mm0', 'mm1', 'mm2', 'mm3', 'mm4', 'mm5', 'mm6', 'mm7',
        'xmm0', 'xmm1', 'xmm2', 'xmm3', 'xmm4', 'xmm5', 'xmm6', 'xmm7',
        'xmm8', 'xmm9', 'xmm10', 'xmm11', 'xmm12', 'xmm13', 'xmm14', 'xmm15',
        'cr0', 'cr2', 'cr3', 'cr4',
        'dr0', 'dr1', 'dr2', 'dr3', 'dr6', 'dr7'
    ],
    constants: [
        'true', 'false', 'NULL', 'null'
    ]
};

// Регулярные выражения для подсветки
var patterns = {
    comment: /;[^\n]*/g,
    string: /"(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*'/g,
    number: /\b(?:0[xX][0-9a-fA-F]+|[0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?)[hH]?\b/g,
    label: /^\s*([a-zA-Z_][a-zA-Z0-9_]*):/gm,
    directive: /\b(db|dw|dd|dq|dt|ddq|dy|dz|resb|resw|resd|resq|rest|resdq|resy|resz|align|even|odd|org|section|segment|extrn|public|include|includelib|label|equ|macro|endm|local|struct|ends|match|rept|endr|times|repeat|while|endw|invoke|proc|endp|arg|locals|endlocals|virtual|endv|file|display|err|assert|format|entry|stack)\b/gi,
    register: /\b(e?[abcd]x|[abcd]l|[abcd]h|e?[sd]i|e?[sb]p|e?ip|cs|ds|es|fs|gs|ss|st\d|mm\d|xmm\d|r\d+[bdw]?)\b/gi,
    keyword: /\b(mov|add|sub|mul|imul|div|idiv|inc|dec|and|or|xor|not|neg|test|jmp|je|jne|jz|jnz|ja|jb|jae|jbe|jg|jl|jge|jle|call|ret|retn|retf|int|iret|push|pop|pushad|popad|pushfd|popfd|cmp|lea|nop|xchg|bswap|movzx|movsx|cbw|cwde|cdq|cwd|loop|loope|loopne|loopz|loopnz|stosb|stosw|stosd|movsb|movsw|movsd|lodsb|lodsw|lodsd|in|out|cli|sti|clc|stc|cmc|shl|shr|sal|sar|rol|ror|rcl|rcr|enter|leave)\b/gi
};

// Цвета для подсветки (в стиле Visual Studio Dark)
var colors = {
    keyword: '#569cd6',      // Инструкции - синий
    directive: '#c586c0',    // Директивы - фиолетовый
    register: '#9cdcfe',     // Регистры - светло-голубой
    number: '#b5cea8',       // Числа - зелёный
    string: '#ce9178',       // Строки - оранжевый
    comment: '#6a9955',      // Комментарии - зелёный
    label: '#dcdcaa',        // Метки - жёлтый
    default: '#d4d4d4'       // По умолчанию - светло-серый
};

// Функция подсветки
function highlight(code) {
    if (!code) return '';
    
    // Разбиваем на токены
    var tokens = [];
    var pos = 0;
    
    // Копируем код для обработки
    var src = code;
    
    // Находим все совпадения
    var matches = [];
    
    // Комментарии
    var re = /;[^\n]*/g;
    var m;
    while ((m = re.exec(src)) !== null) {
        matches.push({ start: m.index, end: m.index + m[0].length, type: 'comment', text: m[0] });
    }
    
    // Строки
    re = /"(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*'/g;
    while ((m = re.exec(src)) !== null) {
        matches.push({ start: m.index, end: m.index + m[0].length, type: 'string', text: m[0] });
    }
    
    // Числа
    re = /\b(?:0[xX][0-9a-fA-F]+|[0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?)[hH]?\b/g;
    while ((m = re.exec(src)) !== null) {
        matches.push({ start: m.index, end: m.index + m[0].length, type: 'number', text: m[0] });
    }
    
    // Метки
    re = /^\s*([a-zA-Z_][a-zA-Z0-9_]*):/gm;
    while ((m = re.exec(src)) !== null) {
        matches.push({ start: m.index, end: m.index + m[0].length, type: 'label', text: m[0] });
    }
    
    // Директивы
    re = /\b(db|dw|dd|dq|dt|ddq|dy|dz|resb|resw|resd|resq|rest|resdq|resy|resz|align|even|odd|org|section|segment|extrn|public|include|includelib|label|equ|macro|endm|local|struct|ends|match|rept|endr|times|repeat|while|endw|invoke|proc|endp|arg|locals|endlocals|virtual|endv|file|display|err|assert|format|entry|stack)\b/gi;
    while ((m = re.exec(src)) !== null) {
        matches.push({ start: m.index, end: m.index + m[0].length, type: 'directive', text: m[0] });
    }
    
    // Регистры
    re = /\b(e?[abcd]x|[abcd]l|[abcd]h|e?[sd]i|e?[sb]p|e?ip|cs|ds|es|fs|gs|ss|st\d|mm\d|xmm\d|r\d+[bdw]?)\b/gi;
    while ((m = re.exec(src)) !== null) {
        matches.push({ start: m.index, end: m.index + m[0].length, type: 'register', text: m[0] });
    }
    
    // Инструкции
    re = /\b(mov|add|sub|mul|imul|div|idiv|inc|dec|and|or|xor|not|neg|test|jmp|je|jne|jz|jnz|ja|jb|jae|jbe|jg|jl|jge|jle|call|ret|retn|retf|int|iret|push|pop|pushad|popad|pushfd|popfd|cmp|lea|nop|xchg|bswap|movzx|movsx|cbw|cwde|cdq|cwd|loop|loope|loopne|loopz|loopnz|stosb|stosw|stosd|movsb|movsw|movsd|lodsb|lodsw|lodsd|in|out|cli|sti|clc|stc|cmc|shl|shr|sal|sar|rol|ror|rcl|rcr|enter|leave)\b/gi;
    while ((m = re.exec(src)) !== null) {
        matches.push({ start: m.index, end: m.index + m[0].length, type: 'keyword', text: m[0] });
    }
    
    // Сортируем по позиции
    matches.sort(function(a, b) { return a.start - b.start; });
    
    // Удаляем перекрывающиеся
    var filtered = [];
    var lastEnd = -1;
    for (var i = 0; i < matches.length; i++) {
        if (matches[i].start >= lastEnd) {
            filtered.push(matches[i]);
            lastEnd = matches[i].end;
        }
    }
    
    // Собираем результат
    var result = '';
    pos = 0;
    for (var j = 0; j < filtered.length; j++) {
        var match = filtered[j];
        // Добавляем обычный текст до совпадения
        if (match.start > pos) {
            result += escapeHtml(src.substring(pos, match.start));
        }
        // Добавляем подсвеченное совпадение
        result += '<span style="color:' + colors[match.type] + '">' + escapeHtml(match.text) + '</span>';
        pos = match.end;
    }
    // Добавляем остаток
    if (pos < src.length) {
        result += escapeHtml(src.substring(pos));
    }
    
    return result;
}

// Экранирование HTML
function escapeHtml(text) {
    var div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

// Экспорт
window.FasmHighlighter = {
    highlight: highlight,
    colors: colors
};

})();
