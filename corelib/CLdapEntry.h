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
	explicit CLdapEntry(CLdapEntry* parentLdapEntry, LDAPEntry* le, QObject* parent = nullptr);
	~CLdapEntry();

	CLdapEntry* parent();
	QVector<CLdapEntry*> children();
	QVector<CLdapAttribute>* attributes();
    QVector<CLdapAttribute> availableAttributesMay();
    QVector<CLdapAttribute> availableAttributesMust();
    bool                    isMust(std::string attributeName);
    std::shared_ptr<CLdapAttribute> createEmptyAttribute(std::string attributeName);

    void                    validateAttribute(CLdapAttribute& attr);
    void                    loadAttributes(QVector<CLdapAttribute>& );

    void                    addAttribute(CLdapAttribute& newOb) throw(CLdapServerException);
    void                    deleteAttribute(CLdapAttribute& newOb) throw(CLdapServerException);
    void                    updateAttribute(CLdapAttribute& newOb) throw(CLdapServerException);
    void                    flushAttributeCache();

public:
	QString dn();
	QString rDn();

signals:

public slots:

private:
	void construct(CLdapData* data, LDAPConnection* conn, QString baseDn);
	void prepareAttributes();
    void availableAttributesMayImpl();
    void availableAttributesMustImpl();

private:
	CLdapEntry*           m_pParent{nullptr};
	LDAPEntry*            m_pEntry{nullptr};
	LDAPConnection*       m_Conn{nullptr};
	QVector<CLdapEntry*>  m_pEntries;
	QString               m_baseDn;
	QString               m_rDn;
	CLdapData*            m_pData;

	QVector<CLdapAttribute> m_attributes;
    QVector<CLdapAttribute> m_Must;
    QVector<CLdapAttribute> m_May;

	friend class CLdapData;
    friend class CLdapAttributesModel;
};

}

#endif // CLDAPENTRY_H
