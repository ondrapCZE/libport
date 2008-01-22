/**
 *
 * Copyright (C) Gostai S.A.S., 2006-2008.
 *
 * This software is provided "as is" without warranty of any kind,
 * either expressed or implied, including but not limited to the
 * implied warranties of fitness for a particular purpose.
 *
 * See the LICENSE file for more information.
 * For comments, bug reports and feedback: http://www.urbiforge.com
 */

#ifndef SIGNSHA1_HH_
# define SIGNSHA1_HH_

# include <cassert>
# include "urbi-crypto/openssl.hh"
# include "urbi-crypto/sign.hh"

namespace crypto
{
  class SignSHA1 : public Sign
  {
    typedef Sign super_type;

  public:
    SignSHA1();

    ~SignSHA1() throw();

    SignSHA1& operator= (const SignSHA1&);

    /// Sign the digest of the message.
    /// To use sign you have to set:
    ///	  - the private key with 'setPrivatekey ()'
    ///   - the message to sign with 'setInStream ()'
    /// After the call to the 'sign' function you will get
    /// the signature generated by calling 'getOutStream ()'
    virtual void sign();

    /// Check that a signature provided with a message is correct.
    /// To use check you have to set:
    ///   - the public key with 'setPublickey ()'
    ///   - the original message with 'setInStream ()'
    ///	  - the signature you want to check with 'setOutStream ()'
    /// This function will return true if the signature is correct.
    virtual bool check();

  };
}

#endif /* !SIGNSHA1_HH_ */
