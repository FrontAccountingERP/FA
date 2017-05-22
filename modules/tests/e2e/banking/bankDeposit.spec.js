'use strict';

var BankDepositPage  = require('./pages/bankDeposit.page.js');
var BankJournalInquiryPage  = require('./pages/bankJournalInquiry.page.js');
var BankAccountInquiryPage  = require('./pages/bankAccountInquiry.page.js');

describe('bank deposit page:', function () {

  beforeEach(function () {
    browser.driver.manage().timeouts().implicitlyWait(2000);
  });

  var reference = '';
  var trans_no = 0;
  it('deposits ok', function () {
    var page = new BankDepositPage();
    expect(page.getTitle()).toEqual('Bank Account Deposit Entry');
    expect(page.getReference()).not.toEqual('');
    page.getReference().then(function(text) {
      reference = text;
    });
    page.deposit('Petty Cash account', '1/2/2013', '100', 'Some deposit');
    expect(page.getNoteMessage()).toEqual('Deposit ' + '1' + ' has been entered');
  });
  it('reads back journal', function () {
    var pageReadBack = new BankJournalInquiryPage();
    expect(pageReadBack.getTitle()).toEqual('Journal Inquiry');
    pageReadBack.search(reference, 'Bank Deposit', '1/2/2013', '1/2/2013', null);
    var items = pageReadBack.getResultRow(0);
    items.then(function(actualItems) {
      trans_no = parseInt(actualItems[2].text, 10);
      expect(items).toEqual([
        {column: 0, text: '01/02/2013'},
        {column: 1, text: 'Bank Deposit'},
        {column: 2, text: trans_no.toString()},
        {column: 3, text: ''},
        {column: 4, text: reference},
        {column: 5, text: '100.00'},
        {column: 6, text: 'Some deposit'},
        {column: 7, text: 'test'},
        {column: 8, text: ''},
        {column: 9, text: ''}
      ]);
    });
  });
  it('reads back account', function () {
    var pageReadBack = new BankAccountInquiryPage();
    expect(pageReadBack.getTitle()).toEqual('Bank Statement');
    pageReadBack.search('Petty Cash account', '1/2/2013', '1/2/2013');
    var items = pageReadBack.getResultRow(0);
    items.then(function(actualItems) {
      trans_no = parseInt(actualItems[2].text, 10);
      expect(items).toEqual([
        {column: 0, text: 'Bank Deposit'},
        {column: 1, text: trans_no.toString()},
        {column: 2, text: reference},
        {column: 3, text: '01/02/2013'},
        {column: 4, text: '100.00'}, // debit
        {column: 5, text: ''},       // credit
        {column: 6, text: '100.00'}, // balance
        {column: 7, text: ''},
        {column: 8, text: 'Some deposit'},
        {column: 9, text: ''},
        {column: 10, text: ''}
      ]);
    });
    var balance = pageReadBack.getBalance();
    expect(balance).toEqual([
      {column: 0, text: 'Ending Balance - 01/02/2013'},
      {column: 1, text: '100.00'},
      {column: 2, text: '0.00'},
      {column: 3, text: '100.00'},
      {column: 4, text: ''}
    ]);
  });

});
