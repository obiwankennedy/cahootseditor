#include "utilities.h"
#include <QtGui/QPlainTextEdit>
#include <QtGui/QTextCursor>
#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>
#include "codeeditor.h"

void Utilities::unCommentSelection(CodeEditor *edit)
{
    QTextCursor cursor = edit->textCursor();
    QTextDocument *doc = cursor.document();
    cursor.beginEditBlock();

    int pos = cursor.position();
    int anchor = cursor.anchor();
    int start = qMin(anchor, pos);
    int end = qMax(anchor, pos);
    bool anchorIsStart = (anchor == start);

    QTextBlock startBlock = doc->findBlock(start);
    QTextBlock endBlock = doc->findBlock(end);

    if (end > start && endBlock.position() == end) {
        --end;
        endBlock = endBlock.previous();
    }

    bool doCStyleUncomment = false;
    bool doCStyleComment = false;
    bool doCppStyleUncomment = false;

    bool hasSelection = cursor.hasSelection();

    if (hasSelection) {
        QString startText = startBlock.text();
        int startPos = start - startBlock.position();
        bool hasLeadingCharacters = !startText.left(startPos).trimmed().isEmpty();
        if ((startPos >= 2
            && startText.at(startPos-2) == QLatin1Char('/')
             && startText.at(startPos-1) == QLatin1Char('*'))) {
            startPos -= 2;
            start -= 2;
        }

        bool hasSelStart = (startPos < startText.length() - 2
                            && startText.at(startPos) == QLatin1Char('/')
                            && startText.at(startPos+1) == QLatin1Char('*'));


        QString endText = endBlock.text();
        int endPos = end - endBlock.position();
        bool hasTrailingCharacters = !endText.left(endPos).remove(QLatin1String("//")).trimmed().isEmpty()
                                     && !endText.mid(endPos).trimmed().isEmpty();
        if ((endPos <= endText.length() - 2
            && endText.at(endPos) == QLatin1Char('*')
             && endText.at(endPos+1) == QLatin1Char('/'))) {
            endPos += 2;
            end += 2;
        }

        bool hasSelEnd = (endPos >= 2
                          && endText.at(endPos-2) == QLatin1Char('*')
                          && endText.at(endPos-1) == QLatin1Char('/'));

        doCStyleUncomment = hasSelStart && hasSelEnd;
        doCStyleComment = !doCStyleUncomment && (hasLeadingCharacters || hasTrailingCharacters);
    }

    if (doCStyleUncomment) {
        cursor.setPosition(end);
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 2);
        cursor.removeSelectedText();
        cursor.setPosition(start);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 2);
        cursor.removeSelectedText();
    } else if (doCStyleComment) {
        cursor.setPosition(end);
        cursor.insertText(QLatin1String("*/"));
        cursor.setPosition(start);
        cursor.insertText(QLatin1String("/*"));
    } else {
        endBlock = endBlock.next();
        doCppStyleUncomment = true;
        for (QTextBlock block = startBlock; block != endBlock; block = block.next()) {
            QString text = block.text();
            if (!text.trimmed().startsWith(QLatin1String("//"))) {
                doCppStyleUncomment = false;
                break;
            }
        }
        for (QTextBlock block = startBlock; block != endBlock; block = block.next()) {
            if (doCppStyleUncomment) {
                QString text = block.text();
                int i = 0;
                while (i < text.size() - 1) {
                    if (text.at(i) == QLatin1Char('/')
                        && text.at(i + 1) == QLatin1Char('/')) {
                        cursor.setPosition(block.position() + i);
                        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 2);
                        cursor.removeSelectedText();
                        break;
                    }
                    if (!text.at(i).isSpace())
                        break;
                    ++i;
                }
            } else {
                cursor.setPosition(block.position());
                cursor.insertText(QLatin1String("//"));
            }
        }
    }

    // adjust selection when commenting out
    if (hasSelection && !doCStyleUncomment && !doCppStyleUncomment) {
        cursor = edit->textCursor();
        if (!doCStyleComment)
            start = startBlock.position(); // move the double slashes into the selection
        int lastSelPos = anchorIsStart ? cursor.position() : cursor.anchor();
        if (anchorIsStart) {
            cursor.setPosition(start);
            cursor.setPosition(lastSelPos, QTextCursor::KeepAnchor);
        } else {
            cursor.setPosition(lastSelPos);
            cursor.setPosition(start, QTextCursor::KeepAnchor);
        }
        edit->setTextCursor(cursor);
    }

    cursor.endEditBlock();
}

void Utilities::shiftLeft(CodeEditor *editor)
{
    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection()) {
        int start = cursor.selectionStart();
        int end = cursor.selectionEnd();
        cursor.setPosition(start);
        int i = cursor.position();
        cursor.beginEditBlock();
        while ( i < end) {
            cursor.movePosition(QTextCursor::StartOfLine);
            cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
            QString line = cursor.selectedText();
            cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            if (line.startsWith("    ")) {
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 4);
                cursor.removeSelectedText();
                end -= 4;
            }
            else if (line.startsWith("\t")) {
                cursor.deleteChar();
            }
            cursor.movePosition(QTextCursor::EndOfLine);
            if (cursor.atEnd()) {
                break;
            } else {
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.movePosition(QTextCursor::Down);
                i = cursor.position();
            }
        }
        cursor.endEditBlock();
    } else {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        QString line = cursor.selectedText();
        cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        if (line.startsWith("    ")) {
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 4);
            cursor.removeSelectedText();
        }
        else if (line.startsWith("\t")) {
            cursor.deleteChar();
        }
    }

}

void Utilities::shiftRight(CodeEditor *editor)
{
    QTextCursor cursor = editor->textCursor();
    int end = cursor.selectionEnd();
    int start = cursor.selectionStart();
    if (cursor.hasSelection()) {
        cursor.setPosition(start);
        int i = cursor.position();
        cursor.beginEditBlock();
        while (i < end) {
            cursor.insertText("    ");
            end += 4;
            cursor.movePosition(QTextCursor::EndOfLine);
            if (cursor.atEnd()) {
                break;
            } else {
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.movePosition(QTextCursor::Down);
                i = cursor.position();
            }
        }
        cursor.endEditBlock();
    } else {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.insertText("    ");
    }
    cursor.setPosition(start);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    editor->setTextCursor(cursor);
}
