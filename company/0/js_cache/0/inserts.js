

var _focus;var _hotkeys={'alt':false,
'list':false,
'focus':-1
};function validate(e){if(e.name&&(typeof _validate[e.name]=='function'))
return _validate[e.name](e);else{var n=e.name.indexOf('[');if(n!=-1){var key=e.name.substring(n+1,e.name.length-1);if(key.length>1&&_validate[e.name.substring(0,n)])
return _validate[e.name.substring(0,n)][key](e);}
}
return true;}
function set_fullmode(){document.getElementById('ui_mode').value=1;document.loginform.submit();return true;}
function save_focus(e){_focus=e.name||e.id;var h=document.getElementById('hints');if(h){h.style.display=e.title&&e.title.length ? 'inline':'none';h.innerHTML=e.title ? e.title:'';}
}
function _expand(tabobj){var ul=tabobj.parentNode.parentNode;var alltabs=ul.getElementsByTagName("button");var frm=tabobj.form;if(ul.getAttribute("rel")){for(var i=0;i<alltabs.length;i++){alltabs[i].className="ajaxbutton"
}
tabobj.className="current";JsHttpRequest.request(tabobj)
}
}
function expandtab(tabcontentid,tabnumber){var tabs=document.getElementById(tabcontentid);_expand(tabs.getElementsByTagName("input")[tabnumber]);}
function _set_combo_input(e){e.setAttribute('_last',e.value);e.onblur=function(){var but_name=this.name.substring(0,this.name.length-4)+'button';var button=document.getElementsByName(but_name)[0];var select=document.getElementsByName(this.getAttribute('rel'))[0];save_focus(select);if(button&&(this.value!=this.getAttribute('_last'))){JsHttpRequest.request(button);}else if(string_contains(this.className,'combo2')){this.style.display='none';select.style.display='inline';setFocus(select);}
return false;};e.onkeyup=function(ev){var select=document.getElementsByName(this.getAttribute('rel'))[0];if(select&&select.selectedIndex>=0){var len=select.length;var byid=string_contains(this.className,'combo')||string_contains(this.className,'combo3');var ac=this.value.toUpperCase();select.options[select.selectedIndex].selected=false;for(i=0;i < len;i++){var txt=byid ? select.options[i].value:select.options[i].text;if(string_contains(this.className,'combo3')){if(txt.toUpperCase().indexOf(ac)==0){select.options[i].selected=true;break;}
}else{if(txt.toUpperCase().indexOf(ac)>=0){select.options[i].selected=true;break;}
}
}
}
};e.onkeydown=function(ev){ev=ev||window.event;key=ev.keyCode||ev.which;if(key==13){this.blur();return false;}
}
}
function _update_box(s){var byid=string_contains(s.className,'combo')||string_contains(s.className,'combo3');var rel=s.getAttribute('rel');var box=document.getElementsByName(rel)[0];if(box&&s.selectedIndex>=0){var opt=s.options[s.selectedIndex];if(box){var old=box.value;box.value=byid ? opt.value:opt.text;box.setAttribute('_last',box.value);return old!=box.value
}
}
}
function _set_combo_select(e){e.setAttribute('_last',e.selectedIndex);e.onblur=function(){var box=document.getElementsByName(this.getAttribute('rel'))[0];if((this.selectedIndex!=this.getAttribute('_last'))
||((string_contains(this.className,'combo')||string_contains(this.className,'combo3'))&&_update_box(this))
)
this.onchange();}
e.onchange=function(){var s=this;this.setAttribute('_last',this.selectedIndex);if(string_contains(s.className,'combo')||string_contains(this.className,'combo3'))
_update_box(s);if(s.selectedIndex>=0){var sname='_'+s.name+'_update';var update=document.getElementsByName(sname)[0];if(update){JsHttpRequest.request(update);}
}
return true;}
e.onkeydown=function(event){event=event||window.event;key=event.keyCode||event.which;var box=document.getElementsByName(this.getAttribute('rel'))[0];if(key==8||(key==37&&event.altKey)){event.returnValue=false;return false;}
if(box&&(key==32)&&(string_contains(this.className,'combo2'))){this.style.display='none';box.style.display='inline';box.value='';setFocus(box);return false;}else{if(key==13&&!e.length)
return false;}
}
}
var _w;function callEditor(key){var el=document.getElementsByName(editors[key][1])[0];if(_w)_w.close();var left=(screen.width-editors[key][2])/2;var top=(screen.height-editors[key][3])/2;_w=open(editors[key][0]+el.value+'&popup=1',
"edit","scrollbars=yes,resizable=0,width="+editors[key][2]+",height="+editors[key][3]+",left="+left+",top="+top+",screenX="+left+",screenY="+top);if(_w.opener==null)
_w.opener=self;editors._call=key;_w.focus();}
function passBack(value){var o=opener;if(value!=false){var back=o.editors[o.editors._call];var to=o.document.getElementsByName(back[1])[0];if(to){if(to[0]!=undefined)
to[0].value=value;to.value=value;o.JsHttpRequest.request('_'+to.name+'_update',to.form);o.setFocus(to.name);}
}
close();}

