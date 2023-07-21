/*
 * FileName: sgd_def.h
 * CopyRight: Beijing Certificate Authority (C) 2020
 * Author: chenshu
 * Date: 2020-08-20
 * Description: algorithm related constant definitions
 */

#ifndef HEADER_SGD_H
#define HEADER_SGD_H

#define SGD_SM4_GCM    0x02000400
#define SGD_SM4_CCM    0x04000400

/* block cipher modes */
#define SGD_ECB			0x01
#define SGD_CBC			0x02
#define SGD_CFB			0x04
#define SGD_OFB			0x08
#define SGD_MAC			0x10
#define SGD_CTR         0x20

/* stream cipher modes */
#define SGD_EEA3		0x01
#define SGD_EIA3		0x02

/* ciphers */
#define SGD_SM1			0x00000100
#define SGD_SSF33		0x00000200
#define SGD_SM4			0x00000400
#define SGD_ZUC			0x00000800
#define SGD_AES         0x10000400
#define SGD_3DES        0x10000800
/* ciphers with modes */
#define SGD_SM1_ECB		    (SGD_SM1|SGD_ECB)
#define SGD_SM1_CBC		    (SGD_SM1|SGD_CBC)
#define SGD_SM1_CFB		    (SGD_SM1|SGD_CFB)
#define SGD_SM1_OFB		    (SGD_SM1|SGD_OFB)
#define SGD_SM1_CTR         (SGD_SM1|SGD_CTR)
#define SGD_SM1_MAC		    (SGD_SM1|SGD_MAC)
#define SGD_SSF33_ECB		(SGD_SSF33|SGD_ECB)
#define SGD_SSF33_CBC		(SGD_SSF33|SGD_CBC)
#define SGD_SSF33_CFB		(SGD_SSF33|SGD_CFB)
#define SGD_SSF33_OFB		(SGD_SSF33|SGD_OFB)
#define SGD_SSF33_MAC		(SGD_SSF33|SGD_MAC)
#define SGD_SM4_ECB		    (SGD_SM4|SGD_ECB)
#define SGD_SM4_CBC		    (SGD_SM4|SGD_CBC)
#define SGD_SM4_CFB		    (SGD_SM4|SGD_CFB)
#define SGD_SM4_OFB		    (SGD_SM4|SGD_OFB)
#define SGD_SM4_MAC		    (SGD_SM4|SGD_MAC)
#define SGD_SM4_CTR         (SGD_SM4|SGD_CTR)
#define SGD_ZUC_EEA3		(SGD_ZUC|SGD_EEA3)
#define SGD_ZUC_EIA3		(SGD_ZUC|SGD_EIA3)

#define SGD_AES_ECB         (SGD_AES|SGD_ECB)
#define SGD_AES_CBC         (SGD_AES|SGD_CBC)
#define SGD_AES_CFB         (SGD_AES|SGD_CFB)
#define SGD_AES_OFB         (SGD_AES|SGD_OFB)

#define SGD_3DES_ECB        (SGD_3DES|SGD_ECB)
#define SGD_3DES_CBC        (SGD_3DES|SGD_CBC)
#define SGD_3DES_CFB        (SGD_3DES|SGD_CFB)
#define SGD_3DES_OFB        (SGD_3DES|SGD_OFB)


/* public key usage */
#define SGD_PK_SIGN		    0x0100
#define SGD_PK_DH		    0x0200
#define SGD_PK_ENC		    0x0400

/* public key types */
#define SGD_RSA			    0x00010000
#define SGD_RSA_SIGN		(SGD_RSA|SGD_PK_SIGN)
#define SGD_RSA_ENC		    (SGD_RSA|SGD_PK_ENC)
#define SGD_SM2			    0x00020100 //SM2Õ÷‘≤«˙œﬂ√‹¬ÎÀ„∑®
#define SGD_SM2_1		    0x00020200 //SM2Õ÷‘≤«˙œﬂ«©√˚À„∑®
#define SGD_SM2_2		    0x00020400 //SM2Õ÷‘≤«˙œﬂ√‹‘øΩªªª–≠“È
#define SGD_SM2_3		    0x00020800 //SM2Õ÷‘≤«˙œﬂº”√‹À„∑®
#define SGD_SHA224          0x00000008
#define SGD_SHA384          0x00000010
#define SGD_SHA512          0x00000020
/* hash */
#define SGD_SM3			    0x00000001
#define SGD_SHA1		    0x00000002
#define SGD_SHA256		    0x00000004

/* hmac */
#define SGD_SHA1_HMAC        0x00010012
#define SGD_SHA256_HMAC      0x00010014
#define SGD_SM3_HMAC         0x00020211

#endif // HEADER_SGD_H