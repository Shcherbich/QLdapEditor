// $OpenLDAP$
/*
 * Copyright 2000-2018 The OpenLDAP Foundation, All Rights Reserved.
 * COPYING RESTRICTIONS APPLY, see COPYRIGHT file
 */

#include <assert.h>
#include "config.h"
#include "debug.h"
#include "LDAPAsynConnection.h"
#include "ac/time.h"

#include "LDAPAddRequest.h"
#include "LDAPBindRequest.h"
#include "LDAPCompareRequest.h"
#include "LDAPDeleteRequest.h"
#include "LDAPExtRequest.h"
#include "LDAPEntry.h"
#include "LDAPModDNRequest.h"
#include "LDAPModifyRequest.h"
#include "LDAPRequest.h"
#include "LDAPRebind.h"
#include "LDAPRebindAuth.h"
#include "LDAPSearchRequest.h"
#include <lber.h>
#include <sstream>
#include <openssl/ssl.h>


#if !defined LDAP_OPT_X_TLS_PROTOCOL_TLS1_3

#define LDAP_OPT_X_TLS_PROTOCOL_TLS1_3 ((3 << 8) + 4)

#endif

using namespace std;

LDAPAsynConnection::LDAPAsynConnection(const string& url, int port,
                                       LDAPConstraints* cons)
{
	DEBUG(LDAP_DEBUG_CONSTRUCT, "LDAPAsynConnection::LDAPAsynConnection()"
	      << endl);
	DEBUG(LDAP_DEBUG_CONSTRUCT | LDAP_DEBUG_PARAMETER,
	      "   URL:" << url << endl << "   port:" << port << endl);
	cur_session = 0;
	m_constr = 0;
	// Is this an LDAP URI?
	if (url.find("://") == std::string::npos)
	{
		this->init(url, port);
	}
	else
	{
		this->initialize(url);
	}
	this->setConstraints(cons);


}

LDAPAsynConnection::~LDAPAsynConnection()
{
	unbind();
	delete m_constr;
}

void LDAPAsynConnection::init(const string& hostname, int port)
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::init" << endl);
	DEBUG(LDAP_DEBUG_TRACE | LDAP_DEBUG_PARAMETER,
	      "   hostname:" << hostname << endl
	      << "   port:" << port << endl);

	unbind();

	m_uri.setScheme("ldap");
	m_uri.setHost(hostname);
	m_uri.setPort(port);

	const char* ldapuri = m_uri.getURLString().c_str();
	int ret = ldap_initialize(&cur_session, ldapuri);
	if (ret != LDAP_SUCCESS)
	{
		throw LDAPException(ret);
	}
	int opt = 3;
	ldap_set_option(cur_session, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);
	ldap_set_option(cur_session, LDAP_OPT_PROTOCOL_VERSION, &opt);

	configureTimeouts();
}

void LDAPAsynConnection::initialize(const std::string& uri)
{
	unbind();

	m_uri.setURLString(uri);
	int ret = ldap_initialize(&cur_session, m_uri.getURLString().c_str());
	if (ret != LDAP_SUCCESS)
	{
		throw LDAPException(ret);
	}
	int opt = 3;
	ldap_set_option(cur_session, LDAP_OPT_REFERRALS, LDAP_OPT_OFF);
	ldap_set_option(cur_session, LDAP_OPT_PROTOCOL_VERSION, &opt);

	configureTimeouts();
}

std::function< bool(std::string)> mainFuncToConfirmWarnings {nullptr};

static int verify_callback(int ok, X509_STORE_CTX* ctx)
{
	//return 0 - stop verification
	//       1 - OK. continue

    //X509* cert = X509_STORE_CTX_get_current_cert(ctx);
	if (ok)
	{
		return ok;
	}

    //long serialNum = ASN1_INTEGER_get(X509_get_serialNumber(cert));
    //char* subjectid = X509_NAME_oneline(X509_get_subject_name(cert), NULL, 0);
	int sslRet = X509_STORE_CTX_get_error(ctx);
	const char* err = NULL;
	switch (sslRet)
	{
	case X509_V_ERR_UNABLE_TO_GET_CRL:
	case X509_V_ERR_CRL_HAS_EXPIRED:
	case X509_V_ERR_CRL_NOT_YET_VALID:
		return 1;
	default:
		err = X509_verify_cert_error_string(sslRet);
		if (err)
		{
			if (mainFuncToConfirmWarnings != nullptr)
			{
				bool canContinue = mainFuncToConfirmWarnings(err);
				return canContinue ? 1 : 0;

			}
		}
		return 0;
	}
	return 0;
}