function fix_date(date,last){var dat=last.split(user.datesep);var cur=date.split(user.datesep);var day,month,year;if(date==""||date==last)
return date;if(user.datefmt==0||user.datefmt==3){day=dat[1];month=dat[0];year=dat[2];}else if(user.datefmt==1||user.datefmt==4){day=dat[0];month=dat[1];year=dat[2];}else{day=dat[2];month=dat[1];year=dat[0];}
if(cur[1]!=undefined&&cur[1]!=""){if(user.datefmt==0||user.datefmt==3||((user.datefmt==2||user.datefmt==5)&&(cur[2]==undefined||cur[2]=="")))
day=cur[1];else
month=cur[1];}
if(cur[0]!=undefined&&cur[0]!=""){if(cur[1]==undefined||cur[1]=="")
day=cur[0];else if(user.datefmt==0||user.datefmt==3||((user.datefmt==2||user.datefmt==5)&&(cur[2]==undefined||cur[2]=="")))
month=cur[0];else if(user.datefmt==2||user.datefmt==5)
year=cur[0];else
day=cur[0];}
if(cur[2]!=undefined&&cur[2]!=""){if(user.datefmt==2||user.datefmt==5)
day=cur[2];else
year=cur[2];}
if(user.datefmt<3){if(day<10)day='0'+parseInt(day,10);if(month<10)month='0'+parseInt(month,10);}
if(year<100)year=year<60 ?(2000+parseInt(year,10)):(1900+parseInt(year,10));if(user.datefmt==0||user.datefmt==3)
return month+user.datesep+day+user.datesep+year;if(user.datefmt==1||user.datefmt==4)
return day+user.datesep+month+user.datesep+year;return year+user.datesep+month+user.datesep+day;}

var inserts={'input':function(e){if(e.onfocus==undefined){e.onfocus=function(){save_focus(this);if(string_contains(this.className,'combo')||string_contains(this.className,'combo3'))
this.select();};}
if(string_contains(e.className,'combo')||string_contains(e.className,'combo2')||string_contains(e.className,'combo3')){_set_combo_input(e);}
else
if(e.type=='text'){e.onkeydown=function(ev){ev=ev||window.event;key=ev.keyCode||ev.which;if(key==13){if(e.className=='searchbox')e.onblur();return false;}
return true;}
}
},
'input.combo2,input[aspect="fallback"]':
function(e){e.style.display='none';},
'div.js_only':
function(e){e.style.display='block';},
'button':function(e){e.onclick=function(){if(validate(e)){setTimeout(function(){var asp=e.getAttribute('aspect');if(asp&&asp.indexOf('download')===-1&&asp.indexOf('popup')===-1)
set_mark((asp&&((asp.indexOf('process')!==-1)||(asp.indexOf('nonajax')!==-1)))? 'progressbar.gif':'ajax-loader.gif');},100);return true;}
return false;},
e.onkeydown=function(ev){ev=ev||window.event;key=ev.keyCode||ev.which;if(key==8||(key==37&&ev.altKey)){ev.returnValue=false;return false;}
}
},
'button.ajaxsubmit,input.ajaxsubmit,input.editbutton,button.editbutton,button.navibutton':
function(e){e.onclick=function(){if(validate(e)){save_focus(e);var asp=e.getAttribute('aspect')
if(asp&&(asp.indexOf('process')!==-1))
JsHttpRequest.request(this,null,600000);else
JsHttpRequest.request(this);}
return false;}
},
'.amount':function(e){if(e.onblur==undefined){e.setAttribute('_last_val',e.value);e.onblur=function(){var dec=this.getAttribute("dec");var val=this.getAttribute('_last_val');if(val!=get_amount(this.name)){this.setAttribute('_last_val',get_amount(this.name));price_format(this.name,get_amount(this.name),dec);if(e.className.match(/\bactive\b/))
JsHttpRequest.request('_'+this.name+'_changed',this.form);}
};}
},
'.searchbox':
function(e){e.setAttribute('_last_val',e.value);e.setAttribute('autocomplete','off');e.onblur=function(){var val=this.getAttribute('_last_val');if(val!=this.value){this.setAttribute('_last_val',this.value);JsHttpRequest.request('_'+this.name+'_changed',this.form);}
}
},
'.date':
function(e){e.setAttribute('_last_val',e.value);e.setAttribute('autocomplete','off');e.onblur=function(){var val=this.getAttribute('_last_val');if(val!=this.value){this.value=fix_date(this.value,val);this.setAttribute('_last_val',this.value);if(e.className.match(/\bactive\b/))
JsHttpRequest.request('_'+this.name+'_changed',this.form);}
}
},
'button[aspect*selector], button[aspect*abort], input[aspect*selector]':function(e){e.onclick=function(){passBack(this.getAttribute('rel'));return false;}
},
'button[aspect=popup]':function(e){e.onclick=function(){if(_w)_w.close();var left=(screen.width-800)/2;var top=(screen.height-600)/2;_w=open(document.location+'popup=1',
"edit","Scrollbars=0,resizable=0,width=800,height=600, top="+top+",left="+left+",screenX="+left+",screenY="+top);if(_w.opener==null)
_w.opener=self;_w.focus();return false;}
},
'select':function(e){if(e.onfocus==undefined){e.onfocus=function(){save_focus(this);};}
var c=e.className;if(string_contains(c,'combo')||string_contains(c,'combo2')||string_contains(c,'combo3'))
_set_combo_select(e);else{e.onkeydown=function(ev){ev=ev||window.event;key=ev.keyCode||ev.which;if(key==8||(key=37&&ev.altKey)){ev.returnValue=false;return false;}
}
}
},
'a.printlink':function(l){l.onclick=function(){save_focus(this);JsHttpRequest.request(this,null,60000);return false;}
},
'a.repopts_link':function(l){l.onclick=function(){save_focus(this);var replinks=document.getElementsBySelector('a.repopts_link');for(var i in replinks)
replinks[i].style.fontWeight=replinks[i]==this ? 'bold':'normal';JsHttpRequest.request(this,null);return false;}
},
'a':function(e){e.onkeydown=function(ev){ev=ev||window.event;key=ev.keyCode||ev.which;if(key==37||key==38||key==39||key==40){move_focus(key,e,document.links);ev.returnValue=false;return false;}
}
if(e.className=='shortcut'
||e.className=='menu_option'
||e.className=='menu_tab'
||e.className=='selected')
e.onclick=function(ev){if(_validate._processing
&&_validate._modified
&&!confirm(_validate._processing)){ev.returnValue=false;return false;}
if(_hotkeys.alt)
return false;window.location=e.href;}
},
'ul.ajaxtabs':function(ul){var ulist=ul.getElementsByTagName("li");for(var x=0;x<ulist.length;x++){var tab=ulist[x].getElementsByTagName("button")[0];var url=tab.form.action
tab.onclick=function(){if(!_hotkeys.alt&&!this.disabled)
_expand(this);return false;}
}
},
'textarea':function(e){if(e.onfocus==undefined){e.onfocus=function(){save_focus(this);};}
}

};function stopEv(ev){if(ev.preventDefault){ev.preventDefault();ev.stopPropagation();}else{ev.returnValue=false;ev.cancelBubble=true;window.keycode=0;}
return false;}

