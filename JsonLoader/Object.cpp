﻿/****************************************Copyright (c)****************************************************
**
**                                       D.H. InfoTech
**
**--------------File Info---------------------------------------------------------------------------------
** File name:                  Object.cpp
** Latest Version:             V1.0.0
** Latest modified Date:       2016/3/2
** Modified by:                
** Descriptions:               
**
**--------------------------------------------------------------------------------------------------------
** Created by:                 Chen Honghao
** Created date:               2016/3/1
** Descriptions:               Object class  [Generated by VP-UML, IGT1.0]
** 
*********************************************************************************************************/
#if defined(_MSC_VER) && (_MSC_VER >= 1600)  
# pragma execution_character_set("utf-8")  
#endif

#include "Object.h"
#include "Parser.h"
#include "JsonLoader.h"

#include <QPair>
#include <QQueue>
#include <QStack>
#include <QDebug>
#include <QMetaType>
#include <QMetaObject>
#include <QMetaProperty>
#include <QtWidgets/QWidget>

/**
 * Constructor
 */
Object::Object() : m_qobject(NULL), m_parent(NULL)
{
    // TODO: Not yet implemented
}

/**
 * 向上查找指定的对象
 * @param[in]    objectName 对象名称
 * @return       查找到的对象指针，未找到则返回NULL
 */
Object* Object::findUpwards( const QString& objectName ) const
{
    // 首先判断xxx.json:obj这种特殊形式
    if (objectName.contains(":"))
    {
        Object* jsonLoader = const_cast<Object*>(this);
        while (jsonLoader->id() != "JsonLoader")
        {
            jsonLoader = jsonLoader->parent();
        }
        QStringList list = objectName.split(":");
        Object* first = NULL;
        foreach(Object* child, jsonLoader->children())
        {
            ObjectContext* oc = static_cast<ObjectContext*>(child);
            if (oc->parentKey() == list.at(0))
            {
                first = child;
                break;
            }
        }

        Object* result = NULL;
        if (NULL != first)
        {
            result =  first->findDownwards(list.at(1));
        }

        return result;
    }

    QQueue<Object*> objectQ;

    if (m_id == objectName) {
        return const_cast<Object*>(this);
    }

    if (m_parent) {
        objectQ.enqueue(const_cast<Object*>(m_parent));
    }

    const Object* object = this;
    while (object = object->m_parent)
    {
        QList<Object*>::const_iterator iter = object->m_children.cbegin();
        QList<Object*>::const_iterator cend = object->m_children.cend();
        for (iter; iter != cend; ++iter)
        {
            Object* child = (*iter);
            //QString id = object->id();
            if (child && child->id() == objectName)
            {
                return child;
            }
        }
    }

    return NULL;
}

/**
 * 向下查找指定的对象
 * @param[in]    objectName 对象名称
 * @return       查找到的对象指针，未找到则返回NULL
 */
Object* Object::findDownwards(const QString& objectName) const
{
    QQueue<Object*> objectQ;

    objectQ.enqueue(const_cast<Object*>(this));

    while (!objectQ.isEmpty())
    {
        Object* object = objectQ.dequeue();
        if (!object)
            continue;

        if (object->id() == objectName)
            return object;

        objectQ.append(object->children());
    }

    return NULL;
}

/**
 * 添加一个子对象
 * @param[in]    object 子对象
 * @return       操作成功返回true
 */
bool Object::addChild( Object* object )
{
    if (object == NULL)
        return false;

    object->m_parent = this;
    m_children.push_back(object);

    return object->setParent(this);
}

/**
 * 移除一个子对象
 * @param[in]    object 子对象
 * @return       操作成功返回true
 */
bool Object::removeChild( Object* object )
{
    if (object == NULL)
        return false;

    object->m_parent = NULL;
    int count = m_children.removeAll(object);
    return count > 0;
}

/**
 * 获取指定位置的子对象
 * @param[in]    index 子对象序号
 * @return       查找到的对象指针，未找到则返回NULL
 */
