/*
    Cahoots is a crossplatform real-time collaborative text editor.

    Copyright (C) 2010 Chris Dimpfl, Anandi Hira, David Vega

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "pythonhighlighter.h"

PythonHighlighter::PythonHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

//    functionFormat.setFontItalic(true);
//    functionFormat.setFontWeight(QFont::Bold);
    functionFormat.setForeground(Qt::darkYellow);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=[\\s]*\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

//    declarationFormat.setFontItalic(true);
    declarationFormat.setForeground(Qt::darkYellow);
    QStringList declarationPatterns;
//    So far these include string, integer, floating point, and imaginary literals
    declarationPatterns << "\\bstringliteral\\b" << "\\bstringprefix\\b"
            << "\\bshortstring\\b" << "\\blongstring\\b" << "\\bshortstringitem\\b"
            << "\\blongstringitem\\b" << "\\bshortstringchar\\b" << "\\blongstringchar\\b"
            << "\\bstringescapeseq\\b" << "\\bbytesliteral\\b" << "\\bbytesprefix\\b"
            << "\\bshortbytes\\b" << "\\blongbytes\\b" << "\\bshortbytesitem\\b"
            << "\\blongbytesitem\\b" << "\\bshortbyteschar\\b" << "\\blongbyteschar\\b"
            << "\\bbytesescapeseq\\b" << "\\binteger\\b" << "\\bdecimalinteger\\b"
            << "\\bnonzerodigit\\b" << "\\bdigit\\b" << "\\boctinteger\\b" << "\\bhexinteger\\b"
            << "\\bbininteger\\b" << "\\boctdigit\\b" << "\\bhexdigit\\b" << "\\bbindigit\\b"
            << "\\bfloatnumber\\b" << "\\bpointfloat\\b" << "\\bexponentfloat\\b"
            << "\\bintpart\\b" << "\\bfraction\\b" << "\\bexponent\\b" << "\\bimagnumber\\b";


    foreach (const QString &pattern, declarationPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = declarationFormat;
        highlightingRules.append(rule);
    }

    keywordFormat.setForeground(Qt::darkBlue);
//    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bFalse\\b" << "\\bclass\\b" << "\\bfinally\\b" << "\\bis\\b" << "\\breturn\\b"
            << "\\bTrue\\b" << "\\bdef\\b" << "\\bfrom\\b" << "\\bnonlocal\\b" << "\\bwhile\\b"
            << "\\band\\b" << "\\bdel\\b" << "\\bglobal\\b" << "\\bnot\\b" << "\\bwith\\b"
            << "\\bas\\b" << "\\belif\\b" << "\\bif\\b" << "\\bor\\b" << "\\byield\\b"
            << "\\bassert\\b" << "\\belse\\b" << "\\bimport\\b" << "\\bpass\\b"
            << "\\bbreak\\b" << "\\bexcept\\b" << "\\bin\\b" << "\\braise\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

//    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);

    singleQuotationFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("((\')(?![\'])(([^\'|\\\\]){1}|((\\\\)([\'|b|f|n|r|t|v|\"|\\\\]))|((\\\\)(([x|X])([0-9|A-F|a-f]{2}))))(\'))");
    rule.format = singleQuotationFormat;
    highlightingRules.append(rule);

    doubleQuotationFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("((\")((([^\"|\\\\])|((\\\\)([\'|b|f|n|r|t|v|\"|\\\\]))|((\\\\)(([x|X])([0-9|A-F|a-f]{2}))))*)(\"))");
    rule.format = doubleQuotationFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void PythonHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
