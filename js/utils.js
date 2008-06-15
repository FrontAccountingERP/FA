//
//	JsHttpRequest class extensions.
//
// Main functions for asynchronus form submitions
// 	Trigger is the source of request and can have following forms:
// 	- input object - all form values are also submited
//  - arbitrary string - POST var trigger with value 1 is added to request;
//		if form parameter exists also form values are submited, otherwise
//		request is directed to current location 
// 
    JsHttpRequest.request= function(trigger, form) {

		
		var submitObj = typeof(trigger) == "string" ? 
			document.getElementsByName(trigger)[0] : trigger;
		
		form = form || (submitObj && submitObj.form);

		var url = form ? form.action : 
		  window.location.toString();

		if (!form) url = url.substring(0, url.indexOf('?'));

		var values = this.formValues(trigger, form);
		if (!submitObj) 
			values[trigger] = 1;
		// this is to avoid caching problems
		values['_random'] = Math.random()*1234567;

        JsHttpRequest.query(
            'POST '+url, // backend
	    	values,
            // Function is called when an answer arrives. 
	    function(result, errors) {
                // Write the answer.
	        if (result) {
		  	  for(var i in result ) { 
			  atom = result[i];
			  cmd = atom['n'];
			  property = atom['p'];
			  type = atom['c'];
			  id = atom['t'];
			  data = atom['data'];
//				debug(cmd+':'+property+':'+type+':'+id);
			// seek element by id if there is no elemnt with given name
			  objElement = document.getElementsByName(id)[0] || document.getElementById(id);
    		  if(cmd=='as') {
				  eval("objElement."+property+"=data;");
			  } else if(cmd=='up') {
//				if(!objElement) debug('No element "'+id+'"');
			    if (objElement.tagName == 'INPUT' || objElement.tagName == 'TEXTAREA')
				  objElement.value = data;
			    else
				  objElement.innerHTML = data; // selector, div, span etc
		  	  } else if(cmd=='di') { // disable/enable element
				  objElement.disabled = data;
			  } else if(cmd=='fc') { // set focus
				  _focus = data;
			  } else if(cmd=='js') {	// evaluate js code
				  eval(data);
			  } else if(cmd=='rd') {	// client-side redirection
				  window.location = data;
			  } else {
				  errors = errors+'<br>Unknown ajax function: '+cmd;
			}
		  }

        // Write errors to the debug div.
		  document.getElementById('msgbox').innerHTML = errors;

		  Behaviour.apply();
		  if (errors.length>0)
			window.scrollTo(0,0);
			//document.getElementById('msgbox').scrollIntoView(true);
	  // Restore focus if we've just lost focus because of DOM element refresh
		  setFocus();
		}
            },
            false  // do not disable caching
        );
    }
	// returns input field values submitted when form button 'name' is pressed
	//
	JsHttpRequest.formValues = function(inp, objForm)
	{
		var submitObj = inp;
		var q = {};
		

		if (typeof(inp) == "string")
			submitObj = document.getElementsByName(inp)[0];
		else
			submitObj = inp;
		
		objForm = objForm || (submitObj && submitObj.form);

		if (objForm)
		{
			var formElements = objForm.elements;
			for( var i=0; i < formElements.length; i++)
			{
			  var el = formElements[i];
				if (!el.name) continue;
				if (el.type )
				  if( 
				  ((el.type == 'radio' || el.type == 'checkbox') && el.checked == false)
				  || (el.type == 'submit' && (!submitObj || el.name!=submitObj.name)))
					continue;
				if (el.disabled && el.disabled == true)
					continue;
				var name = el.name;
				if (name)
				{
					if(el.type=='select-multiple')
					{
						for (var j = 0; j < el.length; j++)
						{
							if (el.options[j].selected == true)
								q[name] = el.options[j].value;
						}
					}
					else
					{
						q[name] = el.value;
					}
				} 
			}
		}
		return q;
	}
//
//	User price formatting
//
function price_format(post, num, dec, label) {
	//num = num.toString().replace(/\$|\,/g,'');
	if(isNaN(num))
		num = "0";
	sign = (num == (num = Math.abs(num)));
	if(dec<0) dec = 2;
	decsize = Math.pow(10, dec);
	num = Math.floor(num*decsize+0.50000000001);
	cents = num%decsize;
	num = Math.floor(num/decsize).toString();
	for( i=cents.toString().length; i<dec; i++){
		cents = "0" + cents;
	}
	for (var i = 0; i < Math.floor((num.length-(1+i))/3); i++)
		num = num.substring(0,num.length-(4*i+3))+user.ts+
			num.substring(num.length-(4*i+3));
	 num = ((sign)?'':'-') + num;
	 if(dec!=0) num = num + user.ds + cents;
	if(label)
	    document.getElementById(post).innerHTML = num;
	else
	    document.getElementsByName(post)[0].value = num;
}

function get_amount(doc, label) {
	    if(label)
		var val = document.getElementById(doc).innerHTML;
	    else
		var val = document.getElementsByName(doc)[0].value;
		val = val.replace(new RegExp('\\'+user.ts, 'g'),'');
		val = val.replace(new RegExp('\\'+user.ds, 'g'),'.');
		return 1*val;
}

function goBack() {
	if (window.history.length <= 1)
	 window.close();
	else
	 window.history.go(-1);
}

function setFocus(name, byId) {

  if(!name) {
	if (_focus)	
		name = _focus;	// last focus set in onfocus handlers
	else {	// no current focus -  set it from from hidden var (first page display)
	  var cur = document.getElementsByName('_focus')[0];
	  if(cur) name = cur.value;
	}
  }
  if(byId)
	el = document.getElementById(name);
  else
  	el = document.getElementsByName(name)[0];

  if(el && el.focus) {
    // The timeout is needed to prevent unpredictable behaviour on IE & Gecko.
    // Using tmp var prevents crash on IE5
	
    var tmp = function() {el.focus(); if (el.select) el.select();};
	setTimeout(tmp, 0);
  }
}