function setHotKeys(){document.onkeydown=function(ev){ev=ev||window.event;key=ev.keyCode||ev.which;if(key==18&&!ev.ctrlKey){_hotkeys.alt=true;_hotkeys.focus=-1;return stopEv(ev);}
else if(ev.altKey&&!ev.ctrlKey&&((key>47&&key<58)||(key>64&&key<91))){key=String.fromCharCode(key);var n=_hotkeys.focus;var l=document.getElementsBySelector('[accesskey='+key+']');var cnt=l.length;_hotkeys.list=l;for(var i=0;i<cnt;i++){n=(n+1)%cnt;if(l[n].accessKey==key&&(l[n].offsetWidth||l[n].offsetHeight)){_hotkeys.focus=n;var tmp=function(){l[_hotkeys.focus].focus();};setTimeout(tmp,0);break;}
}
return stopEv(ev);}
if((ev.ctrlKey&&key==13)||key==27){_hotkeys.alt=false;_hotkeys.focus=-1;ev.cancelBubble=true;if(ev.stopPropagation)ev.stopPropagation();for(var j=0;j<this.forms.length;j++){var form=this.forms[j];for(var i=0;i<form.elements.length;i++){var el=form.elements[i];var asp=el.getAttribute('aspect');if(!string_contains(el.className,'editbutton')&&(asp&&asp.indexOf('selector')!==-1)&&(key==13||key==27)){passBack(key==13 ? el.getAttribute('rel'):false);ev.returnValue=false;return false;}
if(((asp&&asp.indexOf('default')!==-1)&&key==13)||((asp&&asp.indexOf('cancel')!==-1)&&key==27)){if(validate(el)){if(asp.indexOf('nonajax')!==-1)
el.click();else
if(asp.indexOf('process')!==-1)
JsHttpRequest.request(el,null,600000);else
JsHttpRequest.request(el);}
ev.returnValue=false;return false;}
}
}
ev.returnValue=false;return false;}
if(editors!==undefined&&editors[key]){callEditor(key);return stopEv(ev);}
return true;};document.onkeyup=function(ev){ev=ev||window.event;key=ev.keyCode||ev.which;if(_hotkeys.alt==true){if(key==18){_hotkeys.alt=false;if(_hotkeys.focus >=0){var link=_hotkeys.list[_hotkeys.focus];if(link.onclick)
link.onclick();else
if(link.target=='_blank'){window.open(link.href,'','toolbar=no,scrollbar=no,resizable=yes,menubar=no,width=900,height=500');openWindow(link.href,'_blank');}else
window.location=link.href;}
return stopEv(ev);}
}
return true;}
}
Behaviour.register(inserts);Behaviour.addLoadEvent(setFocus);Behaviour.addLoadEvent(setHotKeys);