#ifndef CLDAPSCHEMA_H
#define CLDAPSCHEMA_H

#include <memory>
#include <string>
#include <map>
#include <tuple>
#include "CLdapAttribute.h"
#include "CLdapException.h"
#include <QVector>
#include <QString>

class LDAPSchema;
class LDAPConnection;

namespace ldapcore
{

struct CLdapSchemaImpl;

class CLdapSchema
{
public:
	CLdapSchema();
	~CLdapSchema();

    std::tuple<AttrType, bool> attributeInfoByName(std::string attrName);
    bool isNameExist(std::string attributeName) noexcept(false);
    void validateAttributeByName(std::string attributeName, std::string value) noexcept(false);
    QVector<QString> classes();
    QVector<QString> structuralClasses();
    QVector<QString> auxiliaryClasses();
    QString supByClass(QString c);
    QString startRdn(QString c);
    QVector<QString> auxiliaryClassesBySup(QString sup);
    QVector<CLdapAttribute> attributeByClasses(QVector<QString>& classes, std::map<std::string, std::string>& attribute2value);
    QVector<QString> classesByAttributeName(std::string attrName, QVector<QString>& classesOfEntry);
    QString classDescription(const QString& cls);
    QVector<CLdapAttribute> mayAttributesByClass(QString cl);
    QVector<CLdapAttribute> mustAttributesByClass(QString cl);
    QVector<QString> consistentClassesByStructuralAndOther(QString structuralCl, QVector<QString> allClasses);
    QString deductStructuralClass(const QVector<QString>& classes);

private:
    void build(LDAPConnection* lc);
	LDAPSchema* classesSchema();
	LDAPSchema* attributesSchema();

private:
	std::unique_ptr<CLdapSchemaImpl> m_impl;


	friend class CLdapData;
	friend class CLdapEntry;
};

}
#endif // CLDAPSCHEMA_H
