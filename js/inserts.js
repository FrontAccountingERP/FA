//ajax transfer progress indicator
var starttabs = new Array();
var autoload = new Array();
var onload_script = ""
var loadstatustext="<img src='js/images/progressbar1.gif' /> Requesting content..."
var loadedobjects=""

function Querystring(qs) { // optionally pass a querystring to parse
	this.params = new Object() 
	this.get=Querystring_get
	this.set=Querystring_set
	this.href=window.location.pathname 
	this.url=Querystring_url
	
	if (qs == null)
		qs=location.search.substring(1,location.search.length)

	if (qs.length == 0) return

// Turn <plus> back to <space>
// See: http://www.w3.org/TR/REC-html40/interact/forms.html#h-17.13.4.1
	qs = qs.replace(/\+/g, ' ')
	var args = qs.split('&') // parse out name/value pairs separated via &
	
// split out each name=value pair
	for (var i=0;i<args.length;i++) {
		var value;
		var pair = args[i].split('=')
		var name = unescape(pair[0])

		if (pair.length == 2)
			value = unescape(pair[1])
		else
			value = name
		
		this.params[name] = value
	}
	
	this.page = this.params.page
	delete this.params.page
}

function Querystring_get(key, default_) {
	// This silly looking line changes UNDEFINED to NULL
	if (default_ == null) default_ = null;
	
	var value=this.params[key]
	if (value==null) value=default_;
	
	return value
}

function Querystring_set(key, value) {
	this.params[key] = value;
}

function Querystring_url() {
 var url = this.href + '?page='+ this.page
 for( key in this.params) {
	url += '&'+ key + '='+ this.params[key]
 }
 return url
}

function debug(msg) {
//alert(msg)
document.getElementById('debug').innerHTML=msg
}

function	ajaxloader(url,vars,div) {
		var container = document.getElementById(div)
		var callback=function(response,headers,context) {
		 container.innerHTML=response;
		 Behaviour.apply();
		}
 debug(url)
		container.innerHTML=loadstatustext
		ajaxCaller.postForPlainText(url,vars,callback)
}

function loadobjs(revattribute){
if (revattribute!=null && revattribute!=""){ //if "rev" attribute is defined (load external .js or .css files)
var objectlist=revattribute.split(/\s*,\s*/) //split the files and store as array
for (var i=0; i<objectlist.length; i++){
var file=objectlist[i]
var fileref=""
if (loadedobjects.indexOf(file)==-1){ //Check to see if this object has not already been added to page before proceeding
if (file.indexOf(".js")!=-1){ //If object is a js file
fileref=document.createElement('script')
fileref.setAttribute("type","text/javascript");
fileref.setAttribute("src", file);
}
else if (file.indexOf(".css")!=-1){ //If object is a css file
fileref=document.createElement("link")
fileref.setAttribute("rel", "stylesheet");
fileref.setAttribute("type", "text/css");
fileref.setAttribute("href", file);
}
}
if (fileref!=""){
document.getElementsByTagName("head").item(0).appendChild(fileref)
loadedobjects+=file+" " //Remember this object as being already added to page
}
}
}
}

function _expand(tabobj) {
var alltabs=tabobj.parentNode.parentNode.getElementsByTagName("a")

if (tabobj.getAttribute("rel")){
for (var i=0; i<alltabs.length; i++){
alltabs[i].className= "other"  //deselect all tabs
}
tabobj.className="current"
ajaxloader(tabobj.getAttribute("href"), {}, tabobj.getAttribute("rel"))
//loadobjs(tabobj.getAttribute("rev"))
}
}

