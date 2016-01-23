'use strict';

var SalesCustomerManagePage  = require('./pages/salesCustomerManage.page.js');

describe('sales customer manage page:', function () {

  beforeEach(function () {
    browser.driver.manage().timeouts().implicitlyWait(2000);
  });

  var reference = '';
  it('customer add ok', function () {
    var page = new SalesCustomerManagePage();
    expect(page.getTitle()).toEqual('Customers');
    page.addNew('Acme Customer', 'acme', '1 Acme Way\nSome Town\nSome Where', '+1 555 123 456', 'acme@example.com');
//    browser.pause();
    expect(page.getNoteMessage()).toEqual("A new customer has been added.\nA default Branch has been automatically created, please check default Branch values by using link below.");

  });
  it('customer add reads back ok update ok', function () {
    var page = new SalesCustomerManagePage();
    // read back
    page.selectCustomer('acme');
    expect(page.getAddress()).toEqual('1 Acme Way\nSome Town\nSome Where');
    // update
    page.update('2 Acme Way\nThis Town\nThis Where');
    expect(page.getNoteMessage()).toEqual("Customer has been updated.");
  });

});
