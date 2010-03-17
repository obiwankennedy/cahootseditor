#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QObject>

#include "enu.h"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;


class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void collabTextChange(int pos, int charsRemoved, int charsAdded, QString data);

    void unCommentSelection();
    void shiftLeft();
    void shiftRight();

    // Find functions
    bool findAll(QString searchString, bool ignoreCase = true);
    bool findNext(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode);
    bool findPrev(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode);

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

#endif // CODEEDITOR_H
