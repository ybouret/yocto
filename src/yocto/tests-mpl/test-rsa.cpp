#include "yocto/mpl/rsa/keys.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/alea.hpp"
#include "yocto/sys/rt-clock.hpp"

using namespace yocto;
using namespace mpl;



YOCTO_UNIT_TEST_IMPL(rsa)
{
    mpn p = mpn::rand(6+alea.leq(25));
    mpn q = mpn::rand(6+alea.leq(25));
    p = mpn::__next_prime(p);
    q = mpn::__next_prime(q);
    mpn e = 3+alea.leq(100);
    RSA::PrivateKey prv = RSA::PrivateKey::GenerateFrom(p, q, e);
    RSA::PublicKey  pub(prv);

    std::cerr << std::hex;
    std::cerr << "prv=[" << prv.modulus << "," << prv.privateExponent << "]" << std::endl;
    std::cerr << "pub=[" << pub.modulus << "," << prv.publicExponent  << "]" << std::endl;

    std::cerr << "timing performances..." << std::endl;
    
    rt_clock chrono;
    chrono.calibrate();
    uint64_t _rsa = 0;
    uint64_t _crt = 0;
    uint64_t mark = 0;
    
    for(size_t i=1;i<=1000;++i)
    {
        const mpn M = mpn::rand( pub.ibits );
        const mpn C = pub.encode(M);
        
        mark = chrono.ticks();
        const mpn P = prv.decode_(C);
        _rsa += chrono.ticks()-mark;
        
        mark = chrono.ticks();
        const mpn Q = prv.decode(C);
        _crt += chrono.ticks()-mark;
        if(P!=M)  throw exception("RSA Failure");
        if(P!=Q)
        {
            //std::cerr << "CRT Failure" << std::endl;
            throw exception("CRT Failure");
        }
    }
    
    const double tmx_rsa = chrono(_rsa);
    const double tmx_crt = chrono(_crt);
    std::cerr << "rsa: " << tmx_rsa << " seconds" << std::endl;
    std::cerr << "crt: " << tmx_crt << " seconds" << std::endl;
}
YOCTO_UNIT_TEST_DONE()


#include "yocto/mpl/rsa/codec.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/string/base64.hpp"

static const uint8_t rsa_keys[] =
{
#include "./rsa-keys.inc"
};

static const size_t rsa_keys_length = sizeof(rsa_keys)/sizeof(rsa_keys[0]);

#include "yocto/ios/imstream.hpp"
#include "yocto/ptr/shared.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/string/conv.hpp"

YOCTO_UNIT_TEST_IMPL(rsaQ)
{
    typedef shared_ptr<RSA::Key>  key_ptr;

    vector<key_ptr> pub_vec;
    vector<key_ptr> prv_vec;
    {
        ios::imstream inp(rsa_keys,rsa_keys_length);
        char C;
        while( inp.query(C) )
        {
            inp.store(C);
            const RSA::PublicKey k = RSA::PublicKey::load_pub(inp);
            const key_ptr        p( k.clone() );
            pub_vec.push_back(p);
        }
        std::cerr << "Loaded " << pub_vec.size() << " public keys..." << std::endl;
    }

    {
        ios::imstream inp(rsa_keys,rsa_keys_length);
        char C;
        while( inp.query(C) )
        {
            inp.store(C);
            const RSA::PrivateKey k = RSA::PrivateKey::load_prv(inp);
            const key_ptr         p( k.clone() );
            prv_vec.push_back(p);
        }
        std::cerr << "Loaded " << prv_vec.size() << " private keys..." << std::endl;
    }

    if( prv_vec.size() != pub_vec.size() )
    {
        throw exception("key pairs mismatch!");
    }

    for(size_t i=1;i<=pub_vec.size();++i)
    {
        const RSA::Key &pub_k = *pub_vec[i];
        const RSA::Key &prv_k = *pub_vec[i];
        if(pub_k.modulus!=prv_k.modulus) throw exception("Modulus Mismatch!");
        std::cerr << "key#" << i << " bits=" << pub_k.modulus.bits() << std::endl;
    }


#if 0
    mpn p = mpn::rand(6+alea_leq(30));
    mpn q = mpn::rand(6+alea_leq(30));
    p = mpn::__next_prime(p);
    q = mpn::__next_prime(q);
    mpn e = 3+alea_leq(100);
    RSA::PrivateKey prv = RSA::PrivateKey::GenerateFrom(p, q, e);
    RSA::PublicKey  pub(prv);
    std::cerr << "pub=[" << pub.modulus << "," << pub.publicExponent << "]" << std::endl;
#endif

    size_t idx=1;

    if(argc>1)
    {
        idx = strconv::to<size_t>(argv[1],"idx");
        idx = clamp<size_t>(1,idx,prv_vec.size());
    }

    const RSA::Key &pub = *pub_vec[idx];
    const RSA::Key &prv = *prv_vec[idx];

    std::cerr << "Using pub =[" << pub.modulus << "]" << std::endl;
    std::cerr << "Using bits=[" << pub.modulus.bits() << "]" << std::endl;

    base64::encoder b64;
    
    string line;
    ios::icstream fp( ios::cstdin );
    RSA::encoder  EncPub(pub);
    RSA::encoder  EncPrv(prv);
    RSA::decoder  DecPub(pub);
    RSA::decoder  DecPrv(prv);

    std::cerr << std::hex;
    while( line.clear(), (std::cerr << "> ").flush(), fp.read_line(line) >= 0 )
    {
        const string input       = b64.to_string(line);
        std::cerr << "input  : " << input       << std::endl;
        const string pub_enc     = EncPub.to_string(line);
        const string pub_enc_out = b64.to_string(pub_enc);
        std::cerr << "pub_enc: " << pub_enc_out << std::endl;

        const string pub_dec     = DecPrv.to_string(pub_enc);
        const string pub_dec_out = b64.to_string(pub_dec);
        std::cerr << "pub_dec: " << pub_dec_out << std::endl;


        const string prv_enc     = EncPrv.to_string(line);
        const string prv_enc_out = b64.to_string(prv_enc);
        std::cerr << "prv_enc: " << prv_enc_out << std::endl;
        const string prv_dec     = DecPub.to_string(prv_enc);
        const string prv_dec_out = b64.to_string(prv_dec);
        std::cerr << "prv_dec: " << prv_dec_out << std::endl;
    }


}
YOCTO_UNIT_TEST_DONE()