Object* Object::childAt(int index)
{
    return m_children.at(index);
}

/**
 * 设置对象对应的QObject对象
 * @param[in]    qobject QObject对象
 */
void Object::setQObject(QObject* qobject)
{
    this->m_qobject = qobject;
}

/**
 * 获取对象对应的QObject对象
 * @return    QObject对象
 */
QObject* Object::qObject() const
{
    return this->m_qobject;
}

/**
 * 设置对象的id
 * @param[in]    id 对象id
 */
bool Object::setId(QString id)
{
    this->m_id = id;

    if (m_qobject)
    {
        m_qobject->setObjectName(id);
        return true;
    }

    return false;
}

/**
 * 获取对象的id
 * @return    对象id
 */
QString Object::id() const
{
    // Optimized for searchUpwards
    if (!this->m_id.isEmpty())
        return this->m_id;

    if (this->m_qobject)
    {
        return this->m_qobject->objectName();
    }

    return QString();
}

/**
 * 设置对象的父对象
 * @param[in]    parent 父对象
 */
bool Object::setParent(Object* parent)
{
    this->m_parent = parent;

    // 如果此对象挂载在"JsonLoader"的“global”下，则不改变原有的父子关系
    if (static_cast<ObjectContext*>(this)->parentKey() == "global"
        && static_cast<ObjectContext*>(m_parent)->parentKey() == "JsonLoader")
    {
        return true;
    }

    if (this->m_qobject && parent)
    {
        if (this->m_qobject->isWidgetType()) 
        {
            if (parent->m_qobject && !parent->m_qobject->isWidgetType()) {
                return false;
            }
            ((QWidget*)this->m_qobject)->setParent((QWidget*)parent->m_qobject);
        } 
        else
        {
            if (parent->m_qobject && parent->m_qobject->isWidgetType()) {
                return false;
            }
            this->m_qobject->setParent(parent->m_qobject);
        }
    }

    return true;
}

/**
 * 获取对象的父对象
 * @return    父对象
 */
Object* Object::parent() const
{
    return this->m_parent;

}

/**
 * 获取对象的全部子对象
 * @return    全部子对象
 */
QList<Object*> Object::children() const
{
    return this->m_children;
}


ObjectContext::ObjectContext() : Object()
{

}

ObjectContext::ObjectContext( const QString& parentKey, const QJsonValue& jsonValue ) : 
    Object(),
    m_parentKey(parentKey),
    m_value(jsonValue)
{

}

KeyObjectContextMapIter ObjectContext::child( const QString& key )
{
    KeyObjectContextMapIter iter = m_keyObjectContextMap.begin();
    KeyObjectContextMapIter cend  = m_keyObjectContextMap.end();
    for (; iter != cend; ++iter)
    {
        if (iter->first == key) {
            return iter;
        }
    }

    return cend;
}

KeyObjectContextMapConstIter ObjectContext::constChild( const QString& key )
{
    KeyObjectContextMapConstIter iter = m_keyObjectContextMap.cbegin();
    KeyObjectContextMapConstIter cend  = m_keyObjectContextMap.cend();
    for (; iter != cend; ++iter)
    {
        if (iter->first == key) {
            return iter;
        }
    }

    return cend;
}

