function allocate_all(doc) {
	var amount = get_amount('amount'+doc);
	var unallocated = get_amount('un_allocated'+doc);
	var total = get_amount('total_allocated', 1);
	var left = get_amount('left_to_allocate', 1);

	if(unallocated<amount) amount = unallocated;
	if((unallocated-amount)<=left){
	    left-=unallocated-amount;
	    total+=unallocated-amount;
	    amount=unallocated;
	}else{
	  total+=left;
	  amount+=left;
	  left=0;
	}
	price_format('amount'+doc, amount, user.pdec);
	price_format('left_to_allocate', left, user.pdec, 1);
	price_format('total_allocated', total, user.pdec, 1);
}
function allocate_none(doc) {
	amount = get_amount('amount'+doc);
	left = get_amount('left_to_allocate', 1);
	total = get_amount('total_allocated', 1);
	price_format('left_to_allocate',amount+left, user.pdec, 1);
	price_format('amount'+doc, 0, user.pdec);
	price_format('total_allocated', total-amount, user.pdec, 1);
}