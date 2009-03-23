/**********************************************************************
    Copyright (C) FrontAccounting, LLC.
	Released under the terms of the GNU General Public License, GPL, 
	as published by the Free Software Foundation, either version 3 
	of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the License here <http://www.gnu.org/licenses/gpl-3.0.html>.
***********************************************************************/
var _focus;
var _hotkeys = {
	'alt': false,	// whether is the Alt key pressed
	'focus': -1	// currently selected indeks of document.links
};

function debug(msg) {
    box = document.getElementById('msgbox')
	box.innerHTML= box.innerHTML+'<br>'+msg
}

function progbar() {
	box = document.getElementById('msgbox');
    box.innerHTML= "<center><table width='98%' border='1' cellpadding=3 "
	+"bordercolor='#007700' style='border-collapse: collapse'>"
	+"<tr><td align='center' bgcolor='#ccffcc' >"
		+"<img src='"+user.theme+"images/progressbar.gif' alt='"
		+user.loadtxt+"' /></td></tr></table></center><br>";
	box.style.display = 'block';
}

function save_focus(e) {
  _focus = e.name||e.id;
  var h = document.getElementById('hints');
  if (h) {
	h.style.display = e.title && e.title.length ? 'inline' : 'none';
	h.innerHTML = e.title ? e.title : '';
  }
}

function _expand(tabobj) {

  var ul = tabobj.parentNode.parentNode;
  var alltabs=ul.getElementsByTagName("input");
  var frm = tabobj.form;

  if (ul.getAttribute("rel")){
	for (var i=0; i<alltabs.length; i++){
	  alltabs[i].className = "ajaxbutton"  //deselect all tabs
	}
	tabobj.className = "current";
	JsHttpRequest.request(tabobj)
  }
}

//interface for selecting a tab (plus expand corresponding content)
function expandtab(tabcontentid, tabnumber) {
  var tabs = document.getElementById(tabcontentid);
 _expand(tabs.getElementsByTagName("input")[tabnumber]);
}

function _set_combo_input(e) {
		e.setAttribute('_last', e.value);
		e.onblur=function() { 
		  var but_name = this.name.substring(0, this.name.length-4)+'button';
		  var button = document.getElementsByName(but_name)[0];
		  var select = document.getElementsByName(this.getAttribute('rel'))[0];
		  save_focus(select);
// submit request if there is submit_on_change option set and 
// search field has changed.
		  if (button && (this.value != this.getAttribute('_last'))) {
	  		JsHttpRequest.request(button);
		  } else if(this.className=='combo2') {
				this.style.display = 'none';
				select.style.display = 'inline';
				setFocus(select.name);
		  }
		  return false;
		};
		e.onkeyup = function(ev) {
			var select = document.getElementsByName(this.getAttribute('rel'))[0];
			if(select && select.selectedIndex>=0) {
			  var len = select.length;
			  var byid = this.className=='combo';
			  var ac = this.value.toUpperCase();
			  select.options[select.selectedIndex].selected = false;
			  for (i = 0; i < len; i++) {
				var txt = byid ? select.options[i].value : select.options[i].text;
				if (txt.toUpperCase().indexOf(ac) >= 0) {
				  select.options[i].selected = true;
				  break;
				}
			  }
			}
		};
    	e.onkeydown = function(ev) { 
	  		ev = ev||window.event;
	  		key = ev.keyCode||ev.which;
	  		if(key == 13) {
			  this.blur();
	  		  return false;
	  		}
		}
}

function _update_box(s) {
	var byid = s.className=='combo';
	var rel = s.getAttribute('rel');
	var box = document.getElementsByName(rel)[0];
		if(box && s.selectedIndex>=0) {
			  var opt = s.options[s.selectedIndex];
				if(box) {
				  box.value = byid ? opt.value : opt.text;
				  box.setAttribute('_last', box.value);
				}
		}
}