KeyObjectContextMapIter ObjectContext::addChild( const QString& key, ObjectContext* child, KeyObjectContextMapIter iter )
{
    if (!Object::addChild(child)) {
        // 仍然尝试子类的挂载操作 [3/24/2016 CHENHONGHAO]
        //return m_keyObjectContextMap.end();
    }

    if (child) {
        child->m_parent = this;
    }

    // 判断map中是否已经含有此key的pair,若存在key且child的值的类型是数组，则不必加入list中
    for (KeyObjectContextMapIter i = m_keyObjectContextMap.begin(); i != m_keyObjectContextMap.end(); ++i)
    {
        KeyObjectContextPair pair = *i;
        // 如果新key已存在
        if (pair.first == key/* && child->value().type() == QJsonValue::Array*/)
        {
            ObjectContextList list = pair.second;
            // 如果要添加的类型是array就直接return
            if (QJsonValue::Array == child->value().type())
            {
                return i;
            }
            // 如果如果此key对应的list是array，则添加到array后面
            else if (QJsonValue::Array == list.first()->value().type())
            {
                i->second.push_back(child);
                return i;
            }
        }
        // 当想在map中添加一个相同的key的时候，会将以前的pair清除
        if (pair.first == key && iter == m_keyObjectContextMap.cend())
        {
            ObjectContextList list = pair.second;
            foreach(ObjectContext* oc, list)
            {
                m_children.removeAll(oc);
            }
            m_keyObjectContextMap.erase(i);
            ObjectContextList list1;
            list1.push_back(child);
            KeyObjectContextPair pair(key, list1);
            m_keyObjectContextMap.push_back(pair);
            return m_keyObjectContextMap.end() - 1;
        }
    }

    if (iter.i == 0 || iter == m_keyObjectContextMap.cend())
    {
        ObjectContextList list;
        list.push_back(child);
        KeyObjectContextPair pair(key, list);
        m_keyObjectContextMap.push_back(pair);

        return m_keyObjectContextMap.end() - 1;
    }
    else if (iter->first == key)
    {
#if 0
        // ObjectContext的父子关系不同于QObject的父子关系，
        // 这里一个key下面允许放置多个相同的QObject，
        // 例如在使用了".ref"关键字的多个array元素有可能是完全相同的qObject [12/30/2016 CHENHONGHAO]
        foreach (const ObjectContext& object, iter->second)
        {
            if (object.qObject() == child->qObject())
            {
                
            }
        }
#endif
        iter->second.push_back(child);
        return iter;
    }

    return m_keyObjectContextMap.end();
}

bool ObjectContext::removeChild( const QString& key, ObjectContext* child )
{
    int count = 0;
    KeyObjectContextMap::iterator iter = ObjectContext::child(key);
    if (iter != m_keyObjectContextMap.end()) 
    {
        if (child) 
        {
            count = iter->second.removeAll(child);
            child->m_parent = NULL;
            if (iter->second.isEmpty()) {
                m_keyObjectContextMap.erase(iter);
            }
        } 
        else
        {
            count = iter->second.size();
            //iter->second.clear();
            m_keyObjectContextMap.erase(iter);
            // FIXME: 此处可能存在漏洞：批量删除某个key下的全部ObjectContext时，并未从Object树中移除 [5/9/2016 CHENHONGHAO]
        }
    }

    if (child && !Object::removeChild(child))
    {
        return false;
    }

    return count > 0;
}

/**
 * 删除map中对应的key,且将m_children对应的孩子删除
 */
bool ObjectContext::removeChild(const QString& key)
{
    for (KeyObjectContextMapIter i = m_keyObjectContextMap.begin(); i != m_keyObjectContextMap.end(); ++i)
    {
        KeyObjectContextPair pair = *i;
        if (pair.first == key)
        {
            ObjectContextList list = pair.second;
            foreach(ObjectContext* oc, list)
            {
                m_children.removeAll(oc);
            }
            m_keyObjectContextMap.erase(i);
            return true;

        }
    }
    return false;
}

