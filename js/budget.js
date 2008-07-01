function focus_budget(i) {
    save_focus(i);
	i.setAttribute('_last', get_amount(i.name));
}

function blur_budget(i) {
	var amount = get_amount(i.name);
	var total = get_amount('Total', 1);
	
	if(amount<0) amount = 0;		
	price_format(i.name, amount, 0);
	price_format('Total', total+amount-i.getAttribute('_last'), 0, 1, 1);
}


var budget_calc = {
	'.amount': function(e) {
		e.onblur = function() {
			blur_budget(this);
		  };
		e.onfocus = function() {
			focus_budget(this);
		};
	}
}

Behaviour.register(budget_calc);
