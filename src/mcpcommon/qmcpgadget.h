// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPGADGET_H
#define QMCPGADGET_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qmetatype.h>

QT_BEGIN_NAMESPACE

#if 1
#include <QtCore/qshareddata.h>
#define SharedDataPointer QSharedDataPointer
#else

template <typename T>
class SharedDataPointer
{
public:
    typedef T Type;
    typedef T *pointer;

    void detach() { if (d && d->ref.loadRelaxed() != 1) detach_helper(); }
    T &operator*() { detach(); return *(d.get()); }
    const T &operator*() const { return *(d.get()); }
    T *operator->() { detach(); return d.get(); }
    const T *operator->() const noexcept { return d.get(); }
    operator T *() { detach(); return d.get(); }
    operator const T *() const noexcept { return d.get(); }
    T *data() { detach(); return d.get(); }
    T *get() { detach(); return d.get(); }
    const T *data() const noexcept { return d.get(); }
    const T *get() const noexcept {
        const T * ret = d.get;
        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << ret;
        return ret;
    }
    const T *constData() const noexcept { return d.get(); }
    T *take() noexcept { return std::exchange(d, nullptr).get(); }

    Q_NODISCARD_CTOR
    SharedDataPointer() noexcept : d(nullptr) { }
    ~SharedDataPointer() { if (d && !d->ref.deref()) delete d.get(); }

    Q_NODISCARD_CTOR
        explicit SharedDataPointer(T *data) noexcept : d(data)
    { if (d) d->ref.ref(); }
    Q_NODISCARD_CTOR
    SharedDataPointer(T *data, QAdoptSharedDataTag) noexcept : d(data)
    {}
    Q_NODISCARD_CTOR
    SharedDataPointer(const SharedDataPointer &o) noexcept : d(o.d)
    { if (d) d->ref.ref(); }

    void reset(T *ptr = nullptr) noexcept
    {
        if (ptr != d.get()) {
            if (ptr)
                ptr->ref.ref();
            T *old = std::exchange(d, Qt::totally_ordered_wrapper(ptr)).get();
            if (old && !old->ref.deref()) {
                qDebug() << __PRETTY_FUNCTION__ << "deleting d" << old;
                delete old;
            }
        }
    }

    SharedDataPointer &operator=(const SharedDataPointer &o) noexcept
    {
        reset(o.d.get());
        return *this;
    }
    inline SharedDataPointer &operator=(T *o) noexcept
    {
        reset(o);
        return *this;
    }
    Q_NODISCARD_CTOR
    SharedDataPointer(SharedDataPointer &&o) noexcept : d(std::exchange(o.d, nullptr)) {}
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(SharedDataPointer)

    operator bool () const noexcept { return d != nullptr; }
    bool operator!() const noexcept { return d == nullptr; }

    void swap(SharedDataPointer &other) noexcept
    { qt_ptr_swap(d, other.d); }

protected:
    T *clone();

private:
    friend bool comparesEqual(const SharedDataPointer &lhs, const SharedDataPointer &rhs) noexcept
    { return lhs.d == rhs.d; }
    friend Qt::strong_ordering
    compareThreeWay(const SharedDataPointer &lhs, const SharedDataPointer &rhs) noexcept
    { return Qt::compareThreeWay(lhs.d, rhs.d); }
    Q_DECLARE_STRONGLY_ORDERED(SharedDataPointer)

    friend bool comparesEqual(const SharedDataPointer &lhs, const T *rhs) noexcept
    { return lhs.d == rhs; }
    friend Qt::strong_ordering
    compareThreeWay(const SharedDataPointer &lhs, const T *rhs) noexcept
    { return Qt::compareThreeWay(lhs.d, rhs); }
    Q_DECLARE_STRONGLY_ORDERED(SharedDataPointer, T*)