QString ObjectContext::toString() const
{
    QString value;

    switch (m_value.type())
    {
    case QJsonValue::Bool:
        value = m_value.toBool() ? "true" : "false";
        break;

    case QJsonValue::Double:
        value = QString("%1").arg(m_value.toDouble());
        break;

    case QJsonValue::String:
        value = QString("\"") + m_value.toString() + QString("\"");
        break;

    case QJsonValue::Object:
        {
            QJsonObject object = m_value.toObject();
            value = "{";
            if (object.size() > 0) 
            {
                QString id;
                QJsonValue idValue = object.value(".id");
                QJsonValue refValue = object.value(".ref");
                QJsonValue typeValue = object.value(".type");
                if (refValue.isString())
                    value += QString("\".ref\":") + refValue.toString() + ",";
                if (idValue.isString())
                    value += QString("\".id\":") + idValue.toString() + ",";
                if (typeValue.isString())
                    value += QString("\".type\":") + typeValue.toString() + ",";
                if (value.endsWith(','))
                    value.chop(1);
            }
            value += '}';
        }
        break;
        
    default:
        // 解析之后的ObjectContext不可能出现数组类型，因为数组已经被拆分为独立的对象并展开到同级
        value = m_value.toVariant().toString();
        break;
    }

    QString qObjectInfo = "<null>";
    if (m_qobject)
    {
        const QMetaObject* metaObject = m_qobject->metaObject();
        QString className = metaObject ? metaObject->className() : "<unknown>";
        qObjectInfo = QString("%1@0x%2")
            .arg(className)
            .arg(qulonglong(m_qobject), sizeof(void*)*2, 16, QChar('0'));
    }


    return QString("{\"%1\": %2, (%3)}"/* @ 0x%4*/)
                .arg(m_parentKey)
                .arg(value)
                .arg(qObjectInfo)
                ;//.arg(qulonglong(this), sizeof(void*)*2, 16, QChar('0'));
}

void ObjectContext::dumpObjectContext( const ObjectContext& rootContext, bool recursively /*= true*/ )
{
    int indentationLevel = 0;
    
    typedef QPair<const ObjectContext*, int> DumpInfo;
    QStack< DumpInfo > contextStack;

    ObjectContext context = rootContext;

    contextStack.push(DumpInfo(&context, indentationLevel));
    while (!contextStack.isEmpty())
    {
        DumpInfo pair = contextStack.pop();
        const ObjectContext* context = pair.first;
        indentationLevel = pair.second; 
        if (context == NULL)
            continue;

        QString indentation;
        for (int i = 0; i < indentationLevel; i++)
            indentation.append(QChar('\t'));
        

        qDebug() << qPrintable(indentation)
#if 0
                 << context->m_id << "\t" 
                 << context->m_parentKey << "\t" 
                 << context->m_value;
#else
//                 << qUtf8Printable(context->toString());
                 << context->toString().toUtf8().constData();
#endif

        if (recursively)
        for (int i = context->m_keyObjectContextMap.size() - 1; i >= 0; --i)
        {
            const ObjectContextList& list = context->m_keyObjectContextMap.at(i).second;
            for (int j = list.size() - 1; j >= 0; --j)
            {
                contextStack.push(DumpInfo(list.at(j), indentationLevel + 1));
            }
        }
    }
}

QMetaProperty ObjectContext::parentProperty() const
{
    QObject* parentObject = m_parent->qObject();
    QByteArray parentKey = m_parentKey.toLatin1();
    QByteArray propertyName;
    QMetaProperty property;

    int indexOfDot = 0;
    
    while (parentObject && indexOfDot >= 0)
    {
        if ((indexOfDot = parentKey.indexOf('.', indexOfDot+1)) >= 0)
        {
            propertyName = parentKey.left(indexOfDot);
            parentKey = parentKey.mid(indexOfDot+1);
        }
        else
        {
            propertyName = parentKey;
        }

        const QMetaObject* metaObject = parentObject->metaObject();
        if (metaObject == NULL)
            return QMetaProperty();

        int propertyIndex = metaObject->indexOfProperty(propertyName.constData());
        if (propertyIndex < 0)
            return QMetaProperty();

        property = metaObject->property(propertyIndex);
        if (indexOfDot >= 0)
        {
            QVariant propertyValue = property.read(parentObject);
            parentObject = propertyValue.value<QObject*>();
        }
    }

    return property;
}

int ObjectContext::parentPropertyType() const
{
    QMetaProperty parentMetaProperty = parentProperty();

    if (parentMetaProperty.isValid()) {
        return ObjectContext::propertyType(parentMetaProperty);
    }

    return QMetaType::UnknownType;
}

