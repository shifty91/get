#ifndef _SFTP_H_
#define _SFTP_H_

#include <string>
#include <vector>
#include <utility>

#include "method.h"
#include "tcp_connection.h"
#include "ssh/ssh_wrapper.h"

using KeyPairVector = std::vector<std::pair<std::string, std::string> >;

class SFTPMethod : Method
{
private:
    KeyPairVector find_user_keys() const;
    void print_fingerprint(const std::string& fingerprint) const;
    void publickey_auth(SSHSession& session, const std::string& user) const;

public:
    SFTPMethod(const std::string& host, const std::string& object) :
        Method(host, object)
    {}

    virtual void get(const std::string& fileToSave, const std::string& user = "",
                     const std::string& pw = "") const override;
};

#endif /* _SFTP_H_ */