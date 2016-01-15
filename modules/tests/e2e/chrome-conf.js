// conf.js
exports.config = {
  seleniumAddress: 'http://localhost:4444/wd/hub',
  baseUrl: 'http://localhost:8000',

  // Spec patterns are relative to the location of the conf file. They may
  // include glob patterns.
  suites: {
    login: 'login/*.spec.js',
    banking: ['banking/bankDeposit.spec.js', 'banking/bankTransfer.spec.js']
  },

  // Options to be passed to Jasmine-node.
  jasmineNodeOpts: {
    showColors: true, // Use colors in the command line report.
  }

}