static void ldap_tls_cb(LDAP*, SSL* ssl, SSL_CTX* ctx, void*)
{
    const int ssl_min_proto_version = 123;
    const int ssl_max_proto_version = 124;
    auto bMin = SSL_CTX_ctrl(ctx, ssl_min_proto_version, LDAP_OPT_X_TLS_PROTOCOL_TLS1_3, NULL);
    auto bMax = SSL_CTX_ctrl(ctx, ssl_max_proto_version, LDAP_OPT_X_TLS_PROTOCOL_TLS1_3, NULL);
    assert(bMin == 0);
    assert(bMax == 0);

    //auto sslVersion = SSL_version(ssl);
    //bool b1 = sslVersion == TLS1_2_VERSION;

	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);
	SSL_set_verify(ssl, SSL_VERIFY_PEER, verify_callback);
}


void LDAPAsynConnection::start_tls(std::function< bool(std::string)> funcToConfirmWarnings)
{
    // as least TLS 1.3 will be used
    int value = LDAP_OPT_X_TLS_PROTOCOL_TLS1_3;
    int ret = ldap_set_option(cur_session, LDAP_OPT_X_TLS_PROTOCOL_MIN, &value);
    assert(ret == LDAP_SUCCESS);

	mainFuncToConfirmWarnings = funcToConfirmWarnings;

	void* invalue;
	invalue = (void*)ldap_tls_cb;
	int result = ldap_set_option(cur_session, LDAP_OPT_X_TLS_CONNECT_CB, invalue);
	result = ldap_start_tls_s(cur_session, NULL, NULL);
	if (result != LDAP_SUCCESS)
	{
		throw LDAPException(this);
	}
}

void LDAPAsynConnection::configureTimeouts()
{
	int value = 120;
	int ret = ldap_set_option(cur_session, LDAP_OPT_X_KEEPALIVE_IDLE, &value);
	assert(ret == LDAP_SUCCESS);

	value = 4000;
	ret = ldap_set_option(cur_session, LDAP_OPT_X_KEEPALIVE_PROBES, &value);
	assert(ret == LDAP_SUCCESS);

	value = 120;
	ret = ldap_set_option(cur_session, LDAP_OPT_X_KEEPALIVE_INTERVAL, &value);
	assert(ret == LDAP_SUCCESS);

	value = 120;
	ret = ldap_set_option(cur_session, LDAP_OPT_RESTART, &value);
	assert(ret == LDAP_SUCCESS);

    struct timeval tcp = {0, 0};
	tcp.tv_sec = 10;
	ret = ldap_set_option(cur_session, LDAP_OPT_NETWORK_TIMEOUT, &tcp);
	assert(ret == LDAP_SUCCESS);
}

LDAPMessageQueue* LDAPAsynConnection::bind(const string& dn,
                                           const string& passwd, const LDAPConstraints* cons)
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::bind()" <<  endl);
	DEBUG(LDAP_DEBUG_TRACE | LDAP_DEBUG_PARAMETER, "   dn:" << dn << endl
	      << "   passwd:" << passwd << endl);
	LDAPBindRequest* req = new LDAPBindRequest(dn, passwd, this, cons);
	try
	{
		LDAPMessageQueue* ret = req->sendRequest();
		return ret;
	}
	catch (LDAPException e)
	{
		delete req;
		throw;
	}
}

LDAPMessageQueue* LDAPAsynConnection::saslBind(const std::string& mech,
                                               const std::string& cred,
                                               const LDAPConstraints* cons)
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::saslBind()" <<  endl);
	LDAPSaslBindRequest* req = new LDAPSaslBindRequest(mech, cred, this, cons);
	try
	{
		LDAPMessageQueue* ret = req->sendRequest();
		return ret;
	}
	catch (LDAPException e)
	{
		delete req;
		throw;
	}

}

LDAPMessageQueue* LDAPAsynConnection::saslInteractiveBind(
    const std::string& mech,
    int flags,
    SaslInteractionHandler* sih,
    const LDAPConstraints* cons)
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::saslInteractiveBind"
	      << std::endl);
	LDAPSaslInteractiveBind* req =
	    new LDAPSaslInteractiveBind(mech, flags, sih, this, cons);
	try
	{
		LDAPMessageQueue* ret = req->sendRequest();
		return ret;
	}
	catch (LDAPException e)
	{
		delete req;
		throw;
	}
}

LDAPMessageQueue* LDAPAsynConnection::search(const string& base, int scope,
                                             const string& filter,
                                             const StringList& attrs,
                                             bool attrsOnly,
                                             const LDAPConstraints* cons)
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::search()" <<  endl);
	DEBUG(LDAP_DEBUG_TRACE | LDAP_DEBUG_PARAMETER, "   base:" << base << endl
	      << "   scope:" << scope << endl
	      << "   filter:" << filter << endl);
	LDAPSearchRequest* req = new LDAPSearchRequest(base, scope, filter, attrs,
	                                               attrsOnly, this, cons);
	try
	{
		LDAPMessageQueue* ret = req->sendRequest();
		return ret;
	}
	catch (LDAPException e)
	{
		delete req;
		throw;
	}
}

