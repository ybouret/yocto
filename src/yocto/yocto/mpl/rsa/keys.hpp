#ifndef YOCTO_MPL_RSA_KEY_INCLUDED
#define YOCTO_MPL_RSA_KEY_INCLUDED 1

#include "yocto/mpl/natural.hpp"
#include "yocto/code/fourcc.hpp"

namespace yocto
{
    namespace mpl
    {

        namespace RSA
        {

            class Key : public object
            {
            public:
                static const uint64_t MINIMUM_PRIME   = 3;
                static const uint64_t MINIMUM_MODULUS = MINIMUM_PRIME * MINIMUM_PRIME;
                static const uint32_t PUB32 = YOCTO_FOURCC('@','P','U','B');
                static const uint32_t PRV32 = YOCTO_FOURCC('@','P','R','V');

                const natural modulus;
                const size_t  obits; //!< (modulus-1).bits() :
                const size_t  ibits; //!< obits-1
                virtual ~Key() throw();


                //! M.bits() <= ibits, output C<M, C.bits <= obits
                virtual natural encode( const natural &M ) const = 0;

                //! C.bits() <= obits, C < modulus, output ibits, checked
                virtual natural decode( const natural &C ) const = 0;

                virtual Key *clone() const = 0;

            protected:
                explicit Key( const natural &Modulus);
                Key(const Key &);

            private:
                YOCTO_DISABLE_ASSIGN(Key);
            };

            class PrivateKey : public Key
            {
            public:

                explicit PrivateKey(const natural &Modulus,
                                    const natural &PublicExponent,
                                    const natural &PrivateExponent,
                                    const natural &Prime1,
                                    const natural &Prime2,
                                    const natural &Exponent1,
                                    const natural &Exponent2,
                                    const natural &Coefficient
                                    );
                PrivateKey(const PrivateKey &other );

                virtual ~PrivateKey() throw();

                //! (C^privateExponent) % modulus: C < modulus
                natural decode_( const natural &C ) const;

                //! (C^privateExponent) % modulus: C < modulus, using CRT
                virtual natural decode( const natural &C ) const;

                //! M.bits() <= ibits : (M^privateExponent) % modulus
                natural encode_( const natural &M ) const;

                //! M.bits() <= ibits : (M^privateExponent) % modulus, using CRT
                virtual natural encode( const natural &M ) const;


                //! Generate a private key
                static PrivateKey GenerateFrom(const natural &prime1, const natural &prime2, const natural &exponent );

                const natural publicExponent;  //!< the public exponent
                const natural privateExponent; //!< (1/publicExponent) %( (prime1-1) * (prime2-1) )
                const natural prime1;          //!< bigger  prime
                const natural prime2;          //!< smaller prime
                const natural exponent1;       //!< Chinese Remainder Theoreme exponent1   = d % (prime1-1)
                const natural exponent2;       //!< Chinese Remainder Theoreme exponent2   = d % (prime2-1)
                const natural coefficient;     //!< Chinese Remainder Theoreme coefficient = (1/prime2)  % prime1

                void save_prv( ios::ostream &fp ) const;
                static PrivateKey load_prv( ios::istream &fp );

                virtual Key *clone() const;

            private:
                YOCTO_DISABLE_ASSIGN(PrivateKey);
                natural CRT( const natural &C ) const;
            };


            class PublicKey : public Key
            {
            public:

                const natural publicExponent;


                virtual ~PublicKey() throw();
                explicit PublicKey(const natural &Modulus,
                                   const natural &PublicExponent);
                PublicKey( const PublicKey  &);
                PublicKey( const PrivateKey &);

                //! generate a public key
                static PublicKey GenerateFrom( const natural &prime1, const natural &prime2, const natural &exponent );

                //! M.bits() <= ibits : (M^publicExponent) % modulus
                natural encode( const natural &M ) const;
                
                //! C < modulus: (C^publicExponent) % modulus
                natural decode( const natural &C ) const;
                
                
                void save_pub( ios::ostream &fp ) const;
                static PublicKey load_pub( ios::istream &fp );
                
                virtual Key *clone() const;
                
            private:
                YOCTO_DISABLE_ASSIGN(PublicKey);
            };
            
            
        }
        
    }
    
}


#endif
