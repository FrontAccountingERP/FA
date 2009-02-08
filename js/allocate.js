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
		price_format(i.name, change, user.pdec);		
		if(change<0) change = 0;
		change = change-i.getAttribute('_last');
	var total = get_amount('total_allocated', 1)+change;
	var left = get_amount('left_to_allocate', 1)-change;
	
	price_format('left_to_allocate', left, user.pdec, 1, 1);
	price_format('total_allocated', total, user.pdec, 1, 1);
}

function allocate_all(doc) {
	var amount = get_amount('amount'+doc);
	var unallocated = get_amount('un_allocated'+doc);
	var total = get_amount('total_allocated', 1);
	var left = get_amount('left_to_allocate', 1);
	total -=  (amount-unallocated);
	left += (amount-unallocated);
	amount = unallocated;
	if(left<0) {
		total  += left;
		amount += left;
		left = 0;
	}
	price_format('amount'+doc, amount, user.pdec);
	price_format('left_to_allocate', left, user.pdec, 1,1);
	price_format('total_allocated', total, user.pdec, 1, 1);
}

function allocate_none(doc) {
	amount = get_amount('amount'+doc);
	left = get_amount('left_to_allocate', 1);
	total = get_amount('total_allocated', 1);
	price_format('left_to_allocate',amount+left, user.pdec, 1, 1);
	price_format('amount'+doc, 0, user.pdec);
	price_format('total_allocated', total-amount, user.pdec, 1, 1);
}

var allocations = {
	'.amount': function(e) {
		e.onblur = function() {
			blur_alloc(this);
		  };
		e.onfocus = function() {
			focus_alloc(this);
		};
	}
}

Behaviour.register(allocations);
