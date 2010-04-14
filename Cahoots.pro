# -------------------------------------------------
# Project created by QtCreator 2009-09-14T19:31:57
# -------------------------------------------------
QT += network
TARGET = Cahoots
TEMPLATE = app
QT += webkit
QT += xml
CONFIG += x86

# Windows RC file
RC_FILE = cahootsresources.rc

# Mac icon/plist
ICON = images/icon.icns
QMAKE_INFO_PLIST = app.plist
LIBS += /usr/lib/libsource-highlight-qt4.so
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
    aboutdialog.cpp
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
    aboutdialog.h
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
    aboutdialog.ui
RESOURCES += cahootsresc.qrc \
    highlighters.qrc
OTHER_FILES += highlighters/xml.plist \
    highlighters/vhdl.plist \
    highlighters/verilog.plist \
    highlighters/vb.plist \
    highlighters/udo.plist \
    highlighters/torquescript.plist \
    highlighters/tcltk.plist \
    highlighters/supercollider.plist \
    highlighters/stata.plist \
    highlighters/standard.plist \
    highlighters/sql.plist \
    highlighters/sml.plist \
    highlighters/shell.plist \
    highlighters/sgml.plist \
    highlighters/scala.plist \
    highlighters/ruby.plist \
    highlighters/rhtml.plist \
    highlighters/r.plist \
    highlighters/python.plist \
    highlighters/prolog.plist \
    highlighters/postscript.plist \
    highlighters/plist.plist \
    highlighters/php.plist \
    highlighters/perl.plist \
    highlighters/pdf.plist \
    highlighters/pascal.plist \
    highlighters/ox.plist \
    highlighters/objectivecaml.plist \
    highlighters/objectivec.plist \
    highlighters/nrnhoc.plist \
    highlighters/none.plist \
    highlighters/nemerle.plist \
    highlighters/mysql.plist \
    highlighters/metaslang.plist \
    highlighters/metapost.plist \
    highlighters/mel.plist \
    highlighters/matlab.plist \
    highlighters/lua.plist \
    highlighters/lsl.plist \
    highlighters/logtalk.plist \
    highlighters/lisp.plist \
    highlighters/lilypond.plist \
    highlighters/latex.plist \
    highlighters/jsp.plist \
    highlighters/javascript.plist \
    highlighters/javafx.plist \
    highlighters/java.plist \
    highlighters/idl.plist \
    highlighters/html.plist \
    highlighters/header.plist \
    highlighters/haskell.plist \
    highlighters/gnuassembler.plist \
    highlighters/gedcom.plist \
    highlighters/freefem.plist \
    highlighters/fortran.plist \
    highlighters/eztpl.plist \
    highlighters/erl.plist \
    highlighters/eiffel.plist \
    highlighters/dylan.plist \
    highlighters/d.plist \
    highlighters/css.plist \
    highlighters/csound.plist \
    highlighters/csharp.plist \
    highlighters/cpp.plist \
    highlighters/coldfusion.plist \
    highlighters/cobol.plist \
    highlighters/c.plist \
    highlighters/batch.plist \
    highlighters/awk.plist \
    highlighters/aspdotnet-vb.plist \
    highlighters/aspdotnet-cs.plist \
    highlighters/asp-vb.plist \
    highlighters/asp-js.plist \
    highlighters/asm-x86.plist \
    highlighters/asm-mips.plist \
    highlighters/applescript.plist \
    highlighters/apache.plist \
    highlighters/ampl.plist \
    highlighters/ada.plist \
    highlighters/active4d.plist \
    highlighters/actionscript3.plist \
    highlighters/actionscript.plist
