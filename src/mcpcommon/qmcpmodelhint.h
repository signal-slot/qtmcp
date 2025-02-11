// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPMODELHINT_H
#define QMCPMODELHINT_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpModelHint
    \inmodule QtMcpCommon
    \brief Hints to use for model selection.
    
    Keys not declared here are currently left unspecified by the spec and are up
    to the client to interpret.
*/
class Q_MCPCOMMON_EXPORT QMcpModelHint : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpModelHint::name
        \brief A hint for a model name.
        
        The client SHOULD treat this as a substring of a model name; for example:
         - `claude-3-5-sonnet` should match `claude-3-5-sonnet-20241022`
         - `sonnet` should match `claude-3-5-sonnet-20241022`, `claude-3-sonnet-20240229`, etc.
         - `claude` should match any Claude model
        
        The client MAY also map the string to a different provider's model name or a different model family, as long as it fills a similar niche; for example:
         - `gemini-1.5-flash` could match `claude-3-haiku-20240307`
    */
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    QMcpModelHint() : QMcpGadget(new Private) {}

    QString name() const {
        return d<Private>()->name;
    }

    void setName(const QString &name) {
        if (this->name() == name) return;
        d<Private>()->name = name;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QString name;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpModelHint)

QT_END_NAMESPACE

#endif // QMCPMODELHINT_H
