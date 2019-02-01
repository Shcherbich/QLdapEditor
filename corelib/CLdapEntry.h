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
    QVector<QString>        classes();
    void                    setClasses(QVector<QString>&);
	QVector<QString>        availableClasses();

	void                    validateAttribute(CLdapAttribute& attr);
    void                    loadAttributes(QVector<CLdapAttribute>&, bool needToLoadSystemAttributes = true);
    void                    sortAttributes();

    void                    addAttribute(CLdapAttribute& newOb) noexcept(false);
    void                    deleteAttribute(CLdapAttribute& newOb) noexcept(false);
    void                    updateAttribute(CLdapAttribute& newOb) noexcept(false);
	void                    flushAttributeCache();
	void                    addNewChild(CLdapEntry* child);
	void                    removeChild(CLdapEntry* child);
	void                    addAttributes(QVector<CLdapAttribute>&);
    void                    saveNewChild() noexcept(false);
    void                    update() noexcept(false);
    void                    availableAttributesMayImpl();
    void                    availableAttributesMustImpl();


public:
	QString dn();
	QString rDn();

signals:

public slots:

private:
	void construct(CLdapData* data, LDAPConnection* conn, QString baseDn);
	void prepareAttributes();

private:
	CLdapEntry*           m_pParent{nullptr};
	LDAPEntry*            m_pEntry{nullptr};
	LDAPConnection*       m_Conn{nullptr};
	QVector<CLdapEntry*>  m_pChildren;
	QString               m_baseDn;
	QString               m_rDn;
	CLdapData*            m_pData;
	bool                  m_isNew{false};
    bool                  m_isEdit{false};

	QVector<CLdapAttribute> m_attributes;
	QVector<CLdapAttribute> m_Must;
	QVector<CLdapAttribute> m_May;
	QVector<QString>        m_classes;

	friend class CLdapData;
	friend class CLdapAttributesModel;
	friend class CLdapTreeView;
};

}

#endif // CLDAPENTRY_H
