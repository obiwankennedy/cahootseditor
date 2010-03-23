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
// This is a temporary bunch of rules until I sort them into declarations and keywords
    declarationPatterns << "\\bint\\b" << "\\blong\\b" << "\\bbool\\b" << "\\bchar\\b" << "\\bconst\\b"
            << "\\bconst_cast\\b" << "\\bfloat\\b" << "\\breinterpret_cast\\b" << "\\bshort\\b" << "\\basm\\b"
            << "\\bdynamic_cast\\b" << "\\bauto\\b" << "\\bbreak\\b" << "\\bcase\\b" << "\\bcatch\\b"
            << "\\bclass\\b" << "\\bcontinue\\b" << "\\bdefault\\b" << "\\bdelete\\b" << "\\bdo\\b"
            << "\\bdouble\\b" << "\\belse\\b" << "\\benum\\b" << "\\bexplicit\\b" << "\\bexport\\b"
            << "\\bextern\\b" << "\\bfalse\\b"  << "\\bfor\\b" << "\\bfriend\\b" << "\\bgoto\\b"
            << "\\bif\\b" << "\\binline\\b" << "\\bmutable\\b" << "\\bnamespace\\b" << "\\bnew\\b"
            << "\\boperator\\b" << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b" << "\\bregister\\b"
            << "\\breturn\\b" << "\\bsigned\\b" << "\\bsizeof\\b" << "\\bstatic\\b" << "\\bstatic_cast\\b"
            << "\\bstruct\\b" << "\\bswitch\\b" << "\\btemplate\\b" << "\\bthis\\b" << "\\bthrow\\b"
            << "\\btrue\\b" << "\\btry\\b" << "\\btypedef\\b" << "\\btypeid\\b" << "\\btypename\\b"
            << "\\bunion\\b" << "\\bunsigned\\b" << "\\busing\\b" << "\\bvirtual\\b" << "\\bvoid\\b"
            << "\\bvolatile\\b" << "\\bwhile\\b";

    foreach (const QString &pattern, declarationPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = declarationFormat;
        highlightingRules.append(rule);
    }

    keywordFormat.setForeground(Qt::darkBlue);
//    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bwhile\\b" << "\\bfor\\b" << "\\bswitch\\b"
            << "\\bif\\b" << "\\bdo\\b" << "\\bnew\\b"
            << "\\breturn\\b" << "\\belse\\b";
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
