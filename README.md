![FrontAccounting ERP](./themes/default/images/logo_frontaccounting.jpg  "FrontAccounting ERP")
===================

FrontAccounting ERP is open source, web-based accounting software for small and medium enterprises.
It supports double entry accounting providing both low level journal entry and user friendly, document based 
interface for everyday business activity with automatic GL postings generation. This is multicurrency,
multilanguage system with active worldwide users community:

* [Project web site](http://frontaccounting.com)
* [SourceForge project page](http://sourceforge.net/projects/frontaccounting/)
* [Central users forum](http://frontaccounting.com/punbb/index.php)
* [Main code repository](https://sourceforge.net/p/frontaccounting/git/ci/master/tree/)
* [GitHub mirror](http://github.com/FrontAccountingERP/FA)
* [Mantis bugtracker](http://mantis.frontaccounting.com)
* [FrontAccounting Wiki](http://frontaccounting.com/fawiki/)

This project is developed as cooperative effort by FrontAccounting team and available under [GPL v.3 license](./doc/license.txt) 

## Requirements

To use FrontAccounting application you should have already installed: 

*   Any HTTP web server supporting php eg. _**Apache with mod_php**_ or _**IIS**_.
*   **_PHP_** >=5.0 (version 5.6 or 7.x is recommended)
*   **_MySQL_** >=4.1 server with **_Innodb_** tables enabled, or any version on **MariaDB** server
*   **_Adobe Acrobat Reader_** (or any another PDF reader like _**evince**_) is handy for viewing reports before printing them out.

## Installation
### 1. PHP configuration checks

*   One critical aspect of the PHP installation is the setting of **_session.auto_start_** in the php.ini file. Some rpm distributions of PHP have the default setting of **_session.auto_start = 1_**. This starts a new session at the beginning of each script. However, this makes it impossible to instantiate any class objects that the system relies on. Classes are used extensively by this system. When sessions are required they are started by the system and this setting of **_session.auto_start_** can and should be set to 0.
*   For security reasons both Register Globals and Magic Quotes php settings should be set to Off. When FrontAccounting is used with www server running php as Apache module, respective flags are set in .htaccess file. When your server uses CGI interface to PHP you should set  **_magic_quotes_gpc = 0_** and **_register_globals = 0_** in php.ini file.
*   **_Innodb_** tables must be enabled in the MySQL server. These tables allow database transactions which are a critical component of the software. This is enabled by default in the newer versions of MySQL. If you need to enable it yourself, consult the MySQL manual.

### 2. Download application files

* Download and unpack latest FrontAccounting tarball from SourceForge into folder created under web server document root, e.g. **/var/www/html/frontaccounting**

* If you prefer easy upgrades when new minor versions are released, you can clone sources from SourceForge project page or Github mirror e.g.:
>	# cd  /var/www/html
>	# git clone `https://git.code.sf.net/p/frontaccounting/git` frontaccounting

Master branch contains all the latest bugfixes made atop the last stable release.
	
### 3. Installation

Use your browser to open page at URL related to chosen installation folder. As an example, if you plan to use application locally and in previous step you have put files on your Linux box in /var/www/html/frontaccounting subfolder, just select `http://localhost/frontaccounting` url in your browser, and you will see start page of installation wizard. Follow instructions displayed during the process.

During installation you will need to provide data server credentials with permissions to create new database, or you will have to provide existing database name and credentials for user with valid usage permissions to access it. You will have to chose also a couple of other options including installation language, optimal encoding for database data etc. Keep in mind that some options (like additional translations and charts of accounts) presented during installation process could be installed also later, when FrontAccounting is already in use.

 After successful installation please remove or rename your install directory for safety reasons. You won't need it any more.

### 4. Logging In For the First Time

 Open a browser and enter the URL for the web server directory where FrontAccounting is installed. Enter the user name  **admin** and use password declared during install process to login as company administrator. Now you can proceed with configuration process setting up additional user accounts, creating fiscal years, defining additional currencies, GL accounts etc. All configuration options available in application are described in [FrontAccounting Wiki](http://frontaccounting.com/fawiki/) available directly from Help links on every application page under ![Help](./themes/default/images/help.gif  "Help") icon.
 

## Troubleshooting

If you encountered any problems with FrontAccounting configuration or usage, please consult your case with other users on [Frontaccounting forum](http://frontaccounting.com/punbb/index.php). If you think you have encountered a bug in application and after consulting other community members you still are sure this is really a bug, please fill in a report in project [Mantis bugtracker](http://mantis.frontaccounting.com) with all details which allow development team reproduce the problem, and hopefully fix it. Keep in mind, that  [GitHub](http://github.com/FrontAccountingERP/FA) page is mainly passive mirror for project based on SorceForge, so posting bug reports here is at least suboptimal.
