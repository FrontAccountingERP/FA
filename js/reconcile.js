function focus_amount(i) {
    save_focus(i);
	i.setAttribute('_last', get_amount(i.name));
}

function blur_amount(i) {
	var change = get_amount(i.name);

	price_format(i.name, change, user.pdec);
	change = change-i.getAttribute('_last');
	if (i.name=='beg_balance')
		change = -change;

	price_format('difference', get_amount('difference',1,1)+change, user.pdec);
}

var balances = {
	'.amount': function(e) {
		e.onblur = function() {
			blur_amount(this);
		  };
		e.onfocus = function() {
			focus_amount(this);
		};
	}
}

Behaviour.register(balances);
