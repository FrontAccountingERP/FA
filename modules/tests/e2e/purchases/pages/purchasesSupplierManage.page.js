'use strict';

var PurchasesSupplierManagePage = function() {
  browser.ignoreSynchronization = true;
  var page = this;

  //  http://localhost:8000/purchasing/manage/suppliers.php?
  var url = '/purchasing/manage/suppliers.php';
//  if (transactionNo) {
//    url += '?ModifyTransfer=Yes&trans_no=' + transactionNo;
//  }

  this.pageElements = function() {
    page.name = element(by.name('supp_name'));
    page.shortName = element(by.name('supp_ref'));
    page.bankAccount = element(by.name('bank_account'));
    page.submit = element(by.name('submit'));
    
    page.select = element(by.name('supplier_id'));
  }

  this.addNew = function(name, shortName, bankAccount) {
    page.name.sendKeys(name);
    page.shortName.sendKeys(shortName);
    page.bankAccount.sendKeys(bankAccount);
    page.submit.click();
  };
  
  this.selectSupplier = function(shortName) {
    page.select.element(by.cssContainingText('option', shortName)).click();
    browser.sleep(200);
  };

  this.getTitle = function() {
    return browser.getTitle();
  };

  this.getNoteMessage = function() {
    this.pageElements();
    return element(by.css('div.note_msg')).getText();
  };

  this.getBankAccount = function() {
    this.pageElements();
    return page.bankAccount.getAttribute('value');
  }

  this.update = function(bankAccount) {
    this.pageElements();
    page.bankAccount.clear();
    page.bankAccount.sendKeys(bankAccount);
    page.submit.click();
//    browser.sleep(700);
  };
  
  
  browser.get(url);
  this.pageElements();

}

module.exports = PurchasesSupplierManagePage;
