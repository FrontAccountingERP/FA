'use strict';

var BankJournalInquiryPage = function() {
  browser.ignoreSynchronization = true;
  browser.get('/gl/inquiry/journal_inquiry.php');

  this.reference = element(by.name('Ref'));
  this.type = element(by.name('filterType'));
  this.dateFrom = element(by.name('FromDate'));
  this.dateTo = element(by.name('ToDate'));
  this.showClosed = element(by.name('AlsoClosed'));
  this.submit = element(by.name('Search'));

  this.search = function(reference, type, from, to, showClosed) {
    if (reference) this.reference.sendKeys(reference);
    this.type.element(by.cssContainingText('option', type)).click();
    browser.sleep(700);
    this.dateFrom.clear();
    this.dateFrom.sendKeys(from);
    this.dateTo.clear();
    this.dateTo.sendKeys(to);
    if (showClosed) this.showClosed.click();
    this.submit.click();
    browser.sleep(700);
  };

  this.getTitle = function() {
    return browser.getTitle();
  };

  this.getResultRow = function(row) {
    var items = element.all(by.css('div#_journal_tbl_span tr'))
    .get(row + 1)
    .all(by.tagName('td'))
    .map(function(cellElement, cellIndex) {
      return {
        column: cellIndex,
        text: cellElement.getText()
      };
    });
    return items;
  };

}

module.exports = BankJournalInquiryPage;