# Cahoots Editor #
This repository is for Cahoots, the senior project for Pepperdine University's Computer Science class of 2010. It is currently in beta.

Cahoots is an open-source crossplatform collaborative text editor. The application is written in C++, and is compiled with the Qt 4 development environment using the Qt Creator IDE available from Nokia at http://www.qtsoftware.com/downloads/.

Features:
  * Collaborative editing for documents
  * Chatting
  * Permissions management by the document owner
  * C++ and Python syntax highlighting (abstract syntax highlighter for other languages coming later)
  * Basic text manipulation (shift left/right, commenting for C++)
  * Find dialog

## Contact ##
Please send comments to [Chris Dimpfl](mailto:chris.dimpfl@gmail.com). Bug reports and feature requests are best posted on our [issue tracker](http://code.google.com/p/cahootseditor/issues/list).

## Downloads ##

  * [Cahoots.zip](http://cahootseditor.googlecode.com/files/Cahoots.zip): Windows executable build for Cahoots beta 1

  * [Cahoots.dmg](http://cahootseditor.googlecode.com/files/Cahoots.dmg): Mac universal binary build for Cahoots beta 1

  * [Cahoots.tar.gz](http://cahootseditor.googlecode.com/files/Cahoots.tar.gz): Ubuntu Linux build for Cahoots beta 1

Note: Linux version requires installation of Qt libraries.
If there are version conflicts with the libraries, it is straightforward to download the source code and build the application with Qt Creator.

Debian based systems may support  the Advanced Packaging Tool. In this case, run `sudo apt-get install libqt4-dev g++` to install the Qt libraries. _This is an incomplete list, you will also need the QtNetwork package and possibly several others. Apologies._

  * [CahootsBeta1Source.tar.gz](http://cahootseditor.googlecode.com/files/CahootsBeta1Source.tar.gz): Source code for Cahoots beta 1

Latest version uploaded 19 April 2010.

Known issues in Beta 1:
  * Help dialog's title displays "Pep8/1 Help" _(fixed in [r191](https://code.google.com/p/cahootseditor/source/detail?r=191))_
  * Preferences dialog's title displays as "Dialog" _(fixed in [r191](https://code.google.com/p/cahootseditor/source/detail?r=191))_
  * About dialog's title displays as "Dialog" _(fixed in [r191](https://code.google.com/p/cahootseditor/source/detail?r=191))_
  * Help dialog colors match Pep8 and not Cahoots _(fixed in [r191](https://code.google.com/p/cahootseditor/source/detail?r=191))_
  * Fonts not applied to documents after opening _(fixed in [r192](https://code.google.com/p/cahootseditor/source/detail?r=192))_
  * Colors mean nothing in the preferences pane
  * Preferences pane columns are too narrow on Windows
  * Potential race condition with inserting text on the same line that could cause desynchronization. **Confirmed**, temporary work around: don't edit the same line as another user.