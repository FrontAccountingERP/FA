

var replinks={'a, button':function(e){e.onkeydown=function(ev){ev=ev||window.event;key=ev.keyCode||ev.which;if(key==37||key==38||key==39||key==40){move_focus(key,e,document.links);ev.returnValue=false;return false;}
}
},
'a.repopts_link':function(e){e.onclick=function(){save_focus(this);set_options(this);JsHttpRequest.request(this,null);return false;}
},
'a.repclass_link':function(e){e.onclick=function(){save_focus(this);showClass(this.id.substring(5));return false;}
},
}
function set_options(e){var replinks=document.getElementsBySelector('a.repopts_link');for(var i in replinks)
replinks[i].style.fontWeight=replinks[i]==e ? 'bold':'normal';}
function showClass(pClass){var classes=document.getElementsBySelector('.repclass');for(var i in classes){cl=classes[i];cl.style.display=(cl.id==('TAB_'+pClass))? "block":"none";}
var classlinks=document.getElementsBySelector('a.repclass_link');for(var i in classlinks)
classlinks[i].style.fontWeight=classlinks[i].id==('class'+pClass)?
'bold':'normal';set_options();document.getElementById('rep_form').innerHTML='';return false;}
Behaviour.register(replinks);