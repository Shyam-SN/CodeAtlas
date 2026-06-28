/****************************************************************************
** Meta object code from reading C++ file 'ai_panel.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../qt_app/ai_panel.hpp"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ai_panel.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9codeatlas7AIPanelE_t {};
} // unnamed namespace

template <> constexpr inline auto codeatlas::AIPanel::qt_create_metaobjectdata<qt_meta_tag_ZN9codeatlas7AIPanelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "codeatlas::AIPanel",
        "highlight_nodes_requested",
        "",
        "std::vector<std::string>",
        "node_ids",
        "ask_question",
        "on_response_chunk",
        "chunk",
        "on_response_error",
        "error_msg",
        "on_response_finished",
        "on_query_embedding_ready",
        "std::vector<std::vector<float>>",
        "embeddings",
        "on_link_clicked",
        "QUrl",
        "url",
        "generate_podcast",
        "generate_study_guide",
        "play_next_podcast_line",
        "on_tts_finished",
        "exitCode",
        "exitStatus"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'highlight_nodes_requested'
        QtMocHelpers::SignalData<void(const std::vector<std::string> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Slot 'ask_question'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_response_chunk'
        QtMocHelpers::SlotData<void(const QString &)>(6, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'on_response_error'
        QtMocHelpers::SlotData<void(const QString &)>(8, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Slot 'on_response_finished'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_query_embedding_ready'
        QtMocHelpers::SlotData<void(const std::vector<std::vector<float>> &)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 12, 13 },
        }}),
        // Slot 'on_link_clicked'
        QtMocHelpers::SlotData<void(const QUrl &)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 15, 16 },
        }}),
        // Slot 'generate_podcast'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'generate_study_guide'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'play_next_podcast_line'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_tts_finished'
        QtMocHelpers::SlotData<void(int, int)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 21 }, { QMetaType::Int, 22 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AIPanel, qt_meta_tag_ZN9codeatlas7AIPanelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject codeatlas::AIPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9codeatlas7AIPanelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9codeatlas7AIPanelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9codeatlas7AIPanelE_t>.metaTypes,
    nullptr
} };

void codeatlas::AIPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AIPanel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->highlight_nodes_requested((*reinterpret_cast<std::add_pointer_t<std::vector<std::string>>>(_a[1]))); break;
        case 1: _t->ask_question(); break;
        case 2: _t->on_response_chunk((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->on_response_error((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->on_response_finished(); break;
        case 5: _t->on_query_embedding_ready((*reinterpret_cast<std::add_pointer_t<std::vector<std::vector<float>>>>(_a[1]))); break;
        case 6: _t->on_link_clicked((*reinterpret_cast<std::add_pointer_t<QUrl>>(_a[1]))); break;
        case 7: _t->generate_podcast(); break;
        case 8: _t->generate_study_guide(); break;
        case 9: _t->play_next_podcast_line(); break;
        case 10: _t->on_tts_finished((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AIPanel::*)(const std::vector<std::string> & )>(_a, &AIPanel::highlight_nodes_requested, 0))
            return;
    }
}

const QMetaObject *codeatlas::AIPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *codeatlas::AIPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9codeatlas7AIPanelE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int codeatlas::AIPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void codeatlas::AIPanel::highlight_nodes_requested(const std::vector<std::string> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
QT_WARNING_POP