LDAPMessageQueue* LDAPAsynConnection::del(const string& dn,
                                          const LDAPConstraints* cons)
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::del()" << endl);
	DEBUG(LDAP_DEBUG_TRACE | LDAP_DEBUG_PARAMETER, "   dn:" << dn << endl);
	LDAPDeleteRequest* req = new LDAPDeleteRequest(dn, this, cons);
	try
	{
		LDAPMessageQueue* ret = req->sendRequest();
		return ret;
	}
	catch (LDAPException e)
	{
		delete req;
		throw;
	}
}

LDAPMessageQueue* LDAPAsynConnection::compare(const string& dn,
                                              const LDAPAttribute& attr, const LDAPConstraints* cons)
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::compare()" << endl);
	DEBUG(LDAP_DEBUG_TRACE | LDAP_DEBUG_PARAMETER, "   dn:" << dn << endl
	      << "   attr:" << attr << endl);
	LDAPCompareRequest* req = new LDAPCompareRequest(dn, attr, this, cons);
	try
	{
		LDAPMessageQueue* ret = req->sendRequest();
		return ret;
	}
	catch (LDAPException e)
	{
		delete req;
		throw;
	}
}

LDAPMessageQueue* LDAPAsynConnection::add(const LDAPEntry* le,
                                          const LDAPConstraints* cons)
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::add()" << endl);
	DEBUG(LDAP_DEBUG_TRACE | LDAP_DEBUG_PARAMETER, "   entry:" << *le << endl);
	LDAPAddRequest* req = new LDAPAddRequest(le, this, cons);
	try
	{
		LDAPMessageQueue* ret = req->sendRequest();
		return ret;
	}
	catch (LDAPException e)
	{
		delete req;
		throw;
	}
}

LDAPMessageQueue* LDAPAsynConnection::modify(const string& dn,
                                             const LDAPModList* mod, const LDAPConstraints* cons)
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::modify()" << endl);
	DEBUG(LDAP_DEBUG_TRACE | LDAP_DEBUG_PARAMETER, "   dn:" << dn << endl);
	LDAPModifyRequest* req = new LDAPModifyRequest(dn, mod, this, cons);
	try
	{
		LDAPMessageQueue* ret = req->sendRequest();
		return ret;
	}
	catch (LDAPException e)
	{
		delete req;
		throw;
	}
}

LDAPMessageQueue* LDAPAsynConnection::rename(const string& dn,
                                             const string& newRDN, bool delOldRDN, const string& newParentDN,
                                             const LDAPConstraints* cons)
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::rename()" << endl);
	DEBUG(LDAP_DEBUG_TRACE | LDAP_DEBUG_PARAMETER, "   dn:" << dn << endl
	      << "   newRDN:" << newRDN << endl
	      << "   newParentDN:" << newParentDN << endl
	      << "   delOldRDN:" << delOldRDN << endl);
	LDAPModDNRequest* req = new  LDAPModDNRequest(dn, newRDN, delOldRDN,
	                                              newParentDN, this, cons);
	try
	{
		LDAPMessageQueue* ret = req->sendRequest();
		return ret;
	}
	catch (LDAPException e)
	{
		delete req;
		throw;
	}
}


LDAPMessageQueue* LDAPAsynConnection::extOperation(const string& oid,
                                                   const string& value, const LDAPConstraints* cons)
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::extOperation()" << endl);
	DEBUG(LDAP_DEBUG_TRACE | LDAP_DEBUG_PARAMETER, "   oid:" << oid << endl);
	LDAPExtRequest* req = new  LDAPExtRequest(oid, value, this, cons);
	try
	{
		LDAPMessageQueue* ret = req->sendRequest();
		return ret;
	}
	catch (LDAPException e)
	{
		delete req;
		throw;
	}
}


void LDAPAsynConnection::abandon(LDAPMessageQueue* q)
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::abandon()" << endl);
	LDAPRequestStack* reqStack = q->getRequestStack();
	LDAPRequest* req;
	while (! reqStack->empty())
	{
		req = reqStack->top();
		if (ldap_abandon_ext(cur_session, req->getMsgID(), 0, 0)
		    != LDAP_SUCCESS)
		{
			throw LDAPException(this);
		}
		delete req;
		reqStack->pop();
	}
}

void LDAPAsynConnection::unbind()
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::unbind()" << endl);
	if (cur_session)
	{
		LDAPControl** tmpSrvCtrls = m_constr->getSrvCtrlsArray();
		LDAPControl** tmpClCtrls = m_constr->getClCtrlsArray();
		int err = ldap_unbind_ext(cur_session, tmpSrvCtrls, tmpClCtrls);
		cur_session = 0;
		LDAPControlSet::freeLDAPControlArray(tmpSrvCtrls);
		LDAPControlSet::freeLDAPControlArray(tmpClCtrls);
		if (err != LDAP_SUCCESS)
		{
			throw LDAPException(err);
		}
	}
}