function expandtab(tabcontentid, tabnumber){ //interface for selecting a tab (plus expand corresponding content)
var alltabs=document.getElementById(tabcontentid).getElementsByTagName("a")
var thetab=alltabs[tabnumber]
//debug(tabcontentid+' '+tabnumber)
if (thetab.getAttribute("rel")){
 for (var i=0; i<alltabs.length; i++){
alltabs[i].className= i==tabnumber?"current":"other"  //deselect all tabs
}
ajaxloader(thetab.getAttribute("href"), {}, thetab.getAttribute("rel"))
loadobjs(thetab.getAttribute("rev"))

}
}

//---------------------
function _setlink(element){
if (element.getAttribute("rel")){
var modifiedurl=element.getAttribute("href").replace(/^http:\/\/[^\/]+\//i, "http://"+window.location.hostname+"/")
modifiedurl +="&ajax="+element.getAttribute('id');
//debug(modifiedurl)
element.setAttribute("href", modifiedurl) //replace URL's root domain with dynamic root domain, for ajax security sake
element.onclick=function(){
ajaxloader(this.getAttribute("href"), {}, this.getAttribute("rel"))
loadobjs(this.getAttribute("rev"))
return false
}
}
}

function _settabs(tab) {
var ulist=tab.getElementsByTagName("li") //array containing the LI elements within UL
for (var x=0; x<ulist.length; x++){ //loop through each LI element
var ulistlink=ulist[x].getElementsByTagName("a")[0]
//if (ulistlink.getAttribute("rel"))
if(ulistlink.onclick==undefined) 	  {
var modifiedurl=ulistlink.getAttribute("href").replace(/^http:\/\/[^\/]+\//i, "http://"+window.location.hostname+"/")
modifiedurl += "&ajax="+ulistlink.getAttribute('id');
ulistlink.setAttribute("href", modifiedurl) //replace URL's root domain with dynamic root domain, for ajax security sake

ulistlink.onclick=function(){
_expand(this);
return false
}
if (ulistlink.className=="current"){
starttabs.push(ulistlink)
}
}
}
}

function _TableRowSelector(table,row) {
		 var sels = table.getAttribute('selector').split(',') // tablica selektorów
		 var cols = table.getElementsByTagName('th') //identyfikatory kolumn
		 var colvals = row.getElementsByTagName("td")
		 selector =''
		 for(s=0; s<sels.length; s++) {
				selector += '&' + sels[s] + '='
		  for(c=0; c<cols.length; c++) {
			 if(cols[c].getAttribute('id')==sels[s]) {
				selector += colvals[c].innerHTML
 				break;		
		   }
			}
		 }
		 return selector;
}
/*
 Ajax elements behaviour definitions
*/
var inserts = {
	'div.ajax-component': function(element) { // automatic ajax component init
	 if(element.innerHTML=='') {
		autoload.push(element.id)
	 }
	},
	
  'form.ajaxform': function(element) {
		element.onsubmit=function(){
		 var url=element.action;
		 var div=element.getAttribute("rel")
		 var submit
		 var Query = new Querystring();

//		  url.replace(/^http:\/\/[^\/]+\//i, "http://"+window.location.hostname+"/")
//		 url=window.location
		  var vars = new Array();
			for(i=0; i<element.length; i++) {
			 vars[element.elements[i].name]=element.elements[i].value;
			 if(element.elements[i].name=='submit') 
				submit=element.elements[i].id // obsolete
			}
		 Query.set('ajax', element.id);
		 Query.set('action', 'update');
		 ajaxloader(Query.url(),vars, div)
		}

		element.onreset=function(){
		 var url=element.action;
		 var div=element.getAttribute("rel")
		  url.replace(/^http:\/\/[^\/]+\//i, "http://"+window.location.hostname+"/")
		  url = url + "&ajax=" + element.id + "&action=reset";
//		  var vars = new Array();
//			for(i=0; i<element.length; i++) {
//			 vars[element.elements[i].name]=element.elements[i].value;
//			}
			var vars = {}
			ajaxloader(url,vars, div)
		}
		return false;
	 },

	'a.ajaxlink': function(element) {
	// if onclick is defined this element is initialized
		  if(element.onclick==undefined) _setlink(element);
	 },

  'ul.ajaxtabs': function(element) {
		_settabs(element)
	 },

	'table.ajaxgrid': function(element) {
		var rows = element.getElementsByTagName('tr')
		for(i=1;i<rows.length-1; i++) {
		rows[i].onmouseover=function() {this.className='row2' }
		rows[i].onmouseout=function() {this.className='row1' }
		rows[i].ondblclick=function() {
		 var table = this.parentNode.parentNode
		 var url = table.getAttribute('editor')
		 var vars = {}
		 url += '&action=select'
		 url += _TableRowSelector(table,this)

		 if( table.getAttribute('editor').indexOf('ajax=')>-1) {
			ajaxloader(url,vars, table.getAttribute('rel'))
		 } else { // this is external 'select' handler
			window.location = url;
		 }

		}
	 }
	},
  'table.ajaxgrid th':	 function(element) {
		element.onclick=function(){
		 var table = this.parentNode.parentNode.parentNode
		 var url = table.getAttribute('href')
		 var vars = {}
		 url+='&ajax='+table.id+'&action=sort&id='+ this.id
// debug(url)

		 ajaxloader(url, {}, table.getAttribute("rel"))
//		 loadobjs(this.getAttribute("href"))
		 return false
	 }
	},
	'a.ajaxgrid-navi': function(element) {
  if(element.onclick==undefined)
	 if (element.getAttribute("rel")){
		var modifiedurl=element.getAttribute("href").replace(/^http:\/\/[^\/]+\//i, "http://"+window.location.hostname+"/")
		element.setAttribute("href", modifiedurl) //replace URL's root domain with dynamic root domain, for ajax security sake
 	  if( modifiedurl.indexOf('ajax=')>-1) {
		 element.onclick=function(){ // set ajax handler 
// debug(this.getAttribute("href"))
		  ajaxloader(this.getAttribute("href"), {}, this.getAttribute("rel"))
		  loadobjs(this.getAttribute("href"))
		  return false
		 }
	  }
	 }
	},
  'a.ajaxgrid-select':	 function(element) {
 	 if(element.onclick==undefined) {
		var modifiedurl=element.getAttribute("href").replace(/^http:\/\/[^\/]+\//i, "http://"+window.location.hostname+"/")
		element.setAttribute("href", modifiedurl) //replace URL's root domain with dynamic root domain, for ajax security sake
		element.onclick=function(){
		 var row = this.parentNode.parentNode
		 var table = row.parentNode.parentNode

		 var url = this.getAttribute('href')
		 var vars = {}

		 hideddrivetip() 

//		 url += '&ajax='+table.id
		 url += _TableRowSelector(table,row)
//	 alert( this.getAttribute('rel'))
		 ajaxloader(url,vars, this.getAttribute('rel'))
		 return false
		}
	  element.tooltip = element.getAttribute('title') // save tooltip
		element.removeAttribute('title') // native tooltip off
		element.onmouseover=function() {
		 ddrivetip(this.tooltip)
		}
	  element.onmouseout=function() { 
		 hideddrivetip() 
		}
	 }
	},
  '.amount': function(element) {
		if(element.onblur==undefined) {
		  var dec = element.getAttribute("dec");
  		  element.onblur = function() {
			price_format(this.name, get_amount(this.name), dec);
		  };
		}
	}
};

Behaviour.register(inserts);
// open selected tabs on load
Behaviour.addLoadEvent(function() {
for(i=0; i<starttabs.length; i++) {
_expand(starttabs[i]);
}
}
);

Behaviour.addLoadEvent(function() {
for(i=0; i<autoload.length; i++) {
	var vars = {}
	 var Query = new Querystring();
	 Query.set('ajax', autoload[i]);
	 delete Query.params.action
//alert(Query.url())
	 ajaxloader(Query.url(), {}, autoload[i]);
}
}
);
