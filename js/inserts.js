function debug(msg) {
    document.getElementById('msgbox').innerHTML=
	document.getElementById('msgbox').innerHTML+'<br>'+msg
}

function progbar(container) {
    container.innerHTML= "<center><img src='"+
	user.theme+"images/progressbar1.gif' /> "+
	user.loadtxt+"</center>";
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
	JsHttpRequest.request(tabobj.name)
  }
}

//interface for selecting a tab (plus expand corresponding content)
function expandtab(tabcontentid, tabnumber) {
  var tabs = document.getElementById(tabcontentid);
 _expand(tabs.getElementsByTagName("input")[tabnumber]);
}

function _set_combo_input(e) {
//		  e.onkeydown=function(event) { 
		  e.onblur=function(event) { 
			event = event||window.event;
			if(!this.back) {
			  var but_name = this.name.substring(0, this.name.length-4)+'button';
			  var button = document.getElementsByName(but_name)[0];
			  var select = document.getElementsByName(this.getAttribute('rel'))[0];
			  var byid = this.className=='combo';
			  if(button) { // if *_button set submit search request
			    JsHttpRequest.request(but_name);
			  }
			  return false;
			}
		  };
		  e.onkeyup = function() {
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
//	  this.lastkey = event.keyCode;
			  this.back = (ev||window.event).shiftKey; // save shift state for onblur handler
		  }
}

function _set_combo_select(e) {

		e.onblur = function(event) {
			event = event||window.event;
			if(!this.back && this.selectedIndex>=0) {
				var sname = '_'+this.name+'_update';
				var box = document.getElementsByName(this.getAttribute('rel'))[0];
				var opt = this.options[this.selectedIndex];
				var byid = this.className=='combo';
				var update = document.getElementsByName(sname)[0];
				if (opt.value != 0) {
				  if(box) box.value = byid ? opt.value : opt.text;
				  if(update) {
					if(update.className == 'combo_select') {
					  document.getElementsByName('_focus')[0].value=this.name;
					    JsHttpRequest.request(sname);
					} else {
					  update.click();
					  this.focus();
					}
				  } 
				}
				this.size = 1;
			}
				return true;
		}
		e.onchange = function() {
			if (this.options[this.selectedIndex].value==0)
				document.getElementsByName(this.getAttribute('rel'))[0].value='';
		}
/*		e.onkeydown = function(event) {
			event = event||window.event;
		    if(event.keyCode==13) {
			var box = document.getElementsByName(this.getAttribute('rel'))[0];			
			this.style.display='none';
			box.style.display='';
			this.back=true;
			box.focus();
		    }
		}
*/}

/*
 Behaviour definitions
*/
var inserts = {
    '.amount': function(element) {
		if(element.onblur==undefined) {
		  var dec = element.getAttribute("dec");
  		  element.onblur = function() {
			price_format(this.name, get_amount(this.name), dec);
		  };
		}
	},
	'select': function(element) {
		if(element.onfocus==undefined) {
			element.onfocus = function() {
				document.getElementsByName('_focus')[0].value = element.name;
			};
			element.onkeydown = function(event) { 
			  event = event||window.event;
			  this.back = event.shiftKey; // save shift state for onblur handler
			  this.lastkey = event.keyCode; 
  			  if (event.keyCode==32) {
			   if(this.init_size==undefined)
				this.init_size = this.size;
			  if(this.init_size<=1) {
			   if(this.size>1) {
				this.size = 1;
			   } else{
				var sel = this.selectedIndex;
				this.size = this.options.length;
				if(this.size>10) this.size = 10;
				this.selectedIndex = sel;
			   }
			  }
			 }
			};
			element.onblur = function() { 
			    if(this.init_size<=1)
				this.size = 1;
			};
		var c = element.className;
		if (c == 'combo' || c == 'combo2')
//		if (element.onblur==undefined) {
			_set_combo_select(element);
//		}
		}
	},
	'input': function(element) { // we do not want to change focus on submit
		if(element.type!='submit' && element.onfocus==undefined) {
			element.onfocus = function() {
				document.getElementsByName('_focus')[0].value = element.name;
			};
		  var c = element.className;
		  if (c == 'combo' || c == 'combo2') {
	  		  if(element.onkeydown==undefined) {
				_set_combo_input(element);
			  }
		  }
		}
	},
	'input.combo_submit': function(element) {
  	    // this hides search button for js enabled browsers
	    element.style.display = 'none';
	},
	'input.combo_select': function(element) {
	    // this hides select button for js enabled browsers
	    element.style.display = 'none';
	},
	'input.combo_reload': function(element) {
	    element.style.display = 'none';
	},
	'input.ajaxsubmit': function(e) {
	    e.onclick = function() {
		JsHttpRequest.request(this.name);
		return false;
	    }
	},
	'input.editbutton': function(e) {
	    e.onclick = function() {
		JsHttpRequest.request(this.name);
		return false;
	    }
	},
	'ul.ajaxtabs':	function(ul) {
	    var ulist=ul.getElementsByTagName("li");
	    for (var x=0; x<ulist.length; x++){ //loop through each LI element
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
	'input.navibutton': function(e) {
	    if(e.onclick==undefined) {
	     e.onclick = function() {
		JsHttpRequest.request(this.name);
		return false;
	     }
	    }
	}
//
/* TODO
	'a.date_picker':  function(element) {
	    // this un-hides data picker for js enabled browsers
	    element.href = date_picker(this.getAttribute('rel'));
	    element.style.display = '';
	    element.tabindex = -1; // skip in tabbing order
	}
*/
};

Behaviour.register(inserts);

function setFocus(name, byId) {
  if(byId)
	input = document.getElementById(name);
  else
  	input = document.getElementsByName(name)[0];

  if(input && input.focus)
	input.focus();
}

Behaviour.addLoadEvent(function() {
    var inp = document.getElementsByName('_focus')[0];
	  if(inp!=null) {
		setFocus(inp.value, 0);
	  }
	}
);