QMetaProperty ObjectContext::property( const QString& key ) const
{
    if (m_qobject)
    {
        const QMetaObject* metaObject = m_qobject->metaObject();
        Q_ASSERT(metaObject);
        int propertyIndex = metaObject->indexOfProperty(key.toLatin1().constData());
        if (propertyIndex >= 0) {
            return metaObject->property(propertyIndex);
        }
    }

    return QMetaProperty();
}

int ObjectContext::propertyType( const QMetaProperty& property)
{
    // QVariant::Type与QMetaType::Type的值仅在非自定义字段时是一一对应的
    int propertyTypeId = property.userType();
    if (propertyTypeId == QMetaType::UnknownType)
    {
        // QMetaProperty::userType已经尝试自动注册，如果注册失败，则此刻已经不可挽回 [3/24/2016 CHENHONGHAO]
        return propertyTypeId;
    }
    else if (propertyTypeId < QMetaType::User) 
    {
        // [DEPRECATED] 对于标准数据类型，仅需要处理StringList，因为QObjectList并不在标注类型之列
        return propertyTypeId;
    }

    const char* propertyTypeStr = property.typeName();
    QString propertyTypeName = QString::fromLatin1(propertyTypeStr);

    int indirectLevel = 0;
    while (propertyTypeName.endsWith(QLatin1Char('*')))
    {
        // FIXME: 此处只支持一级指针，但对一般场合足够
        propertyTypeName.chop(1);
        indirectLevel++;
    }
    if (indirectLevel > 1) {
        //emit m_loader->error(JsonLoader::ObjectCreatorError, "Multi-indirect-level is NOT supported.");
        return QMetaType::UnknownType;
    }

    int metaTypeId = GET_METATYPE_ID_METHOD(propertyTypeName);
    return metaTypeId;
}

QList<QMetaMethod> ObjectContext::methods( const QString& key ) const
{
    const QMetaObject* metaObject = m_qobject ? m_qobject->metaObject() : NULL;
    if (metaObject == NULL) {
        return QList<QMetaMethod>();
    }

    int methodCount = metaObject->methodCount();
    bool fuzzyMatch = !key.contains(QLatin1Char('('));
    QList<QMetaMethod> fuzzyMatchList;
    for (int i = 0; i < methodCount; i++)
    {
        QMetaMethod method = metaObject->method(i);

        if (fuzzyMatch)
        {
            if (key == method.name()) 
                fuzzyMatchList.push_back(method);
        }
        else
        {
            if (key == method.methodSignature()) 
            {
                fuzzyMatchList.push_back(method);
                // 不使用模糊匹配时，一般仅有一个匹配结果，为了提高效率，直接返回 [5/8/2016 CHH513]
                break;
            }
        }
    }

    return fuzzyMatchList;
}


PropertyBinding::PropertyBinding( 
    QObject* observerable, const QMetaProperty& observerableProperty, 
    QObject* observer, const QMetaProperty& observerProperty,
    bool notifyOnBind // 在刚刚绑定时是否无论属性发生变更均自动notify一次
    ) : QObject(NULL),
    m_observable(observerable),
    m_observerableProperty(observerableProperty),
    m_observer(observer),
    m_observerProperty(observerProperty),
    m_bound(false)
{
    if (observerable && observer)
    {
        QMetaMethod notifier = observerableProperty.notifySignal();
        const QMetaObject* thisMetaObject = metaObject();
        int handlerIndex = thisMetaObject->indexOfMethod("onNotify()");
        QMetaMethod handler  = thisMetaObject->method(handlerIndex);
        if (notifier.isValid())
        {
            m_bound = QObject::connect(observerable, notifier, this, handler);
        }
        else
        {
            qWarning() << "Warning: Notify signal for " << observerableProperty.name() << " is not defined, "
                << "the property value will be copied only once on binding.";
        }

        if (notifyOnBind)
        {
            // property binding无论有没有notify信号，一律先手动onNotify一次，完成初始赋值
            onNotify();
        }
    }
}

