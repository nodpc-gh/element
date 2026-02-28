; html.inc - Element IDE v1.0 Release

html_content db '<!DOCTYPE html>', 13, 10
             db '<html><head><meta charset="UTF-8"><title>Element IDE</title>', 13, 10
             db '<meta http-equiv="Cache-Control" content="no-cache,no-store,must-revalidate">', 13, 10
             db '<meta http-equiv="Pragma" content="no-cache">', 13, 10
             db '<meta http-equiv="Expires" content="0">', 13, 10
             db '<style>', 13, 10
             db 'html,body{height:100%;margin:0;padding:0;overflow:hidden}', 13, 10
             db 'body{font-family:Segoe UI,sans-serif;background:#1e1e1e;color:#ccc}', 13, 10
             db '.container{display:flex;height:100vh}', 13, 10
             db '.left{width:250px;background:#252526;border-right:1px solid #3c3c3c;overflow-y:auto}', 13, 10
             db '.center{flex:1;display:flex;flex-direction:column;min-width:0}', 13, 10
             db '.right{width:300px;background:#252526;border-left:1px solid #3c3c3c;overflow-y:auto}', 13, 10
             db '.toolbar{height:30px;background:#333;display:flex;align-items:center;padding:0 10px}', 13, 10
             db '.toolbar button{background:#3c3c3c;border:1px solid #555;color:#ccc;padding:3px 10px;margin-right:5px;cursor:pointer}', 13, 10
             db '.tabs{height:28px;background:#2d2d30;display:flex;align-items:flex-end}', 13, 10
             db '.tab{padding:5px 15px;background:#252526;border-right:1px solid #333;cursor:pointer;font-size:12px}', 13, 10
             db '.tab.active{background:#1e1e1e;border-bottom:2px solid #007acc}', 13, 10
             db '.workspace{flex:1;position:relative;background:#1e1e1e;overflow:hidden}', 13, 10
             db '.status{height:24px;background:#007acc;color:#fff;display:flex;align-items:center;padding:0 10px;font-size:11px}', 13, 10
             db '.tree-item{padding:4px 8px 4px 20px;cursor:grab;user-select:none}', 13, 10
             db '.tree-item:hover{background:#2a2d2e}', 13, 10
             db '.tree-group{font-weight:bold;padding-left:8px;background:#2d2d30;margin:2px 0}', 13, 10
             db '.tree-item .icon{margin-right:6px}', 13, 10
             db '.element{position:absolute;min-width:100px;min-height:40px;background:#2d2d30;border:1px solid #007acc;color:#ccc;cursor:move;user-select:none;font-size:11px;border-radius:2px}', 13, 10
             db '.element.selected{border:2px solid #00ccff}', 13, 10
             db '.el-hdr{padding:3px 6px;background:#3c3c3c;font-weight:bold;cursor:move}', 13, 10
             db '.el-body{padding:4px 6px}', 13, 10
             db '.el-data{color:#888;font-size:9px}', 13, 10
             db '.el-stats{color:#0a8;font-size:9px}', 13, 10
             db '.port-ctrl{position:absolute;width:18px;height:18px;background:#2d2d30;border:1px solid #444;border-radius:4px;cursor:pointer;z-index:11;display:flex;align-items:center;justify-content:center;font-size:16px;font-weight:bold;color:#0a8;line-height:1;transition:all 0.15s}', 13, 10
             db '.port-ctrl:hover{background:#0a8;color:#fff;border-color:#0a8;transform:scale(1.1)}', 13, 10
             db '.port-ctrl.minus{color:#f66;border-color:#f66}', 13, 10
             db '.port-ctrl.minus:hover{background:#f66;color:#fff;border-color:#f66}', 13, 10
             db '.port-ctrl.in{color:#4a9}', 13, 10
             db '.port-ctrl.in:hover{background:#4a9}', 13, 10
             db '.port-ctrl.out{color:#94f}', 13, 10
             db '.port-ctrl.out:hover{background:#94f}', 13, 10
             db '.port{position:absolute;width:10px;height:10px;background:#444;border:2px solid #666;border-radius:50%;cursor:crosshair;z-index:10}', 13, 10
             db '.port:hover{background:#00ccff;border-color:#fff}', 13, 10
             db '.port.in{border-color:#4a9}', 13, 10
             db '.port.out{border-color:#94f}', 13, 10
             db '.port.pl{left:-5px;top:50%}', 13, 10
             db '.port.pr{right:-5px;top:50%}', 13, 10
             db '.port.pt{left:50%;top:-5px}', 13, 10
             db '.port.pb{left:50%;bottom:-5px}', 13, 10
             db '#svg{position:absolute;left:0;top:0;width:100%;height:100%;pointer-events:none;z-index:5}', 13, 10
             db '.line{fill:none;stroke:#666;stroke-width:2;opacity:0.7;transition:stroke 0.2s}', 13, 10
             db '.line.data{stroke:#0a8;stroke-width:2.5}', 13, 10
             db '.line.event{stroke:#94f;stroke-width:2}', 13, 10
             db '.line.id{stroke:#f60;stroke-width:3;stroke-dasharray:5,3}', 13, 10
             db '.line:hover{opacity:1;stroke-width:3;cursor:pointer;pointer-events:stroke}', 13, 10
             db '#ctx{position:fixed;display:none;background:#2d2d30;border:1px solid #007acc;border-radius:3px;box-shadow:0 3px 10px rgba(0,0,0,0.5);z-index:1000;min-width:150px;font-size:11px}', 13, 10
             db '#ctx .mh{padding:5px 10px;background:#3c3c3c;font-weight:bold;color:#888}', 13, 10
             db '#ctx .mi{padding:6px 10px;cursor:pointer;border-bottom:1px solid #333}', 13, 10
             db '#ctx .mi:hover{background:#007acc}', 13, 10
             db '#ctx .mi:last-child{border-bottom:none}', 13, 10
             db '.pr{display:flex;border-bottom:1px solid #333;font-size:11px}', 13, 10
             db '.pn{width:45%;padding:5px 8px;background:#2d2d30;border-right:1px solid #333}', 13, 10
             db '.pv{width:55%;padding:5px 8px;background:#252526}', 13, 10
             db '.pv input{width:100%;background:#333;border:1px solid #444;color:#ccc;padding:2px}', 13, 10
             db '.sh{padding:6px 8px;background:#2d2d30;font-weight:bold;font-size:11px;border-bottom:1px solid #333}', 13, 10
             db '</style></head><body>', 13, 10
             db '<div class=container>', 13, 10
             
             ; Левая панель
             db '<div class=left>', 13, 10
             db '<div class=sh>Elements</div>', 13, 10
             db '<div class=tree-group><div class=tree-item draggable=true data-type=btn><span class=icon>🔘</span>Button</div></div>', 13, 10
             db '<div class=tree-group><div class=tree-item draggable=true data-type=lbl><span class=icon>📝</span>Label</div></div>', 13, 10
             db '<div class=tree-group><div class=tree-item draggable=true data-type=edt><span class=icon>✏️</span>Edit</div></div>', 13, 10
             db '<div class=tree-group><div class=tree-item draggable=true data-type=chk><span class=icon>☑️</span>CheckBox</div></div>', 13, 10
             db '<div class=tree-group><div class=tree-item draggable=true data-type=pnl><span class=icon>📦</span>Panel</div></div>', 13, 10
             db '</div>', 13, 10
             
             ; Центральная панель
             db '<div class=center>', 13, 10
             db '<div class=toolbar><button id=btnCompile title=Compile>⚙️</button><button id=btnExport title=Export>📤</button><button id=btnImport title=Import>📥</button><button>💾</button><button>📂</button><button>▶️</button><button>⏹️</button></div>', 13, 10
             db '<div class=tabs><div class="tab active">Form1</div></div>', 13, 10
             db '<div class=workspace id=ws>', 13, 10
             db '<canvas id=cvs style="position:absolute;left:0;top:0"></canvas>', 13, 10
             db '<svg id=svg></svg>', 13, 10
             db '<div id=ec style="position:absolute;left:0;top:0"></div>', 13, 10
             db '</div>', 13, 10
             db '<div class=status><span id=st>ROOT</span><span style="margin-left:auto">Element IDE v1.0</span></div>', 13, 10
             db '</div>', 13, 10
             
             ; Правая панель
             db '<div class=right>', 13, 10
             db '<div class=tabs><div class="tab active">Свойства</div></div>', 13, 10
             db '<div class=sh>Элемент</div>', 13, 10
             db '<div class=pr><div class=pn>Name</div><div class=pv><input id=pName></div></div>', 13, 10
             db '<div class=pr><div class=pn>Left</div><div class=pv><input id=pLeft></div></div>', 13, 10
             db '<div class=pr><div class=pn>Top</div><div class=pv><input id=pTop></div></div>', 13, 10
             db '<div class=pr><div class=pn>Width</div><div class=pv><input id=pWidth></div></div>', 13, 10
             db '<div class=pr><div class=pn>Height</div><div class=pv><input id=pHeight></div></div>', 13, 10
             db '<div class=sh>Данные</div>', 13, 10
             db '<div class=pr><div class=pn>Type</div><div class=pv><input id=pType readonly></div></div>', 13, 10
             db '<div class=pr><div class=pn>ID</div><div class=pv><input id=pId readonly></div></div>', 13, 10
             db '<div class=sh>Статистика</div>', 13, 10
             db '<div class=pr><div class=pn>Events</div><div class=pv><input id=pEv readonly></div></div>', 13, 10
             db '</div>', 13, 10
             db '</div>', 13, 10
             
             ; Контекстное меню
             db '<div id=ctx><div class=mh id=ch>Element</div>', 13, 10
             db '<div style="padding:8px 10px;font-size:10px;color:#888;border-bottom:1px solid #333">Управление портами</div>', 13, 10
             db '<div style="display:grid;grid-template-columns:1fr auto 1fr;gap:2px;padding:8px;align-items:center">', 13, 10
             db '<div style="text-align:center;font-size:9px;color:#666">Слева</div>', 13, 10
             db '<div style="text-align:center;font-size:9px;color:#666">Сверху</div>', 13, 10
             db '<div style="text-align:center;font-size:9px;color:#666">Справа</div>', 13, 10
             db '<button class=port-ctrl in data-a=add-pl style="justify-self:end">+</button>', 13, 10
             db '<button class=port-ctrl in data-a=add-pt>+</button>', 13, 10
             db '<button class=port-ctrl out data-a=add-pr style="justify-self:start">+</button>', 13, 10
             db '<button class=port-ctrl minus data-a=del-pl style="justify-self:end;color:#f66">−</button>', 13, 10
             db '<button class=port-ctrl minus data-a=del-pt style="color:#f66">−</button>', 13, 10
             db '<button class=port-ctrl minus data-a=del-pr style="justify-self:start;color:#f66">−</button>', 13, 10
             db '</div>', 13, 10
             db '<div style="display:grid;grid-template-columns:1fr auto 1fr;gap:2px;padding:0 8px 8px;align-items:center">', 13, 10
             db '<div style="text-align:center;font-size:9px;color:#666">Снизу</div>', 13, 10
             db '<button class=port-ctrl out data-a=add-pb>+</button>', 13, 10
             db '<button class=port-ctrl minus data-a=del-pb style="color:#f66">−</button>', 13, 10
             db '</div>', 13, 10
             db '<div class=sep style="height:1px;background:#333;margin:0 8px"></div>', 13, 10
             db '<div class=mi data-a=dup style=color:#0f8>📋 Копировать</div>', 13, 10
             db '<div class=mi data-a=del style=color:#f66>🗑️ Удалить элемент</div>', 13, 10
             db '</div>', 13, 10
             
             db '<div id=ctxLn style="display:none;background:#2d2d30;border:1px solid #f66;border-radius:3px;box-shadow:0 3px 10px rgba(255,102,102,0.3);z-index:1000;min-width:150px;font-size:11px;position:fixed">', 13, 10
             db '<div class=mh style="background:#3c1a1a;color:#f66">Соединение</div>', 13, 10
             db '<div class=mi data-a=del-ln style=color:#f66>🗑️ Удалить</div>', 13, 10
             db '<div class=mi data-a=ln-type>Type: <span id=lnTypeInfo></span></div>', 13, 10
             db '</div>', 13, 10
             
             db '<div id=compileModal style="display:none;position:fixed;top:50%;left:50%;transform:translate(-50%,-50%);background:#1e1e1e;border:2px solid #007acc;border-radius:5px;box-shadow:0 5px 20px rgba(0,0,0,0.5);z-index:2000;min-width:500px;max-width:800px;max-height:80vh;overflow:auto">', 13, 10
             db '<div style="padding:10px 15px;background:#2d2d30;border-bottom:1px solid #3c3c3c;display:flex;justify-content:space-between;align-items:center">', 13, 10
             db '<b style="color:#007acc">⚙️ Компилятор</b>', 13, 10
             db '<button id=btnCloseModal style="background:#3c3c3c;border:none;color:#ccc;cursor:pointer;padding:5px 10px;border-radius:3px">✕</button>', 13, 10
             db '</div>', 13, 10
             db '<div style="padding:15px">', 13, 10
             db '<div style="margin-bottom:10px"><b>Элементы:</b> <span id=compileElCnt>0</span> | <b>Соединения:</b> <span id=compileConnCnt>0</span></div>', 13, 10
             db '<textarea id=compileOutput style="width:100%;height:300px;background:#1e1e1e;border:1px solid #3c3c3c;color:#0f0;font-family:Consolas,monospace;font-size:11px;padding:10px;resize:vertical"></textarea>', 13, 10
             db '<div style="margin-top:10px;display:flex;gap:10px">', 13, 10
             db '<button id=btnCopyJson style="background:#007acc;border:none;color:#fff;cursor:pointer;padding:8px 15px;border-radius:3px">📋 Копировать JSON</button>', 13, 10
             db '<button id=btnDownloadJson style="background:#0a8;border:none;color:#fff;cursor:pointer;padding:8px 15px;border-radius:3px">💾 Скачать файл</button>', 13, 10
             db '</div>', 13, 10
             db '</div>', 13, 10
             db '</div>', 13, 10
             
             db '<div id=overlay style="display:none;position:fixed;top:0;left:0;width:100%;height:100%;background:rgba(0,0,0,0.5);z-index:1999"></div>', 13, 10
             
             db '<script>', 13, 10
             db 'var cvs=document.getElementById("cvs"),x=cvs.getContext("2d");', 13, 10
             db 'var ws=document.getElementById("ws"),ec=document.getElementById("ec"),svg=document.getElementById("svg");', 13, 10
             db 'var st=document.getElementById("st"),ctx=document.getElementById("ctx"),ch=document.getElementById("ch");', 13, 10
             db 'var ctxLn=document.getElementById("ctxLn"),lnTypeInfo=document.getElementById("lnTypeInfo");', 13, 10
             db 'var compileModal=document.getElementById("compileModal"),compileOutput=document.getElementById("compileOutput");', 13, 10
             db 'var compileElCnt=document.getElementById("compileElCnt"),compileConnCnt=document.getElementById("compileConnCnt");', 13, 10
             db 'var overlay=document.getElementById("overlay");', 13, 10
             db 'var dragType=null,dragEl=null,offX=0,offY=0,selEl=null,elCnt=0,conns=[],connId=0,connPort=null,tmpLine=null,ctxT=null,ctxType=null,ctxLnT=null;', 13, 10
             
             db 'function resize(){var w=ws.clientWidth,h=ws.clientHeight;cvs.width=w;cvs.height=h;svg.style.width=w+"px";svg.style.height=h+"px";ec.style.width=w+"px";ec.style.height=h+"px";drawGrid();}', 13, 10
             db 'function drawGrid(){x.fillStyle="#1e1e1e";x.fillRect(0,0,cvs.width,cvs.height);x.strokeStyle="#2a2a2a";x.lineWidth=0.5;', 13, 10
             db 'for(var i=0;i<cvs.width;i+=20){x.beginPath();x.moveTo(i,0);x.lineTo(i,cvs.height);x.stroke();}', 13, 10
             db 'for(var j=0;j<cvs.height;j+=20){x.beginPath();x.moveTo(0,j);x.lineTo(cvs.width,j);x.stroke();}}', 13, 10
             db 'window.onresize=resize;setTimeout(resize,100);', 13, 10
             
             db 'document.querySelectorAll(".tree-item[draggable]").forEach(function(it){it.ondragstart=function(e){dragType=this.getAttribute("data-type");};});', 13, 10
             db 'ec.ondragover=function(e){e.preventDefault();};', 13, 10
             db 'ec.ondrop=function(e){e.preventDefault();var r=ec.getBoundingClientRect();if(dragType)mkEl(dragType,e.clientX-r.left,e.clientY-r.top);dragType=null;};', 13, 10
             
             db 'function mkEl(type,x,y){elCnt++;var el=document.createElement("div");el.className="element";el.id="el_"+elCnt;el.setAttribute("data-type",type);el.setAttribute("data-ev",0);el.style.left=x+"px";el.style.top=y+"px";', 13, 10
             db 'var t={btn:"Button",lbl:"Label",edt:"Edit",chk:"CheckBox",pnl:"Panel"};var cap=t[type]||type;el.setAttribute("data-caption",cap);el.innerHTML="<div class=el-hdr>"+cap+"</div><div class=el-body><div class=el-data>"+cap+"</div><div class=el-stats>⚡0</div></div>";', 13, 10
             db 'addPort(el,"pl","in");addPort(el,"pr","out");addPort(el,"pt","id");addPort(el,"pb","data");el.onmousedown=onElDown;el.ondblclick=onElDbl;el.oncontextmenu=onElCtx;ec.appendChild(el);sel(el);updProps(el);}', 13, 10
             
             db 'function addPort(el,pos,lbl){var p=document.createElement("div");p.className="port "+pos+" "+lbl;p.setAttribute("data-port",lbl);p.onmousedown=onPortDown;p.onmouseup=onPortUp;p.oncontextmenu=onPortCtx;el.appendChild(p);var s=document.createElement("span");s.style.position="absolute";s.style.fontSize="8px";s.style.color="#666";s.style.pointerEvents="none";if(pos==="pl"){s.style.left="10px";s.style.top="50%";s.style.transform="translateY(-50%)";s.textContent=lbl;}else if(pos==="pr"){s.style.right="10px";s.style.top="50%";s.style.transform="translateY(-50%)";s.textContent=lbl;}else if(pos==="pt"){s.style.top="10px";s.style.left="50%";s.style.transform="translateX(-50%)";s.textContent=lbl;}else if(pos==="pb"){s.style.bottom="10px";s.style.left="50%";s.style.transform="translateX(-50%)";s.textContent=lbl;}el.appendChild(s);}', 13, 10
             db 'function sel(el){if(selEl)selEl.classList.remove("selected");selEl=el;if(el){el.classList.add("selected");st.textContent=el.id;updProps(el);}else{st.textContent="ROOT";clrProps();}}', 13, 10
             
             db 'function onElDown(e){if(e.button!==0||e.target.className.indexOf("port")>=0)return;e.stopPropagation();dragEl=this;var r=this.getBoundingClientRect();offX=e.clientX-r.left;offY=e.clientY-r.top;sel(this);document.onmousemove=onElMove;document.onmouseup=onElUp;}', 13, 10
             db 'function onElMove(e){if(!dragEl)return;var r=ec.getBoundingClientRect();dragEl.style.left=(e.clientX-r.left-offX)+"px";dragEl.style.top=(e.clientY-r.top-offY)+"px";updLines();}', 13, 10
             db 'function onElUp(e){dragEl=null;document.onmousemove=null;document.onmouseup=null;}', 13, 10
             db 'function onElDbl(e){if(this.parentNode){delEl(this);}}', 13, 10
             
             db 'function onElCtx(e){e.preventDefault();e.stopPropagation();ctxT=this;ctxType="el";var pc=this.querySelectorAll(".port").length;ch.textContent=this.id+" (портов: "+pc+")";ctx.style.left=e.clientX+"px";ctx.style.top=e.clientY+"px";ctx.style.display="block";document.onclick=hideCtx;}', 13, 10
             
             db 'function onPortDown(e){if(e.button!==0)return;e.stopPropagation();connPort=this;var r=ec.getBoundingClientRect(),pr=this.getBoundingClientRect();var x1=pr.left+pr.width/2-r.left,y1=pr.top+pr.height/2-r.top;', 13, 10
             db 'tmpLine=document.createElementNS("http://www.w3.org/2000/svg","path");tmpLine.setAttribute("class","line");tmpLine.setAttribute("stroke-dasharray","5,5");tmpLine.style.pointerEvents="none";svg.appendChild(tmpLine);document.onmousemove=onConnMove;document.onmouseup=onConnUp;}', 13, 10
             
             db 'function onConnMove(e){if(!connPort||!tmpLine)return;var r=ec.getBoundingClientRect(),pr=connPort.getBoundingClientRect();var x1=pr.left+pr.width/2-r.left,y1=pr.top+pr.height/2-r.top,x2=e.clientX-r.left,y2=e.clientY-r.top;tmpLine.setAttribute("d","M"+x1+","+y1+" C"+(x1+50)+","+y1+", "+(x2-50)+","+y2+", "+x2+","+y2);}', 13, 10
             
             db 'function onConnUp(e){if(connPort&&tmpLine){tmpLine.parentNode.removeChild(tmpLine);}connPort=null;document.onmousemove=null;document.onmouseup=null;}', 13, 10
             
             db 'function onPortUp(e){e.stopPropagation();if(!connPort||connPort===this)return;var fEl=connPort.closest(".element"),tEl=this.closest(".element");if(fEl===tEl)return;var hasConn=false;conns.forEach(function(c){if(c.from===this||c.to===this){hasConn=true;}}.bind(this));if(hasConn){delLineByPort(this);}mkConn(connPort,this);connPort=null;if(tmpLine){tmpLine.parentNode.removeChild(tmpLine);}}', 13, 10
             
             db 'function mkConn(from,to){connId++;var ln=document.createElementNS("http://www.w3.org/2000/svg","path");', 13, 10
             db 'var cls="line";', 13, 10
             db 'if(from.getAttribute("data-port")==="data"||to.getAttribute("data-port")==="data"){cls+=" data";}', 13, 10
             db 'else if(from.getAttribute("data-port")==="id"||to.getAttribute("data-port")==="id"){cls+=" id";}', 13, 10
             db 'else{cls+=" event";}', 13, 10
             db 'ln.setAttribute("class",cls);ln.onclick=function(e){e.stopPropagation();sel(null);};ln.oncontextmenu=onLineCtx;svg.appendChild(ln);', 13, 10
             db 'var cn={id:"c_"+connId,from:from,to:to,line:ln,type:cls.split(" ")[1]};conns.push(cn);from.classList.add("connected");to.classList.add("connected");updLines();var fEv=from.closest(".element");if(fEv){var n=parseInt(fEv.getAttribute("data-ev")||0)+1;fEv.setAttribute("data-ev",n);updStats(fEv);}}', 13, 10
             
             db 'function updLines(){var r=ec.getBoundingClientRect();conns.forEach(function(cn){var p1=cn.from.getBoundingClientRect(),p2=cn.to.getBoundingClientRect();var x1=p1.left+p1.width/2-r.left,y1=p1.top+p1.height/2-r.top,x2=p2.left+p2.width/2-r.left,y2=p2.top+p2.height/2-r.top;', 13, 10
             db 'cn.line.setAttribute("d","M"+x1+","+y1+" C"+(x1+40)+","+y1+", "+(x2-40)+","+y2+", "+x2+","+y2);});}', 13, 10
             
             db 'function updStats(el){var s=el.querySelector(".el-stats");if(s){s.textContent="⚡"+(el.getAttribute("data-ev")||0);}}', 13, 10
             db 'function onPortCtx(e){e.preventDefault();e.stopPropagation();ctxT=this;ctxType="port";ch.textContent="Port";ctx.style.left=e.clientX+"px";ctx.style.top=e.clientY+"px";ctx.style.display="block";document.onclick=hideCtx;}', 13, 10
             db 'function onLineCtx(e){e.preventDefault();e.stopPropagation();ctxLnT=this;var cn=conns.find(function(c){return c.line===this;});if(cn){ctxLn.style.left=e.clientX+"px";ctxLn.style.top=e.clientY+"px";ctxLn.style.display="block";lnTypeInfo.textContent=cn.type;}document.onclick=hideCtxAll;}', 13, 10
             db 'function hideCtx(){ctx.style.display="none";}', 13, 10
             db 'function hideCtxLn(){ctxLn.style.display="none";}', 13, 10
             db 'function hideCtxAll(){ctx.style.display="none";ctxLn.style.display="none";document.onclick=null;}', 13, 10
             
             db 'ctx.querySelectorAll(".port-ctrl").forEach(function(btn){btn.onclick=function(){var a=this.getAttribute("data-a");', 13, 10
             db 'if(a==="add-pl"){addPort(ctxT,"pl","in");}', 13, 10
             db 'if(a==="add-pt"){addPort(ctxT,"pt","in");}', 13, 10
             db 'if(a==="add-pr"){addPort(ctxT,"pr","out");}', 13, 10
             db 'if(a==="add-pb"){addPort(ctxT,"pb","out");}', 13, 10
             db 'if(a==="del-pl"){delPortByPos(ctxT,"pl");}', 13, 10
             db 'if(a==="del-pt"){delPortByPos(ctxT,"pt");}', 13, 10
             db 'if(a==="del-pr"){delPortByPos(ctxT,"pr");}', 13, 10
             db 'if(a==="del-pb"){delPortByPos(ctxT,"pb");}', 13, 10
             db 'hideCtx();};});', 13, 10
             db 'ctx.querySelectorAll(".mi").forEach(function(mi){mi.onclick=function(){var a=this.getAttribute("data-a");', 13, 10
             db 'if(a==="dup"){dupEl(ctxT);}', 13, 10
             db 'if(a==="del"&&ctxType==="el"){delEl(ctxT);}', 13, 10
             db 'if(a==="del"&&ctxType==="port"){delPort(ctxT);}', 13, 10
             db 'if(a==="del-ln"){delLine(ctxLnT);}hideCtxAll();};});', 13, 10
             
             db 'function delEl(el){conns.slice().forEach(function(c){if(c.from.closest(".element")===el||c.to.closest(".element")===el){delLine(c.line);}});if(el.parentNode)el.parentNode.removeChild(el);sel(null);}', 13, 10
             db 'function dupEl(el){var x=parseInt(el.style.left)||0,y=parseInt(el.style.top)||0;mkEl(el.getAttribute("data-type"),x+20,y+20);var newEl=selEl;var ports=el.querySelectorAll(".port");var newPorts=newEl.querySelectorAll(".port");for(var i=0;i<Math.min(ports.length,newPorts.length);i++){ports[i].setAttribute("data-port",newPorts[i].getAttribute("data-port"));}}', 13, 10
             db 'function delPort(p){conns.slice().forEach(function(c){if(c.from===p||c.to===p){delLine(c.line);}});var nxt=p.nextSibling;if(nxt&&nxt.className==="port-lbl")nxt.parentNode.removeChild(nxt);if(p.parentNode)p.parentNode.removeChild(p);}', 13, 10
             db 'function delPortByPos(el,pos){var ports=el.querySelectorAll(".port."+pos);if(ports.length>0){delPort(ports[ports.length-1]);}}', 13, 10
             db 'function delLineByPort(p){conns.slice().forEach(function(c){if(c.from===p||c.to===p){delLine(c.line);}});}', 13, 10
             db 'function delLine(ln){var idx=conns.findIndex(function(c){return c.line===ln;});if(idx>=0){var cn=conns[idx];cn.from.classList.remove("connected");cn.to.classList.remove("connected");if(cn.line.parentNode)cn.line.parentNode.removeChild(cn.line);conns.splice(idx,1);}}', 13, 10
             
             db 'function updProps(el){if(!el)return;document.getElementById("pName").value=el.getAttribute("data-type")||"";document.getElementById("pLeft").value=parseInt(el.style.left)||0;document.getElementById("pTop").value=parseInt(el.style.top)||0;', 13, 10
             db 'document.getElementById("pWidth").value=el.offsetWidth;document.getElementById("pHeight").value=el.offsetHeight;document.getElementById("pType").value=el.getAttribute("data-type")||"";document.getElementById("pId").value=el.id;document.getElementById("pEv").value=el.getAttribute("data-ev")||0;}', 13, 10
             db 'function clrProps(){document.getElementById("pName").value="";document.getElementById("pLeft").value="";document.getElementById("pTop").value="";document.getElementById("pWidth").value="";document.getElementById("pHeight").value="";document.getElementById("pType").value="";document.getElementById("pId").value="";document.getElementById("pEv").value="";}', 13, 10

             db 'cvs.onclick=function(){sel(null);hideCtxAll();};', 13, 10
             
             db 'document.getElementById("btnCompile").onclick=function(){compile();};', 13, 10
             db 'document.getElementById("btnCloseModal").onclick=function(){compileModal.style.display="none";overlay.style.display="none";};', 13, 10
             db 'document.getElementById("btnCopyJson").onclick=function(){compileOutput.select();document.execCommand("copy");};', 13, 10
             db 'document.getElementById("btnDownloadJson").onclick=function(){var blob=new Blob([compileOutput.value],{type:"application/json"});var a=document.createElement("a");a.href=URL.createObjectURL(blob);a.download="element_project.json";a.click();};', 13, 10
             db 'overlay.onclick=function(){compileModal.style.display="none";overlay.style.display="none";};', 13, 10
             
             db 'function compile(){var elements=[];ec.querySelectorAll(".element").forEach(function(el){var ports=[];el.querySelectorAll(".port").forEach(function(p){ports.push({pos:p.className.split(" ")[1],type:p.getAttribute("data-port")});});elements.push({id:el.id,type:el.getAttribute("data-type"),caption:el.getAttribute("data-caption"),x:parseInt(el.style.left)||0,y:parseInt(el.style.top)||0,ev:parseInt(el.getAttribute("data-ev"))||0,ports:ports});});var connections=[];conns.forEach(function(cn){connections.push({from:cn.from.getAttribute("data-port"),to:cn.to.getAttribute("data-port"),type:cn.type});});var project={version:"1.0",elements:elements,connections:connections};var json=JSON.stringify(project,null,2);compileOutput.value=json;compileElCnt.textContent=elements.length;compileConnCnt.textContent=connections.length;compileModal.style.display="block";overlay.style.display="block";}', 13, 10
             
             db '</script></div></body></html>', 0

html_content_len = $ - html_content
