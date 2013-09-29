package com.alimama.display.algo.luna.ad;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.BytesWritable;
import org.apache.hadoop.io.SequenceFile;

import com.alimama.display.algo.luna.message.Luna.Ad;
import com.alimama.display.algo.luna.message.Luna.Label;
import com.alimama.display.algo.luna.message.Luna.Tag;
import com.taobao.algo.ecpm.admidtier.common.AdMidTierMessage.BoardInfo;
import com.taobao.algo.ecpm.admidtier.common.AdMidTierMessage.DestInfo;
import com.taobao.algo.ecpm.admidtier.common.AdMidTierMessage.TransInfo;
import com.taobao.algo.ecpm.admidtier.common.AdMidTierMessage.TransInfoBase;

import display.algo.common.Constants;

public class AdExtractor {
	private Map<Long, Long> AdvertiserId2MainCate = new  HashMap<Long, Long>();
	private static final String MID_AD_INPUT = "mid_ad_input";
	
	
	
	private long find_main_cate = 0;
	private long unfind_main_cate = 0;
	
	TransInfo.Builder transBuilder= TransInfo.newBuilder();
	
	private Map<String, Ad> transAdMap = null;
	
	Label.Builder tmpLabel = Label.newBuilder();
	Tag.Builder tmpTag= Tag.newBuilder();
	Ad.Builder ad = Ad.newBuilder();
	
	
	private AdExtractor() {
		transAdMap = new HashMap<String, Ad>();
	}
	
	public static AdExtractor newInstance(Configuration conf) throws IOException, URISyntaxException {		

		AdExtractor extractor = new AdExtractor();
		extractor.init(conf);		
		return extractor;
	}
	
	private void init(Configuration conf) throws IOException, URISyntaxException {
		
		System.out.println("AdDataBase init...");
		readCustomerID2MainCate(conf);
		
		//String filenum = conf.get(SYMBOLIC_LINK + name + CONF_ADFILE_NUM);
		//int fnum = Integer.parseInt(filenum);
		//if (filenum == null || filenum.isEmpty()) {
		//	throw new RuntimeException("filename is null");
		//}
		//for (int i = 0; i < fnum; i++) {
		//	String path = SYMBOLIC_LINK + name + i;
		//	loadData(conf, path);
		//}
		String path = MID_AD_INPUT;
		loadData(conf, path);
		System.out.println("transMap.size="+transAdMap.size());
		System.out.println("AdDataBase init Success!");

	}
	
	public void readCustomerID2MainCate(Configuration conf){
		System.out.println("read CustomerID2MainCate...");
		String file = conf.get("customer_cate");
		try {
			BufferedReader br = new BufferedReader(new FileReader(file));
			String line = null;
			while ((line = br.readLine()) != null) {
				line = line.trim();
				String[] tmp = line.split(Constants.CTRL_A);
				AdvertiserId2MainCate.put(Long.parseLong(tmp[0]),
						Long.parseLong(tmp[1]));
			}
			br.close();
		}catch (Exception e) {
			throw new RuntimeException(e);
		}
		System.out.println("AdvertiserId2MainCate.size()="+AdvertiserId2MainCate.size());
	}
	
