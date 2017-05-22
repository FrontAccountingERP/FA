'use strict';

var BankAccountInquiryPage = function() {
  browser.ignoreSynchronization = true;
  var page = this;

  this.pageElements = function() {
    page.account = element(by.name('bank_account'));
    page.dateFrom = element(by.name('TransAfterDate'));
    page.dateTo = element(by.name('TransToDate'));
    page.submit = element(by.name('Show'));
  }

  this.search = function(account, from, to) {
    page.account.element(by.cssContainingText('option', account)).click();
    browser.sleep(700);
    page.dateFrom.clear();
    page.dateFrom.sendKeys(from);
    page.dateTo.clear();
    page.dateTo.sendKeys(to);
    page.submit.click();
    browser.sleep(700);
    this.pageElements();
  };

  this.getTitle = function() {
    return browser.getTitle();
  };

  this.getResultRow = function(row) {
    this.pageElements();
    var items = element.all(by.css('div#trans_tbl tr'))
    .get(row + 2)
    .all(by.tagName('td'))
    .map(function(cellElement, cellIndex) {
      return {
        column: cellIndex,
        text: cellElement.getText()
      };
    });
    return items;
  };

  this.getBalance = function() {
    this.pageElements();
    var items = element.all(by.css('div#trans_tbl tr.inquirybg'))
    .get(1)
    .all(by.tagName('td'))
    .map(function(cellElement, cellIndex) {
      return {
        column: cellIndex,
        text: cellElement.getText()
      };
    });
    return items;
  }

  browser.get('/gl/inquiry/bank_inquiry.php');
  this.pageElements();
}

module.exports = BankAccountInquiryPage;