    friend bool comparesEqual(const SharedDataPointer &lhs, std::nullptr_t) noexcept
    { return lhs.d == nullptr; }
    friend Qt::strong_ordering
    compareThreeWay(const SharedDataPointer &lhs, std::nullptr_t) noexcept
    { return Qt::compareThreeWay(lhs.d, nullptr); }
    Q_DECLARE_STRONGLY_ORDERED(SharedDataPointer, std::nullptr_t)

    void detach_helper();

    Qt::totally_ordered_wrapper<T *> d;
};

// Declared here and as Q_OUTOFLINE_TEMPLATE to work-around MSVC bug causing missing symbols at link time.
template <typename T>
Q_INLINE_TEMPLATE T *SharedDataPointer<T>::clone()
{
    T *ret = new T(*d);
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << ret;
    return ret;
}

template <typename T>
Q_OUTOFLINE_TEMPLATE void SharedDataPointer<T>::detach_helper()
{
    T *x = clone();
    x->ref.ref();
    if (!d.get()->ref.deref()) {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "deleting d" << d.get();
        delete d.get();
    }
    d.reset(x);
}
#endif
class Q_MCPCOMMON_EXPORT QMcpGadget
{
    Q_GADGET
protected:
    class Private : public QSharedData
    {
    public:
        virtual ~Private() = default;
        virtual Private *clone() const { return new Private(*this); }
    };

    explicit QMcpGadget(Private *d)
        : data(d)
    {}

public:
    QMcpGadget() : data(new Private) {}
    virtual ~QMcpGadget() = default;
    QMcpGadget &operator=(const QMcpGadget &) = default;
    void swap(QMcpGadget &other) { data.swap(other.data); }

    bool operator!=(const QMcpGadget &other) const {
        return !operator==(other);
    }

    bool operator==(const QMcpGadget &other) const {
        if (typeid(*this) != typeid(other)) {
            return false;
        }
        if (data == other.data) {
            return true;
        }
        const auto mo = metaObject();
        for (int i = 0; i < mo->propertyCount(); ++i) {
            const auto property = mo->property(i);
            const auto value = property.readOnGadget(&other);
            if (value != property.readOnGadget(this)) {
                return false;
            }
        }
        return true;
    }

    virtual bool fromJsonObject(const QJsonObject &object);
    virtual QJsonObject toJsonObject() const;
    virtual const QMetaObject* metaObject() const { return &staticMetaObject; }

protected:
    template<typename DerivedData>
    const DerivedData *d() const {
        const DerivedData *ret = static_cast<const DerivedData *>(data.constData());
        // qDebug() << __PRETTY_FUNCTION__ << __LINE__ << ret;
        return ret;
    }

    template<typename DerivedData>
    DerivedData *d() {
        // do detach() manually
        // Do not use d->ref or not call d->clone(). They detach implicitly.
        const auto ref = data.constData()->ref.loadRelaxed();
        // qDebug() << __PRETTY_FUNCTION__ << __LINE__ << data.constData() << ref;
        if (ref != 1) {
            auto d = static_cast<DerivedData *>(data.constData()->clone());
            // qDebug() << __PRETTY_FUNCTION__ << __LINE__ << d;
            data.reset(d);
        }
        DerivedData *ret = static_cast<DerivedData *>(data.data());
        // qDebug() << __PRETTY_FUNCTION__ << __LINE__ << ret;
        return ret;
    }

private:
    SharedDataPointer<Private> data;

    template <typename T>
    friend auto operator<<(QDebug debug, const T &gadget) -> std::enable_if_t<std::is_base_of_v<QMcpGadget, T>, QDebug> {
        QDebugStateSaver saver(debug);
        static const auto mo = gadget.metaObject();
        debug.nospace() << mo->className() << '(';
        for (int i = 0; i < mo->propertyCount(); ++i) {
            if (i > 0) {
                debug.nospace() << ", ";
            }
            const auto property = mo->property(i);
            const auto value = property.readOnGadget(&gadget);
            debug.nospace() << property.name() << ": " << value;
        }
        debug.nospace() << ')';
        return debug;
    }
};

QT_END_NAMESPACE

#endif // QMCPGADGET_H
