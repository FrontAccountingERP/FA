'use strict';

var BankDepositPage = function(transactionNo) {
  browser.ignoreSynchronization = true;
  var page = this;

//  http://bms.local/gl/bank_transfer.php&trans_type=4
  var url = '/gl/gl_bank.php?NewDeposit=Yes';
//  if (transactionNo) {
//    url += '?ModifyTransfer=Yes&trans_no=' + transactionNo;
//  }

  this.pageElements = function() {
    page.toAccount = element(by.name('bank_account'));
    page.date = element(by.name('date_'));
    page.reference = element(by.name('ref'));
    page.amount = element(by.name('amount'));
    page.addItemButton = element(by.name('AddItem'));
    page.memo = element(by.name('memo_'));
    page.submit = element(by.name('Process'));
  }

  this.deposit = function(to, date, amount, memo) {
    page.toAccount.element(by.cssContainingText('option', to)).click();
    browser.sleep(700);
    page.date.clear();
    page.date.sendKeys(date);
    page.amount.clear();
    page.amount.sendKeys(amount);
    page.addItemButton.click();
    browser.sleep(700);
    if (memo) {
      page.memo.clear();
      page.memo.sendKeys(memo);
    }
    page.submit.click();
    browser.sleep(700);
  };

  this.getTitle = function() {
    return browser.getTitle();
  };

  this.getReference = function() {
    return page.reference.getAttribute('value');
  };

  this.getNoteMessage = function() {
    this.pageElements();
    return element(by.css('div.note_msg')).getText();
  };

  browser.get(url);
  this.pageElements();

}

module.exports = BankDepositPage;