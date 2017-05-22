'use strict';

var SalesCustomerManagePage = function() {
  browser.ignoreSynchronization = true;
  var page = this;

  //  http://localhost:8000/sales/manage/customers.php?
  var url = '/sales/manage/customers.php';
//  if (transactionNo) {
//    url += '?ModifyTransfer=Yes&trans_no=' + transactionNo;
//  }

  this.pageElements = function() {
    page.name = element(by.name('CustName'));
    page.shortName = element(by.name('cust_ref'));
    page.address = element(by.name('address'));
    page.branchPhone = element(by.name('phone'));
    page.branchEmail = element(by.name('email'));
    page.submit = element(by.name('submit'));
    
    page.select = element(by.name('customer_id'));
  }

  this.addNew = function(name, shortName, address, phone, email) {
    page.name.sendKeys(name);
    page.shortName.sendKeys(shortName);
    page.address.sendKeys(address);
    page.branchPhone.sendKeys(phone);
    page.branchEmail.sendKeys(email);
    page.submit.click();
  };
  
  this.selectCustomer = function(shortName) {
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

  this.getAddress = function() {
    this.pageElements();
    return page.address.getText();
  }

  this.update = function(address) {
    this.pageElements();
    page.address.clear();
    page.address.sendKeys(address);
    page.submit.click();
//    browser.sleep(700);
  };
  
  
  browser.get(url);
  this.pageElements();

}

module.exports = SalesCustomerManagePage;