PropertyBinding::~PropertyBinding()
{
    if (m_bound)
    {
        QMetaMethod notifier = m_observerableProperty.notifySignal();
        const QMetaObject* thisMetaObject = metaObject();
        int handlerIndex = thisMetaObject->indexOfMethod("onNotify()");
        QMetaMethod handler  = thisMetaObject->method(handlerIndex);
        QObject::disconnect(m_observable, notifier, this, handler);
        m_bound = false;
    }
}

PropertyBinding* PropertyBinding::bind(
    QObject* subject, const QString& subjectProperty,
    QObject* observer, const QString& observerProperty
    )
{
    const QMetaObject* subjectMetaObject = subject ? subject->metaObject() : NULL;
    const QMetaObject* observerMetaObject = observer ? observer->metaObject() : NULL;
    if (subjectMetaObject && observerMetaObject)
    {
        int subjectPropIndex = subjectMetaObject->indexOfProperty(subjectProperty.toLatin1().constData());
        int observerPropIndex = observerMetaObject->indexOfProperty(observerProperty.toLatin1().constData());
        if (subjectPropIndex >= 0 && observerPropIndex >= 0)
        {
            QMetaProperty subjectProp = subjectMetaObject->property(subjectPropIndex);
            QMetaProperty observerProp = observerMetaObject->property(observerPropIndex);
            return new PropertyBinding(subject, subjectProp, observer, observerProp);
        }
    }

    return NULL;
}

bool PropertyBinding::fullBind(QObject* obj1, const QString& property1, QObject* obj2, const QString& property2)
{
    const auto p1 = PropertyBinding::bind(obj1, property1, obj2, property2);
    const auto p2 = PropertyBinding::bind(obj2, property2, obj1, property1);
    return (nullptr != p1 && nullptr != p2);
}

bool PropertyBinding::unbind(PropertyBinding* binding)
{
    if (binding)
    {
        bool bound = binding->isBound();
        delete binding;
        return bound;
    }

    return false;
}

void PropertyBinding::onNotify() const
{
    if (m_observable && m_observer)
    {
        QVariant value = m_observerableProperty.read(m_observable);
        m_observerProperty.write(m_observer, value);
    }
}

PropertyBinding* PropertyContext::addObserver( QObject* observer, const QMetaProperty& observerProperty )
{
    PropertyBinding* connection = new PropertyBinding(
        m_qObject, m_metaProperty, observer, observerProperty
        );

    if (fullBind())
    {
        // 刚刚绑定到一起时不进行反向通知，因为刚刚正向通知完，没啥好变的 [5/26/2017 CHENHONGHAO]
        PropertyBinding* fullConnection = new PropertyBinding(
            observer, observerProperty,m_qObject, m_metaProperty, false
            );
    }

    return connection;
}

QVariant PropertyContext::read() const
{
    return m_metaProperty.read(m_qObject);
}

bool PropertyContext::write(const QVariant& value)
{
    return m_metaProperty.write(m_qObject, value);
}

int MethodConnection::connect(QObject* objectA, const QList<QMetaMethod>& methodsA, QObject* objectB, const QList<QMetaMethod>& methodsB)
{
    int count = 0;

    if (objectA == NULL || objectB == NULL)
        return 0;

    foreach (const QMetaMethod& methodA, methodsA)
    {
        QMetaMethod::MethodType typeA = methodA.methodType();
        QByteArray signatureA = methodA.methodSignature();

        foreach (const QMetaMethod& methodB, methodsB)
        {
            QByteArray signatureB = methodB.methodSignature();
            if (!matchSignature(signatureA, signatureB))
                continue;

            QMetaMethod::MethodType typeB = methodB.methodType();
            bool ok = false;
            if (typeA == QMetaMethod::Signal && 
                (typeB == QMetaMethod::Slot || typeB == QMetaMethod::Signal))
            {
                ok = QObject::connect(objectA, methodA, objectB, methodB);
            }
            else if (typeB == QMetaMethod::Signal && typeA == QMetaMethod::Slot)
            {
                ok = QObject::connect(objectB, methodB, objectA, methodA);
            }

            if (ok)
            {
                count++;
#if 1
                //  目前发现对于有默认参数的信号/槽，若多次绑定，会被多次激活，须深度了解Qt的槽的激活实现方式
                // 因此这里实际上不需要多次绑定，只需要绑定完整参数的版本，一次即可 [5/8/2016 CHH513]
                return count;
#endif
            }
            else
            {
                qWarning() 
                    << "Failed to connect signal&slot: "
                    << "ThisType=" << methodA.methodSignature()
                    << "&TargetType=" << methodB.methodSignature();
            }
        }
    }

    return count;
}

