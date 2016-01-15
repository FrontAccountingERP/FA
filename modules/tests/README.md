# FrontAccounting Test Module

This module contains tests for the [Front Accounting](http://frontaccounting.com) web based accounting system.

Currently two types of tests are available:

1. E2E tests using the protractor test framework
2. PHP unit tests using the PHPUnit test framework

The E2E tests exercise the UI much as a user would.
The PHP unit tests exercise the database back end for various functions.

### Status
The test suite is far (very far) from complete. In fact, it has only just begun.

You can have a look at our current [Code Coverage](https://rawgit.com/wiki/cambell-prince/frontaccounting/code_coverage/index.html).  The code coverage report is updated manually from time to time and may not be up to date.  The Code Coverage report only reflects code covered by the PHPUnit tests.  It does not report on code covered by the E2E tests.

### Travis CI Integration and Automation

The tests have a travis configuration which is [running here](https://travis-ci.org/cambell-prince/frontaccounting).

Note that the travis build pulls the latest code from the branch 'master-cp' from https://github.com/cambell-prince/frontaccounting.

The travis testing is done using phantomjs on the Travis node rather than chrome, as Travis nodes are headless.

The version of webdriver currently installed is not the latest as that runs too quickly and does not work well with the ajax library used by Front Accounting.  See the .travis_yml file for details.

### Installation & Operation

#### For the PHPUnit Tests

 * Install the dependencies (if not installed)

		npm install
		composer install

 * Run the PHPUnit tests (via gulp)

		gulp test-php
    
#### For the E2E Tests

 * Install the dependencies (if not installed)

		npm install

 * Start the local php server

		sh build-startServer

 * Start the local web driver

		webdrive-manager start

 * Run the E2E tests using the Chrome web driver

		gulp test-chrome

#### Installing NodeJS and Gulp

npm is the Node Package Manager and comes installed as part of 'nodejs'.  The task runner used here is 'gulp' which can be installed via the Node Package Manager, npm.

If you don't have nodejs and npm installed you can get it on a debian / ubuntu system by:

````
apt-get install nodejs
npm install -g gulp
````