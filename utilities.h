#ifndef UTILITIES_H
#define UTILITIES_H

#include <QPlainTextEdit>
#include "codeeditor.h"

class Utilities
{
public:
    static void unCommentSelection(CodeEditor *edit);

    static void shiftLeft(CodeEditor *edit);

    static void shiftRight(CodeEditor *edit);
};

#endif // UTILITIES_H