	private void loadData(Configuration conf, String path) throws IOException, URISyntaxException{
        Path[] cached = DistributedCache.getLocalCacheFiles(conf);
        System.out.println(cached.length);
        for(int i = 0; i < cached.length; i++){
            System.out.println(cached[i].toString());
        }
		System.out.println("load File:" + path);
		BytesWritable key = new BytesWritable();
		BytesWritable val = new BytesWritable();
		FileSystem fs = FileSystem.get(new URI("file:///"), conf);
		SequenceFile.Reader reader = new SequenceFile.Reader(fs,
				new Path(path), conf);
		while (reader.next(key, val)) {
			transBuilder.clear().mergeFrom(val.getBytes(), 0, val.getLength());
			insertToMap(transBuilder.build());
		}
		reader.close();
	}
	
	
	//@function turn each ader, plan to a board, record
	private void insertToMap(TransInfo  transinfo) {
		if (!transinfo.hasBase() || !transinfo.getBase().hasBidType()) {
			return ;
		}
		// "bid_type == 1" indicate that this trans is black box cpm.
		// aurora is only applied to black box cpm now.	
		// 2013/2/21 add white box cpm.
		if (transinfo.getBase().getBidType() != 1 && transinfo.getBase().getBidType() != 2) { 
			return;
		}
		long trans_id=transinfo.getBase().getTransId();
		//get All adboard of this Trans
		List <Ad> adList = TransInfo2Ad(transinfo);
		for(Ad eachAd : adList) {			
			long board_id = eachAd.getAdboardId();
			transAdMap.put(trans_id+Constants.CTRL_A+board_id,eachAd);			
		}
 	}
	
	
	public List<Ad> TransInfo2Ad(TransInfo t) {
		tmpLabel.clear();
		List<Ad> adlists = new ArrayList<Ad>();
		TransInfoBase base = t.getBase();
		
		
		
		//for each ad, assign all targeting infomation
		ad.clear();
		long transid = transBuilder.getBase().getTransId();
		ad.setTransId(transid);
		ad.setCustomerId(transBuilder.getBase().getAdvertiserId());
		
		for (DestInfo destinfo : t.getDestInfoList()) {
			tmpLabel.clear();
			tmpLabel.setType(destinfo.getDestType());
			
			for (long typeValue : destinfo.getFieldValsList()) {
				
				tmpTag.clear();
				tmpTag.setId(typeValue);
				tmpLabel.addTags(tmpTag);
				
			}
			
			ad.addLabels(tmpLabel.build());
		}
		
		
		long mainCate = 0;
		if (AdvertiserId2MainCate.containsKey(base.getAdvertiserId())) {
			mainCate = AdvertiserId2MainCate.get(base.getAdvertiserId());
			find_main_cate++;
		} else {
			mainCate = 0;
			unfind_main_cate++;
		}
		/*
		long shopId = 0;
		if (Customid2Shopid.containsKey(transBuilder.getBase().getAdvertiserId())) {
			shopId = Customid2Shopid.get(transBuilder.getBase().getAdvertiserId());
		} else {
			shopId = 0;
		}
		*/
		//add mainCate
		ad.setMaincate(mainCate);
		//ad.setShopId(shopId);
		

		for (BoardInfo board : t.getBoardInfoList()) {
			ad.setAdboardId(board.getBoardId());
			adlists.add(ad.build());
		}
		return adlists;
	}
	
	
	
	public Ad getAdInfo(long tranId,long adBoardId) {
		String key = tranId + Constants.CTRL_A + adBoardId;
		Ad adinfo = transAdMap.get(key);
		return adinfo;
		
	}
	
	public long getUnfindMainCate(){
		return unfind_main_cate;
	}
	public long getFindMainCate(){
		return find_main_cate;
	}
	
	/*
	public static void addPath(Configuration conf, String adpath) throws URISyntaxException, IOException {			
        System.out.println("TESTTTTT");
        System.out.println(adpath);
		addPath(conf, adpath, "default");
	}
	
	public static void addPath(Configuration conf, String adpath, String adfindername) throws URISyntaxException, IOException {			
		int suffix = 0;
		if (adfindername == null || adfindername.isEmpty()) {
			adfindername = SYMBOLIC_LINK + "default";
		} else {
			adfindername = SYMBOLIC_LINK + adfindername;
		}
		Path cachePath = new Path(adpath);	
		FileSystem fs = FileSystem.get(conf);
		
		DistributedCache.createSymlink(conf);
		FileStatus fstatus = fs.getFileStatus(cachePath);
		if (fstatus.isDir()) {
			FileStatus[] subfstatus = fs.listStatus(cachePath);
			for (FileStatus status : subfstatus) {
				if (status.isDir()) {
					continue;
				}
                
				DistributedCache.addCacheFile(makeSymbolicLink(status.getPath(), adfindername, suffix++), conf);
			}
			if (suffix == 0) {
				throw new RuntimeException("no file in the input path.");
			}
		} else {
			DistributedCache.addCacheFile(makeSymbolicLink(fstatus.getPath(), adfindername, suffix++), conf);
		}
		conf.set(adfindername + CONF_ADFILE_NUM, String.valueOf(suffix));
	}
	
	private static URI makeSymbolicLink(Path path, String adFinderName, int suffix) throws URISyntaxException {
		StringBuffer strbuf = new StringBuffer();	
		strbuf.append(path.toString());
		strbuf.append("#");
		strbuf.append(adFinderName);
		strbuf.append(suffix);
		return new URI(strbuf.toString());
	}
	*/
}
