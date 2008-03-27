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
		}
	},
	'input': function(element) {
		if(element.onfocus==undefined) {
			element.onfocus = function() {
				document.getElementsByName('_focus')[0].value = element.name;
			};
		}
	
	},
	'input.submit': function(element) {
		element.onfocus = function() {}	// we do not want to change focus on submit
	},
	// combo: text input and related selector in next <TD> cell
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
			  if (txt.indexOf(ac) == 0) {
				select.options[i].selected = true;
				break;
			  }
			}
		  };
		  if(element.onblur==undefined) {  // onblur can be set to submit(); here
			element.onblur = function() {
			  var select = document.getElementsByName(this.getAttribute('rel'))[0];
			  if (this.value != "")
				this.value = select.options[select.selectedIndex].value;
//					myForm.$next_name.focus();
			  return true;
			};
		  }
		}
	},
	'select.combo': function(element) {
		if(element.onchange==undefined) { 
			  element.onchange = function() {
			  var input = document.getElementsByName(this.getAttribute('rel'))[0];
				input.value = this.options[this.selectedIndex].value;
//				myForm.$next_name.focus();
				return true;
			  };
		}
	}

};

Behaviour.register(inserts);

function setFocus(name, byId) {
  if(byId)
	input = document.getElementById(name).focus();
  else
  	input = document.getElementsByName(name)[0].focus();
}

Behaviour.addLoadEvent(function() {
    var inp = document.getElementsByName('_focus')[0];
if(inp!=null) {
  setFocus(inp.value, 0);
} else {
}
}
);

