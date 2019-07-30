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
function focus_alloc(i) {
    save_focus(i);
	i.setAttribute('_last', get_amount(i.name));
}

function blur_alloc(i) {
		var change = get_amount(i.name);
		
		if (i.name != 'amount' && i.name != 'charge' && i.name != 'discount')
			change = Math.min(change, get_amount('maxval'+i.name.substr(6), 1))

		price_format(i.name, change, user.pdec);
		if (i.name != 'amount' && i.name != 'charge') {
			if (change<0) change = 0;
			change = change-i.getAttribute('_last');
			if (i.name == 'discount') change = -change;

			var total = get_amount('amount')+change;
			price_format('amount', total, user.pdec, 0);
		}
}

function update_totals() {
	var amount = 0;
	var discount = 0;

	for (var i=0; i<docs.length; i++) {
		amount += get_amount('amount'+docs[i])
		if (document.getElementsByName('early_disc'+docs[i])[0].checked 
			&& (get_amount('un_allocated'+docs[i]) == get_amount('amount'+docs[i])))
				discount += get_amount('early_disc'+docs[i]);
	}
	price_format('amount', amount-discount, user.pdec);
	price_format('discount', discount, user.pdec);
	
}

function allocate_all(doc) {
	var unallocated = get_amount('maxval'+doc, 1);
	price_format('amount'+doc, unallocated, user.pdec);
	update_totals();
}

function allocate_none(doc) {
	price_format('amount'+doc, 0, user.pdec);
	update_totals();
}


var allocations = {
	'.amount': function(e) {
 		if(e.name == 'allocated_amount' || e.name == 'bank_amount')
 		{
  		  e.onblur = function() {
			var dec = this.getAttribute("dec");
			price_format(this.name, get_amount(this.name), dec);
		  };
 		} else {
			e.onblur = function() {
				blur_alloc(this);
			};
			e.onfocus = function() {
				focus_alloc(this);
			};
		}
	},
	'.check':function(e) {
		e.onclick = function() {
			update_totals();
		}
	}
}

Behaviour.register(allocations);
