#ifndef HIGHLIGHTER_HPP
#define HIGHLIGHTER_HPP
//实现代码语法高亮
#include "base.hpp"

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Highlighter(QTextDocument *parent = nullptr)
        : QSyntaxHighlighter(parent)
    {
        HighlightingRule rule;

        keywordFormat.setForeground(Qt::darkBlue);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns;
        keywordPatterns
                        << "\\bchar\\b"    << "\\bclass\\b"      << "\\bconst\\b"
                        << "\\bdouble\\b"  << "\\benum\\b"       << "\\bexplicit\\b"
                        << "\\bfriend\\b"  << "\\binline\\b"     << "\\bint\\b"
                        << "\\blong\\b"    << "\\bnamespace\\b"  << "\\boperator\\b"
                        << "\\bprivate\\b" << "\\bprotected\\b"  << "\\bpublic\\b"
                        << "\\bshort\\b"   << "\\bsignals\\b"    << "\\bsigned\\b"
                        << "\\bslots\\b"   << "\\bstatic\\b"     << "\\bstruct\\b"
                        << "\\btemplate\\b"<< "\\btypedef\\b"    << "\\btypename\\b"
                        << "\\bunion\\b"   << "\\bunsigned\\b"   << "\\bvirtual\\b"
                        << "\\bvoid\\b"    << "\\bvolatile\\b"   << "\\breturn\\b"
                        << "\\bcin\\b"     << "\\bcout\\b"       << "\\basm\\b"
                        << "\\bif\\b"      << "\\breturn\\b"     <<"\\btypedef\\b"
                        << "\\binline\\b"  <<"\\bshort\\b"       << "\\bbool\\b"
                        << "\\bdynamic_cast\\b"<<"\\btypename\\b"<< "\\bbreak\\b"
                        <<"\\belse\\b"     << "\\bdo\\b"         << "\\bauto\\b"
                        <<"\\btypeid\\b"   << "\\bsizeof\\b"     << "\\bcase\\b"
                        <<"\\bsigned\\b"   <<"\\blong\\b"        <<"\\bmutable\\b"
                        <<"\\bstatic\\b"   <<"\\bunsigned\\b"    <<"\\bcatch\\b"
                        <<"\\bstatic_cast\\b"<<"\\busing\\b"     <<"\\bchar\\b"
                        <<"\\bexport\\b"   <<"\\bnew\\b"         <<"\\bstruct\\b"
                        <<"\\bvirtual\\b"  <<"\\bclass\\b"       <<"\\bextern\\b"
                        <<"\\boperator\\b" <<"\\bswitch\\b"      <<"\\bvoid\\b"
                        <<"\\bconst\\b"    <<"\\bfalse\\b"       <<"\\bprivate\\b"
                        <<"\\btemplate\\b" <<"\\bvolatile\\b"    <<"\\bconst_cast\\b"
                        <<"\\bfloat\\b"    <<"\\bprotected\\b"   <<"\\bthis\\b"
                        <<"\\bwchar_t\\b"  <<"\\bcontinue\\b"    <<"\\bfor\\b"
                        <<"\\bpublic\\b"   <<"\\bthrow\\b"       <<"\\bwhile\\b"
                        <<"\\bdefault\\b"  <<"\\bregister\\b"    <<"\\btrue\\b"
                        <<"\\bdelete\\b"   <<"\\bgoto\\b"        <<"\\breinterpret_cast\\b"
                        <<"\\btry\\b";
        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }
        classFormat.setFontWeight(QFont::Bold);
        classFormat.setForeground(Qt::darkMagenta);
        rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");//所有的Qt类
        rule.format = classFormat;
        highlightingRules.append(rule);

        singleLineCommentFormat.setForeground(Qt::red);
        rule.pattern = QRegExp("//[^\n]*");//所有注释
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        multiLineCommentFormat.setForeground(Qt::red);

        quotationFormat.setForeground(Qt::darkGreen);
        rule.pattern = QRegExp("\".*\"");//所有的头文件
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        functionFormat.setFontItalic(true);
        functionFormat.setForeground(Qt::blue);
        rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");//所有（前的单词
        rule.format = functionFormat;
        highlightingRules.append(rule);

        commentStartExpression = QRegExp("/\\*");
        commentEndExpression = QRegExp("\\*/");
    }
protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE
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
private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};


#endif // HIGHLIGHTER_HPP