function _set_combo_select(e) {
		// When combo position is changed via js (eg from searchbox)
		// no onchange event is generated. To ensure proper change 
		// signaling we must track selectedIndex in onblur handler.
		e.setAttribute('_last', e.selectedIndex);
		e.onblur = function() {
			if(this.className=='combo')
			    _update_box(this);
			if (this.selectedIndex != this.getAttribute('_last'))
				this.onchange();
		}
		e.onchange = function() {
			var s = this;
			this.setAttribute('_last', this.selectedIndex);			
			if(s.className=='combo')
			    _update_box(s);
			if(s.selectedIndex>=0) {
				 var sname = '_'+s.name+'_update';
				 var update = document.getElementsByName(sname)[0];
				 if(update) {
					    JsHttpRequest.request(update);
				} 
			}
			return true;
		}
		e.onkeydown = function(event) {
		    event = event||window.event;
		    key = event.keyCode||event.which;
		    var box = document.getElementsByName(this.getAttribute('rel'))[0];
		    if (box && key == 32 && this.className == 'combo2') {
			    this.style.display = 'none';
			    box.style.display = 'inline';
				box.value='';
				setFocus(box.name);
			    return false;
			 }
			if (this.getAttribute('aspect') == 'editable' && key==115) {
				// F4: call related database editor - not available in non-js fallback mode
				JsHttpRequest.request('_'+this.name+'_editor', this.form);
				return false; // prevent default binding
				// TODO: stopPropagation when needed
			}
		}
}		

