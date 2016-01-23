'use strict';

var PurchasesSupplierManagePage  = require('./pages/purchasesSupplierManage.page.js');

describe('purchases supplier manage page:', function () {

  beforeEach(function () {
    browser.driver.manage().timeouts().implicitlyWait(2000);
  });

  var reference = '';
  it('supplier add ok', function () {
    var page = new PurchasesSupplierManagePage();
    expect(page.getTitle()).toEqual('Suppliers');
    page.addNew('Acme Supplier', 'acme', '01-1234-56789876-00');
//    browser.pause();
    expect(page.getNoteMessage()).toEqual("A new supplier has been added.");

  });
  it('supplier add reads back ok update ok', function () {
    var page = new PurchasesSupplierManagePage();
    // read back
    page.selectSupplier('acme');
    expect(page.getBankAccount()).toEqual('01-1234-56789876-00');
    // update
    page.update('02-1234-56789876-00');
    expect(page.getNoteMessage()).toEqual("Supplier has been updated.");
  });

});
