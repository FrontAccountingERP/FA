'use strict';

var BankTransferPage = function(transactionNo) {
  browser.ignoreSynchronization = true;
  var page = this;

//  http://bms.local/gl/bank_transfer.php&trans_type=4
  var url = '/gl/bank_transfer.php';
  if (transactionNo) {
    url += '?ModifyTransfer=Yes&trans_no=' + transactionNo;
  }

  this.pageElements = function() {
    page.fromAccount = element(by.name('FromBankAccount'));
    page.toAccount = element(by.name('ToBankAccount'));
    page.date = element(by.name('DatePaid'));
    page.reference = element(by.name('ref'));
    page.amount = element(by.name('amount'));
    page.bankCharge = element(by.name('charge'));
    page.memo = element(by.name('memo_'));
    page.submit = element(by.name('submit'));
  }

  this.transfer = function(from, to, date, amount, memo, bankCharge) {
    page.fromAccount.element(by.cssContainingText('option', from)).click();
    browser.sleep(700);
    page.toAccount.element(by.cssContainingText('option', to)).click();
    browser.sleep(700);
    page.date.clear();
    page.date.sendKeys(date);
    page.amount.clear();
    page.amount.sendKeys(amount);
    if (memo) {
      page.memo.clear();
      page.memo.sendKeys(memo);
    }
    if (bankCharge) {
      page.bankCharge.clear();
      page.bankCharge.sendKeys(bankCharge);
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
    return element(by.css('div.note_msg')).getText();
  };

  this.getErrorMessage = function() {
    return element(by.css('div.err_msg')).getText();
  }

  browser.get(url);
  this.pageElements();

}

module.exports = BankTransferPage;