bool MethodConnection::matchSignature( const QByteArray& signatureA, const QByteArray& signatureB )
{
    const char* ptrA = signatureA.constData();
    const char* ptrB = signatureB.constData();

    while (*ptrA++ != '(');
    while (*ptrB++ != '(');

    return strcmp(ptrA, ptrB) == 0;
}



// QMetaType::UserType=1024，此处应远高于该值
int                     ObjectType::s_currentTypeId = ObjectType::ObjectTypeIdBase; 
QHash<QString, int>     ObjectType::s_nameIdMap;
QVector<ObjectFactory*> ObjectType::s_factories;

int ObjectType::registerFactory(const QString& typeName, ObjectFactory* factory)
{
    QHash<QString, int>::const_iterator i = s_nameIdMap.constFind(typeName);
    if (s_nameIdMap.constEnd() != i)
    {
        qDebug() << "JSON warning: " << typeName << "multiple registration.";
    }

    int typeId = s_currentTypeId++;

    Q_ASSERT(s_factories.size() == typeId - ObjectTypeIdBase);
    s_factories.push_back(factory);
    s_nameIdMap.insert(typeName, typeId);

    return typeId;
}

ObjectFactory* ObjectType::factory( int objectType )
{
    if (objectType >= ObjectTypeIdBase)
    {
        objectType -= ObjectTypeIdBase;
        Q_ASSERT(objectType < s_factories.size());

        return s_factories[objectType];
    }

    return NULL;
}

void* ObjectType::create(int objectType)
{
    ObjectFactory* factory = ObjectType::factory(objectType);
    if (factory)
        return factory->create();

    return QMetaType::create(objectType);
}

void* ObjectType::create(int objectType, void* copy)
{
    return QMetaType::create(objectType, copy);
}

QObject* ObjectType::clone(const QObject* object)
{
    if (!object)
        return NULL;

    const QMetaObject* metaObject = object->metaObject();
    const char* metaType = metaObject->className();
    int metaTypeId = QMetaType::type(metaType);
    if (metaTypeId == QMetaType::UnknownType)
    {
        qCritical() << "Failed to clone QObject of " << metaType << " type, you may need to register this class to the Meta-System.";
        return NULL;
    }

    return reinterpret_cast<QObject*>(QMetaType::create(metaTypeId, object));
}

void ObjectType::destroy(int objectType, void* ptr)
{
    ObjectFactory* factory = ObjectType::factory(objectType);
    if (factory)
        return factory->destroy(ptr);

    return QMetaType::destroy(objectType, ptr);
}

int ObjectType::type(const QString& typeName)
{
    QHash<QString, int>::const_iterator iter = s_nameIdMap.constFind(typeName);
    if (iter != s_nameIdMap.constEnd())
        return (int)iter.value();

    return QMetaType::type(typeName.toLatin1().constData());
}

QString ObjectType::typeName(int objectType)
{
    ObjectFactory* factory = ObjectType::factory(objectType);
    if (factory)
        return factory->typeName();

    return QLatin1String(QMetaType::typeName(objectType));
}

const QMetaObject* ObjectType::metaObjectForType( int objectType )
{
    ObjectFactory* factory = ObjectType::factory(objectType);
    if (factory)
        return factory->metaObject();

    return QMetaType::metaObjectForType(objectType);
}

/*********************************************************************************************************
** End of file
*********************************************************************************************************/