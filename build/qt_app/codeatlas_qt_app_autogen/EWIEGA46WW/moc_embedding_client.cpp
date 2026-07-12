/****************************************************************************
** Meta object code from reading C++ file 'embedding_client.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../qt_app/embedding_client.hpp"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'embedding_client.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN9codeatlas15EmbeddingClientE_t {};
} // unnamed namespace

template <> constexpr inline auto codeatlas::EmbeddingClient::qt_create_metaobjectdata<qt_meta_tag_ZN9codeatlas15EmbeddingClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "codeatlas::EmbeddingClient",
        "embeddings_ready",
        "",
        "std::vector<std::vector<float>>",
        "embeddings",
        "progress_updated",
        "current",
        "total",
        "error_occurred",
        "error_msg",
        "on_finished"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'embeddings_ready'
        QtMocHelpers::SignalData<void(const std::vector<std::vector<float>> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'progress_updated'
        QtMocHelpers::SignalData<void(int, int)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 }, { QMetaType::Int, 7 },
        }}),
        // Signal 'error_occurred'
        QtMocHelpers::SignalData<void(const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Slot 'on_finished'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<EmbeddingClient, qt_meta_tag_ZN9codeatlas15EmbeddingClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject codeatlas::EmbeddingClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9codeatlas15EmbeddingClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9codeatlas15EmbeddingClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9codeatlas15EmbeddingClientE_t>.metaTypes,
    nullptr
} };

void codeatlas::EmbeddingClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<EmbeddingClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->embeddings_ready((*reinterpret_cast<std::add_pointer_t<std::vector<std::vector<float>>>>(_a[1]))); break;
        case 1: _t->progress_updated((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 2: _t->error_occurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->on_finished(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (EmbeddingClient::*)(const std::vector<std::vector<float>> & )>(_a, &EmbeddingClient::embeddings_ready, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (EmbeddingClient::*)(int , int )>(_a, &EmbeddingClient::progress_updated, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (EmbeddingClient::*)(const QString & )>(_a, &EmbeddingClient::error_occurred, 2))
            return;
    }
}

const QMetaObject *codeatlas::EmbeddingClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *codeatlas::EmbeddingClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9codeatlas15EmbeddingClientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int codeatlas::EmbeddingClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void codeatlas::EmbeddingClient::embeddings_ready(const std::vector<std::vector<float>> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void codeatlas::EmbeddingClient::progress_updated(int _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void codeatlas::EmbeddingClient::error_occurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
