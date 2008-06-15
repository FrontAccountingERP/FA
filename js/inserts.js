var _focus;

function debug(msg) {
    box = document.getElementById('msgbox')
	box.innerHTML= box.innerHTML+'<br>'+msg
}

function progbar(container) {
    container.innerHTML= "<center><img src='"+
	user.theme+"images/progressbar1.gif' /> "+
	user.loadtxt+"</center>";
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
		e.onblur = function() {
			if(this.className=='combo')
			    _update_box(this);
		}
		e.onchange = function() {
			var s = this;
			
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
		}
}		

/*
 Behaviour definitions
*/
var inserts = {
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
	},
	'input.combo_submit,input.combo_select,input.combo2': 
	function(e) {
  	    // this hides search button for js enabled browsers
	    e.style.display = 'none';
	},
	'input.ajaxsubmit,input.editbutton,input.navibutton': 
	function(e) {
	    e.onclick = function() {
		JsHttpRequest.request(this);
		return false;
	    }
	},
    '.amount': function(e) {
		if(e.onblur==undefined) {
		  var dec = e.getAttribute("dec");
  		  e.onblur = function() {
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
    	  	e.onkeydown = function(ev) { 
	  			ev = ev||window.event;
	  			key = ev.keyCode||ev.which;
	  			if (key == 13 && (this.value != this.getAttribute('_last_val'))) {
			  		this.blur();
  		 	  		return false;
	  			}
		  	}
		},
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

Behaviour.register(inserts);

Behaviour.addLoadEvent(setFocus);
