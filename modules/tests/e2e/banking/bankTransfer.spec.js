'use strict';

var BankTransferPage  = require('./pages/bankTransfer.page.js');
var BankJournalInquiryPage  = require('./pages/bankJournalInquiry.page.js');

describe('bank transfer page:', function () {

  beforeEach(function () {
    browser.driver.manage().timeouts().implicitlyWait(2000);
  });

  var reference = '';
  var trans_no = 0;
  it('transfers too much fails', function () {
    var page = new BankTransferPage();
    expect(page.getTitle()).toEqual('Bank Account Transfer Entry');
    expect(page.getReference()).not.toEqual('');
    page.getReference().then(function(text) {
      reference = text;
    });
    page.transfer('Petty Cash account', 'Current account', '1/2/2013', '120', 'Some memo', null);
    expect(page.getErrorMessage()).toEqual('This bank transfer would result in exceeding authorized overdraft limit of the account (20.00)');
  });
  it('transfers ok', function () {
    var page = new BankTransferPage();
    expect(page.getTitle()).toEqual('Bank Account Transfer Entry');
    expect(page.getReference()).not.toEqual('');
    page.getReference().then(function(text) {
      reference = text;
    });
    page.transfer('Petty Cash account', 'Current account', '1/2/2013', '70', 'Some memo', null);
    expect(page.getNoteMessage()).toEqual('Transfer has been entered');
  });
  it('reads back', function () {
    var pageReadBack = new BankJournalInquiryPage();
    expect(pageReadBack.getTitle()).toEqual('Journal Inquiry');
    pageReadBack.search(reference, 'Funds Transfer', '1/2/2013', '1/2/2013', null);
    var items = pageReadBack.getResultRow(0);
    items.then(function(actualItems) {
      trans_no = parseInt(actualItems[2].text, 10);
      expect(items).toEqual([
        {column: 0, text: '01/02/2013'},
        {column: 1, text: 'Funds Transfer'},
        {column: 2, text: trans_no.toString()},
        {column: 3, text: ''},
        {column: 4, text: reference},
        {column: 5, text: '70.00'},
        {column: 6, text: 'Some memo'},
        {column: 7, text: 'test'},
        {column: 8, text: ''},
        {column: 9, text: ''}
      ]);
    });
  });
  it('updates too much fails', function () {
    var page = new BankTransferPage(trans_no);
    expect(page.getTitle()).toEqual('Modify Bank Account Transfer');
    expect(page.getReference()).toEqual(reference);
    page.transfer('Petty Cash account', 'Current account', '1/2/2013', '110', 'Some other memo', null);
    expect(page.getErrorMessage()).toEqual('This bank transfer change would result in exceeding authorized overdraft limit (10.00) of the account \'Petty Cash account\'');
  });
  it('updates and reads back', function () {
    var page = new BankTransferPage(trans_no);
    expect(page.getTitle()).toEqual('Modify Bank Account Transfer');
    expect(page.getReference()).toEqual(reference);
    page.transfer('Petty Cash account', 'Current account', '1/2/2013', '60', 'Some other memo', null);
    expect(page.getNoteMessage()).toEqual('Transfer has been entered');

    var pageReadBack = new BankJournalInquiryPage();
    expect(pageReadBack.getTitle()).toEqual('Journal Inquiry');
    pageReadBack.search(reference, 'Funds Transfer', '1/2/2013', '1/2/2013', null);
    var items = pageReadBack.getResultRow(0);
    expect(items).toEqual([
      {column: 0, text: '01/02/2013'},
      {column: 1, text: 'Funds Transfer'},
      {column: 2, text: (trans_no + 1).toString()},
      {column: 3, text: ''},
      {column: 4, text: reference},
      {column: 5, text: '60.00'},
      {column: 6, text: 'Some other memo'},
      {column: 7, text: 'test'},
      {column: 8, text: ''},
      {column: 9, text: ''}
    ]);
  });
  it('can be voided', function () {
  });


});
