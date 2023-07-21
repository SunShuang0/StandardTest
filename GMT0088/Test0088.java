package gmt0088;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.math.BigInteger;
import java.net.HttpURLConnection;
import java.net.URL;

import cn.org.bjca.mssp.msspjce.jcajce.provider.asymmetric.util.SM2Util;
import cn.org.bjca.mssp.util.EncodeUtil;

public class Test0088 {

	
	
	public static void test_AuthPK() throws Exception{
		
		URL u =  new URL(ConstBJCA.CHSM_URL+"/authpk");
		HttpURLConnection uc = (HttpURLConnection)u.openConnection();
 
		uc.setDoOutput(true);
		uc.setDoOutput(true);
		uc.setRequestMethod("POST");
		
		uc.setRequestProperty("Content-Type", "application/json;charset=gbk");
		
		String json = "{ \"requestId\":\"1889900\" ,\"algorithm\":\"sm2\",\"pks\":[\"%publickey%\"] }";
		json = json.replaceAll("%publickey%",EncodeUtil.base64Encode( ConstBJCA.pubKey.getEncoded()  ) );
		
		System.out.println(json);
		//if(true)return;
		uc.connect();
		
		OutputStream out = uc.getOutputStream();
        out.write(json.getBytes());
        out.flush();
        out.close();
        
		int code = uc.getResponseCode();
		System.out.println( "HTTP Respose Code=" + code );
		
		if(code==200) {
			BufferedReader in = new BufferedReader( new InputStreamReader( uc.getInputStream(),"utf-8" ) );
			String result = "";
			String line;
			while( (line=in.readLine())!=null ) {
				result += line;
			}			
			in.close();
			System.out.println(result);
		}
		uc.disconnect();
		
	}
	
	
	public static void test_GetCHSHStatus() throws Exception{
		
		URL u =  new URL(ConstBJCA.CHSM_URL+"/status" + "?" + "9172c481-1f7b-4ec1-9c45-0d76a00e7200");
		HttpURLConnection uc = (HttpURLConnection)u.openConnection();
 
		uc.setDoOutput(false);
		uc.setDoOutput(true);
		uc.setRequestMethod("GET");
		uc.connect();
		
		int code = uc.getResponseCode();
		System.out.println( "HTTP Respose Code=" + code );
		
		if(code==200) {
			BufferedReader in = new BufferedReader( new InputStreamReader( uc.getInputStream() ) );
			String result = "";
			String line;
			while( (line=in.readLine())!=null ) {
				result += line;
			}			
			in.close();
			System.out.println(result);
		}
		uc.disconnect();
	
	};
	
	
	
	public static void test_GetCHSMInfo() throws Exception{
		
		String json = "{ \"requestId\":\"1889900\" ,\"oprType\":\"getinfo\" }";
		URL u =  new URL(ConstBJCA.CHSM_URL);
		HttpURLConnection uc = (HttpURLConnection)u.openConnection();
 
		uc.setDoOutput(true);
		uc.setDoOutput(true);
		uc.setRequestMethod("POST");
		
		uc.setRequestProperty("Content-Type", "application/json;charset=gbk");
		
		
		uc.setRequestProperty("CHSM-AuthPK", EncodeUtil.base64Encode( ConstBJCA.pubKey.getEncoded()  ));
		uc.setRequestProperty("CHSM-SignatureAlg", "SM2WithSM3");
		
		
		byte[] hash = SM2Util.SM3ForSignature(json.getBytes(), ConstBJCA.pubKey);
		
		byte[] sig = SM2Util.sign( SM2Util.pack2PrivateKey( ConstBJCA.prikey ), hash   );
		
		uc.setRequestProperty("CHSM-Signature", EncodeUtil.base64Encode(     sig      ));
		
		
		System.out.println(json);
		//if(true)return;
		uc.connect();
		
		OutputStream out = uc.getOutputStream();
        out.write(json.getBytes());
        out.flush();
        out.close();
        
		int code = uc.getResponseCode();
		System.out.println( "HTTP Respose Code=" + code );
		
		if(code==200) {
			BufferedReader in = new BufferedReader( new InputStreamReader( uc.getInputStream(),"utf-8" ) );
			String result = "";
			String line;
			while( (line=in.readLine())!=null ) {
				result += line;
			}			
			in.close();
			System.out.println(result);
		}
		uc.disconnect();	
		
	}
	
	
	public static void main(String[] args) throws Exception {

		
		//test_GetCHSHStatus();
		//test_AuthPK();
		test_GetCHSMInfo();

	}

}
