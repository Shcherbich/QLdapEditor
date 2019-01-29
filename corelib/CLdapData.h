#ifndef CLdapData_H
#define CLdapData_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include "CLdapObject.h"
#include "CLdapEntry.h"
#include "datatypes.h"
#include "CLdapSchema.h"
#include <memory>

class LDAPConnection;

namespace ldapcore
{

class CLdapData :  public QObject
{
	Q_OBJECT

public:
	explicit CLdapData(QObject* parent = nullptr);
	~CLdapData();

public:
	void connect(const tConnectionOptions& connectOptions);
	void resetConnection();
    void rebuild();

	QVector<CLdapEntry*> topList();
	QStringList search(const tSearchOptions& searchOptions);
	CLdapSchema& schema();
	QString host();
	int port();
	QString baseDN();

signals:
	void OnConnectionCompleted(CLdapData* pThis, bool isSucceed, QString errorDescription);

private:
	void build();

private:
	QVector<CLdapEntry*> m_Entries;
	std::unique_ptr<LDAPConnection> m_Connection;
	CLdapSchema m_Schema;
	std::string m_baseDN;
};

}

#endif // CLdapData_H
