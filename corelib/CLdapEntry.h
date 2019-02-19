#ifndef CLDAPENTRY_H
#define CLDAPENTRY_H

#include <QObject>
#include <QString>
#include <QVector>
#include <memory>
#include "CLdapAttribute.h"
#include "CLdapException.h"

class LDAPEntry;
class LDAPConnection;

namespace ldapcore
{

class CLdapData;

class CLdapEntry : public QObject
{
	Q_OBJECT
public:
	explicit CLdapEntry(CLdapEntry* parentLdapEntry, LDAPEntry* le, QObject* parentDn = nullptr);
	explicit CLdapEntry(CLdapEntry* parentLdapEntry, QString rdn, QString parent, QVector<QString>& classes, QObject* parentDn);
	~CLdapEntry();

	CLdapEntry*             parent();
	QVector<CLdapEntry*>    children();
	QVector<CLdapAttribute>* attributes();
	QVector<CLdapAttribute> availableAttributesMay();
	QVector<CLdapAttribute> availableAttributesMust();
	bool                    isMust(std::string attributeName);
    bool                    isNew();
    bool                    isEdit();
    void                    setEditable(bool isEdit);
	std::shared_ptr<CLdapAttribute> createEmptyAttribute(std::string attributeName);

    void                    addChild(CLdapEntry* child);
    void                    addAttributes(QVector<CLdapAttribute>&);
	void                    removeChild(CLdapEntry* child);
    void                    loadAttributes(QVector<CLdapAttribute>&, bool needToLoadSystemAttributes = true);
    void                    sortAttributes();
    void                    validateAttribute(CLdapAttribute& attr);
    void                    flushAttributeCache();

    void                    availableAttributesMayImpl();
    void                    availableAttributesMustImpl();

    QVector<QString>        classes();
    void                    setClasses(QVector<QString>&);
    QVector<QString>        availableClasses();
    QString                 structuralClass();
    QVector<QString>        auxiliaryClasses();

public:
	QString dn();
	QString rDn();

signals:

public slots:

private:
    void initialize(CLdapData* data, QString baseDn);
    void construct(CLdapData* data, QString baseDn);
	void prepareAttributes();
    LDAPConnection* connectionPtr() const;

private:
    CLdapData*            m_pData;
	CLdapEntry*           m_pParent{nullptr};
	LDAPEntry*            m_pEntry{nullptr};
    QVector<CLdapEntry*>  m_vChildren;
	QString               m_baseDn;
	QString               m_rDn;
	bool                  m_isNew{false};
    bool                  m_isEdit{false};
    bool                  m_isLoaded{false};

	QVector<CLdapAttribute> m_attributes;
	QVector<CLdapAttribute> m_Must;
	QVector<CLdapAttribute> m_May;
	QVector<QString>        m_classes;

	friend class CLdapData;
	friend class CLdapAttributesModel;
	friend class CLdapTreeView;
    friend class CLdapServer;
};

}

#endif // CLDAPENTRY_H