void LDAPAsynConnection::setConstraints(LDAPConstraints* cons)
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::setConstraints()" << endl);
	delete m_constr;
	m_constr = cons;
}

const LDAPConstraints* LDAPAsynConnection::getConstraints() const
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::getConstraints()" << endl);
	return m_constr;
}

TlsOptions LDAPAsynConnection::getTlsOptions() const
{
	return TlsOptions(cur_session);
}

LDAP* LDAPAsynConnection::getSessionHandle() const
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::getSessionHandle()" << endl);
	return cur_session;
}

const string& LDAPAsynConnection::getHost() const
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::setHost()" << endl);
	return m_uri.getHost();
}

string LDAPAsynConnection::getServerMsg()
{
    const char* err_string = nullptr;
    int ret = ldap_get_option(cur_session, LDAP_OPT_ERROR_STRING, &err_string);
    if (ret == LDAP_SUCCESS && err_string)
    {
        return std::string(err_string);
    }
    return "";
}


int LDAPAsynConnection::getPort() const
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::getPort()" << endl);
	return m_uri.getPort();
}

LDAPAsynConnection* LDAPAsynConnection::referralConnect(
    const LDAPUrlList& urls, LDAPUrlList::const_iterator& usedUrl,
    const LDAPConstraints* cons) const
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::referralConnect()" << endl)
	LDAPUrlList::const_iterator conUrl;
	LDAPAsynConnection* tmpConn = 0;
	const LDAPRebind* rebind = cons->getReferralRebind();
	LDAPRebindAuth* auth = 0;

	for (conUrl = urls.begin(); conUrl != urls.end(); conUrl++)
	{
		string host = conUrl->getHost();
		int port = conUrl->getPort();
		DEBUG(LDAP_DEBUG_TRACE, "   connecting to: " << host << ":" <<
		      port << endl);
		//Set the new connection's constraints-object ?
		tmpConn = new LDAPAsynConnection(host.c_str(), port);
		int err = 0;

		if (rebind)
		{
			auth = rebind->getRebindAuth(host, port);
		}
		if (auth)
		{
			string dn = auth->getDN();
			string passwd = auth->getPassword();
			const char* c_dn = 0;
			struct berval c_passwd = { 0, 0 };
			if (dn != "")
			{
				c_dn = dn.c_str();
			}
			if (passwd != "")
			{
				c_passwd.bv_val = const_cast<char*>(passwd.c_str());
				c_passwd.bv_len = passwd.size();
			}
            err = ldap_sasl_bind_s(tmpConn->getSessionHandle(), c_dn, LDAP_SASL_SIMPLE, &c_passwd, NULL, NULL, NULL);
		}
		else
		{
			// Do anonymous bind
            err = ldap_sasl_bind_s(tmpConn->getSessionHandle(), NULL, LDAP_SASL_SIMPLE, NULL, NULL, NULL, NULL);
		}
		if (err == LDAP_SUCCESS)
		{
			usedUrl = conUrl;
			return tmpConn;
		}
		else
		{
			delete tmpConn;
			tmpConn = 0;
		}
		auth = 0;
	}
	return 0;
}



void  LDAPAsynConnection::modify_s(const string& dn, LDAPModList* mod)
{
	DEBUG(LDAP_DEBUG_TRACE, "LDAPAsynConnection::modify()" << endl);
	DEBUG(LDAP_DEBUG_TRACE | LDAP_DEBUG_PARAMETER, "   dn:" << dn << endl);

	LDAPMod** tmpMods = mod->toLDAPModArray();
    int err = ldap_modify_ext_s(getSessionHandle(), dn.c_str(), tmpMods, nullptr, nullptr);

    // LDAP_SERVER_DOWN
    // Try to hard reconnect to server
    if (err == LDAP_SERVER_DOWN)
    {
        if (hardReset())
        {
            err = ldap_modify_ext_s(getSessionHandle(), dn.c_str(), tmpMods, nullptr, nullptr);
        }
    }


	ldap_mods_free(tmpMods, 1);
	if (err != LDAP_SUCCESS)
	{
        throw LDAPException(err, getServerMsg());
	}
}

void LDAPAsynConnection::setHardResetFunc(std::function< bool() > f)
{
	hardResetFunc = std::move(f);
}

bool LDAPAsynConnection::hardReset()
{
	if (hardResetFunc == nullptr)
	{
		return false;
	}
	cur_session = nullptr;
	setConstraints(new LDAPConstraints());
	return hardResetFunc();
}
