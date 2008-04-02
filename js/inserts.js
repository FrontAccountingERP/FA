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
			}
			element.onblur = function(event) { 
			    if(this.init_size<=1)
				this.size = 1;
			}
		}
	},
	'input': function(element) { // we do not want to change focus on submit
		if(element.type!='submit' && element.onfocus==undefined) {
			element.onfocus = function() {
				document.getElementsByName('_focus')[0].value = element.name;
			};
		}
	},
	// combo: text input and related selector
	'input.combo': function(element) {
	  if(element.onkeydown==undefined) {
		  element.onkeydown=function(event) { 
			if (event.keyCode==13) event.keyCode=9;
		  };
		  element.onkeyup = function() {
			var select = document.getElementsByName(this.getAttribute('rel'))[0];
			var len = select.length;
			var ac = this.value;
			var txt;
			select.options[select.selectedIndex].selected = false;
			for (i = 0; i < len; i++) {
//			  txt = select.options[i].text;
			  txt = select.options[i].value;
			  if (txt.indexOf(ac) >= 0) {
				select.options[i].selected = true;
				break;
			  }
			}
		  };
		  element.onblur = function() {
			  var button = document.getElementsByName(this.name+'_button')[0];
			  var select = document.getElementsByName(this.getAttribute('rel'))[0];
//			  var val = select.options[select.selectedIndex].text;
			  var val = select.options[select.selectedIndex].value; TODO
			  if (this.value != "")
				  this.value = val;
			  return true;
		  };
		}
	},
	'select.combo': function(element) {
			  element.onblur = function() {
				var box = document.getElementsByName(this.getAttribute('rel'))[0];
				val = this.options[this.selectedIndex].value;
				box.value = val; 
				this.size = 1;
				return true;
			 }
	},
	'input.combo_submit': function(element) {
	    // this hides search button for js enabled browsers
	    element.style.display = 'none';
	}
};

Behaviour.register(inserts);

function setFocus(name, byId) {
  if(byId)
	input = document.getElementById(name);
  else
  	input = document.getElementsByName(name)[0];
  if(input.focus)
	input.focus();
}


Behaviour.addLoadEvent(function() {
    var inp = document.getElementsByName('_focus')[0];
if(inp!=null) {
  setFocus(inp.value, 0);
} else {
}
}
);

