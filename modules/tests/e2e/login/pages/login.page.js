'use strict';

var LoginPage = function() {
  browser.ignoreSynchronization = true;
  browser.get('/index.php');

  this.userName = element(by.name('user_name_entry_field'));
  this.password = element(by.name('password'));
  this.submit = element(by.name('SubmitUser'));

  this.login = function(userName, password) {
    this.userName.sendKeys(userName);
    this.password.sendKeys(userName);
    this.submit.click();
    browser.sleep(700);
  };

  this.getTitle = function() {
    return browser.getTitle();
  }
}

module.exports = LoginPage;