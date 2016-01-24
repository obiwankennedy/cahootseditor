# -------------------------------------------------
# Project created by QtCreator 2009-09-14T19:31:57
# -------------------------------------------------
TARGET = cahoots
TEMPLATE = app

QT += gui network widgets printsupport \
    webkit webkitwidgets

# Windows RC file
RC_FILE = cahootsresources.rc

# Mac icon/plist
ICON = images/icon.icns

QMAKE_INFO_PLIST = app.plist

SOURCES += main.cpp \
    mainwindow.cpp \
    document.cpp \
    highlighters/cpphighlighter.cpp \
    connecttodocument.cpp \
    finddialog.cpp \
    preferencesdialog.cpp \
    codeeditor.cpp \
    participantspane.cpp \
    chatpane.cpp \
    client.cpp \
    findtoolbar.cpp \
    server.cpp \
    chatbrowser.cpp \
    utilities.cpp \
    highlighters/pythonhighlighter.cpp \
    announcedocumentdialog.cpp \
    firstrundialog.cpp \
    aboutdialog.cpp \
    helpdialog.cpp
HEADERS += mainwindow.h \
    document.h \
    highlighters/cpphighlighter.h \
    connecttodocument.h \
    finddialog.h \
    preferencesdialog.h \
    codeeditor.h \
    participantspane.h \
    chatpane.h \
    client.h \
    findtoolbar.h \
    enu.h \
    server.h \
    chatbrowser.h \
    utilities.h \
    highlighters/pythonhighlighter.h \
    announcedocumentdialog.h \
    firstrundialog.h \
    aboutdialog.h \
    helpdialog.h

FORMS += mainwindow.ui \
    document.ui \
    connecttodocument.ui \
    finddialog.ui \
    preferencesdialog.ui \
    participantspane.ui \
    chatpane.ui \
    findtoolbar.ui \
    announcedocumentdialog.ui \
    firstrundialog.ui \
    aboutdialog.ui \
    helpdialog.ui


RESOURCES += cahootsresc.qrc \
    highlighters.qrc \
    helpresources.qrc

OTHER_FILES += help/collaboration.html