/*
 Behaviour definitions
*/
var inserts = {
	'form': function(e) {
  		e.onkeydown = function(ev) { 
			ev = ev||window.event;
			key = ev.keyCode||ev.which;
			if((ev.ctrlKey && key == 13) || key == 27) {
				_hotkeys.alt = false;
				ev.cancelBubble = true;
    			if(ev.stopPropagation) ev.stopPropagation();
// here ctrl-enter/escape support
				ev.returnValue = false;
				return false;
			} 
			return true;
	  	}
	},
	'input': function(e) {
		if(e.onfocus==undefined) {
			e.onfocus = function() {
			    save_focus(this);
				if (this.className == 'combo') 
					this.select();
			};
		}
		if (e.className == 'combo' || e.className == 'combo2') {
				_set_combo_input(e);
		} 
		else
    		if(e.type == 'text' ) {
   	  			e.onkeydown = function(ev) { 
  					ev = ev||window.event;
  					key = ev.keyCode||ev.which;
 	  				if(key == 13) {
						if(e.className == 'searchbox') e.onblur();
						return false;
					} 
					return true;
	  			}
			}
	},
	'input.combo2,input[aspect="fallback"]': 
	function(e) {
  	    // this hides search button for js enabled browsers
	    e.style.display = 'none';
	},
	'div.js_only': 
	function(e) {
  	    // this shows divs for js enabled browsers only
	    e.style.display = 'block';
	},
//	'.ajaxsubmit,.editbutton,.navibutton': // much slower on IE7
	'button.ajaxsubmit,input.ajaxsubmit,input.editbutton,button.navibutton': 
	function(e) {
	    e.onclick = function() {
			if (this.getAttribute('aspect') == 'process')
				progbar();
		    save_focus(this);
			JsHttpRequest.request(this);
			return false;
	    }
	},
    '.amount': function(e) {
		if(e.onblur==undefined) {
  		  e.onblur = function() {
			var dec = this.getAttribute("dec");
			price_format(this.name, get_amount(this.name), dec);
		  };
		}
	},
	'.searchbox': // emulated onchange event handling for text inputs
		function(e) {
			e.setAttribute('_last_val', e.value);
			e.setAttribute('autocomplete', 'off'); //must be off when calling onblur
  		  	e.onblur = function() {
				var val = this.getAttribute('_last_val');
				if (val != this.value) {
					this.setAttribute('_last_val', this.value);
					JsHttpRequest.request('_'+this.name+'_changed', this.form);
				}
			}
/*    	  	e.onkeydown = function(ev) { 
	  			ev = ev||window.event;
	  			key = ev.keyCode||ev.which;
	  			if (key == 13 && (this.value != this.getAttribute('_last_val'))) {
			  		this.blur();
  		 	  		return false;
	  			}
		  	}
*/		},
	'select': function(e) {
		if(e.onfocus==undefined) {
			e.onfocus = function() {
			    save_focus(this);
			};
  		  var c = e.className;
		  if (c == 'combo' || c == 'combo2')
			_set_combo_select(e);
		}
	},
	'textarea,a': function(e) {
		if(e.onfocus==undefined) {
			e.onfocus = function() {
			    save_focus(this);
			};
		}
	},
	'a.printlink': 	function(l) {
		l.onclick = function() {
		    save_focus(this);
			JsHttpRequest.request(this);
			return false;
		}
	},
	'ul.ajaxtabs':	function(ul) {
	    var ulist=ul.getElementsByTagName("li");
	    for (var x=0; x<ulist.length; x++){ //loop through each LI e
		var ulistlink=ulist[x].getElementsByTagName("input")[0];
		if(ulistlink.onclick==undefined) {
// ?  var modifiedurl=ulistlink.getAttribute("href").replace(/^http:\/\/[^\/]+\//i, "http://"+window.location.hostname+"/")
		    var url = ulistlink.form.action
		    ulistlink.onclick=function(){
			_expand(this);
			return false;
		    }
		}
	    }
	},
	'#msgbox': function(e) {
	// this is to avoid changing div height after ajax update in IE7
	  e.style.display = e.innerHTML.length ? 'block' : 'none';
	}
/* TODO
	'a.date_picker':  function(e) {
	    // this un-hides data picker for js enabled browsers
	    e.href = date_picker(this.getAttribute('rel'));
	    e.style.display = '';
	    e.tabindex = -1; // skip in tabbing order
	}
*/
};
function stopEv(ev) {
			ev.returnValue = false;
			ev.cancelBubble = true;
			if(ev.preventDefault) ev.preventDefault();
			return false;
}
/*
	Modified accesskey system. While Alt key is pressed letter keys moves 
	focus to next marked link. Alt key release activates focused link.
*/
function setHotKeys() {
	document.onkeydown = function(ev) {
		ev = ev||window.event;
		key = ev.keyCode||ev.which;

		if (key == 27 && ev.altKey) { // cancel selection
			_hotkeys.alt = false;
			_hotkeys.focus = -1;
			return stopEv(ev);
		} 
		else 
		if (ev.altKey && !ev.ctrlKey && ((key>47 && key<58) || (key>64 && key<91))) {
			_hotkeys.alt = true;
			var n = _hotkeys.focus;
			var l = document.links;
			var cnt = l.length;
			key = String.fromCharCode(key);
			for (var i=0; i<cnt; i++) { 
				n = (n+1)%cnt;
				// check also if the link is visible
				if (l[n].accessKey==key && l[n].scrollWidth) {
					_hotkeys.focus = n;
	    // The timeout is needed to prevent unpredictable behaviour on IE.
					var tmp = function() {document.links[_hotkeys.focus].focus();};
					setTimeout(tmp, 0);
					break;
				}
			}
			return stopEv(ev);
		}
		return true;
	};
	document.onkeyup = function(ev) {
		if (_hotkeys.alt==true) {
			ev = ev||window.event;
			key = ev.keyCode||ev.which;

			if (key == 18) {
				_hotkeys.alt = false;
				if (_hotkeys.focus>=0) {
					var link = document.links[_hotkeys.focus];
					if (link.target=='_blank') {
//						window.open(link.href,'','toolbar=no,scrollbar=no,resizable=yes,menubar=no,width=900,height=500');
						openWindow(link.href,'_blank');
					} else
						window.location = link.href;
				}
			} 
			return stopEv(ev);
		}
		return true;
	}
}

Behaviour.register(inserts);

Behaviour.addLoadEvent(setFocus);
Behaviour.addLoadEvent(setHotKeys);
