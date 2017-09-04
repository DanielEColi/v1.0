#include "intcoeff.h"
#include <string.h>

#if(USE_CHENHUA_METHOD==1)
double ICInTmp[CHANNEL][ICINLEN]={0};	 //输入缓冲
double ICOutTmp[CHANNEL][ICOUTLEN]={0};	 //输出缓冲
double ICOutBuf[CHANNEL]={0,0};				 //实际输出
u16 ICInTmpPos[CHANNEL]={0,0};		//当前输入缓冲位置
u16 ICOutTmpPos[CHANNEL]={0,0};		//当前输出缓冲位置
//double tst[500]={1354,1389,1389,1392,1378,1341,1353,1390,1386,1386,1377,1346,1359,1393,1387,1385,1375,1339,1349,1385,1381,1383,1378,1344,1355,1389,1384,1384,1371,1338,1356,1395,1399,1420,1450,1465,1517,1603,1647,1680,1696,1691,1726,1778,1781,1783,1772,1735,1731,1736,1703,1675,1621,1541,1508,1487,1433,1413,1389,1347,1359,1398,1392,1390,1375,1341,1358,1395,1390,1388,1376,1349,1365,1397,1388,1388,1379,1350,1358,1386,1378,1348,1267,1151,1099,1151,1385,1746,2255,2785,3278,3856,4265,4332,4049,3527,3050,2502,1921,1477,1134,1015,1071,1210,1311,1371,1378,1346,1357,1388,1387,1391,1375,1346,1362,1404,1405,1418,1411,1379,1395,1432,1433,1434,1417,1396,1429,1478,1498,1522,1515,1489,1515,1551,1556,1571,1562,1542,1563,1587,1583,1598,1603,1594,1636,1682,1690,1702,1693,1682,1717,1757,1765,1785,1793,1797,1853,1908,1933,1964,1968,1963,1996,2031,2025,2030,2006,1962,1956,1962,1927,1899,
//1853,1779,1746,1718,1644,1580,1517,1442,1419,1421,1389,1378,1354,1326,1351,1383,1376,1380,1361,1337,1361,1392,1389,1394,1372,1343,1358,1379,1372,1382,1366,1341,1359,1388,1380,1382,1357,1334,1353,1382,1381,1386,1368,1347,1369,1398,1394,1401,1374,1339,1359,1386,1382,1390,1370,1342,1364,1394,1389,1389,1360,1327,1352,1382,1375,1382,1360,1338,1359,1386,1381,1389,1362,1331,1356,1385,1379,1386,1362,1342,1367,1390,1384,1386,1370,1359,1398,1448,1490,1553,1579,1601,1661,1715,1733,1764,1750,1734,1754,1777,1764,1741,1681,1643,1636,1612,1559,1510,1422,1365,1364,1384,1381,1386,1351,1339,1369,1398,1389,1389,1351,1332,1359,1390,1392,1392,1356,1340,1370,1392,1383,1379,1339,1310,1296,1256,1177,1104,1117,1329,1792,2349,2817,3376,3889,4218,4288,3999,3562,3002,2397,1889,1400,1128,1052,1102,1169,1239,1328,1378,1378,1386,1353,1343,1377,1403,1397,1400,1368,1356,1383,1407,1406,1416,1385,1376,1414,1449,1452,1472,1451,1438,1470,1510,1522,1543,1518,1512,1550,1585,1587,1598,1578,1581,1622,1660,1669,1689,1661,1656,1698,1735,1743,1761,1744,1749,1791,1831,1853,1896,1895,1906,1959,2011,2030,2052,2019,2011,2035,2054,2033,2014,1945,1911,1914,1901,1851,1786,1682,1614,1580,1551,1512,1488,
//1414,1377,1391,1406,1396,1397,1359,1358,1386,1401,1393,1400,1367,1363,1388,1398,1393,1403,1363,1353,1377,1393,1389,1397,1359,1351,1376,1398,1396,1405,1373,1366,1389,1406,1403,1409,1376,1373,1395,1402,1392,1393,1354,1350,1380,1397,1385,1385,1353,1352,1373,1389,1390,1401,1366,1360,1386,1397,1385,1390,1357,1348,1370,1392,1401,1410,1363,1350,1378,1397,1394,1400,1364,1361,1390,1409,1412,1441,1446,1484,1546,1613,1655,1686,1679,1710,1759,1787,1791,1794,1753,1735,1737,1720};
//test
//double tstout[4][200]={0};
//double ttmp[100]={0}; 

void IntCoeffAssign(u32 val,u8 type)
{
	ICInTmp[type][ICInTmpPos[type]]=val;
}

//取输入的历史数据	数组从0开始
double xx(u8 type,u16 index )
{
	if(index>ICInTmpPos[type])
		return  ICInTmp[type][ICINLEN+ICInTmpPos[type]-index];
	else
		return 	ICInTmp[type][ICInTmpPos[type]-index];
}
//取输出的历史数据	 数组从0开始
double yy(u8 type,u16 index)
{
	if(index>ICOutTmpPos[type])
		return  ICOutTmp[type][ICOUTLEN+ICOutTmpPos[type]-index];
	else
		return 	ICOutTmp[type][ICOutTmpPos[type]-index];
}

void IntCoeffCal(u8 type)
{
//0hz
#if(SAMPLE_100HZ==1)
	//ICOutTmp[type][ICOutTmpPos[type]]=2.00*yy(type,2)-yy(type,4)+xx(type,118)-1.99944444*xx(type,120)+xx(type, 122)-0.00027778*xx(type,0)-0.00027778*xx(type,240);	
	//ICOutTmp[type][ICOutTmpPos[type]]=2*yy(type,2)-yy(type,4)-0.0002778*xx(type,240)+xx(type, 122)-1.999444*xx(type,120)+xx(type, 118)-0.0002778*xx(type,0);	
	
	ICOutTmp[type][ICOutTmpPos[type]]=2*yy(type,2)-yy(type,14)+xx(type,14)-1.96875*xx(type,16)+xx(type,18)-0.015625*xx(type,0)-0.015625*xx(type,32);
#else
	ICOutTmp[type][ICOutTmpPos[type]]=2*yy(type,1)-yy(type,2)-0.000025*xx(type,400)+1.00005*xx(type,200)-2*xx(type, 199)+xx(type,198)-0.000025*xx(type,0);	//数组下标-1		
#endif
}	  

u32 IntCoeffInvAssign(u8 type)
{
	ICOutBuf[type]=yy(type,1);
	//return ICOutBuf[type]+0xfffffff;
	return ICOutBuf[type];
} 
//所有计算结束后，更新当前输入输出指针位置
void IntCoeffNewPos(u8 type)
{
	if(++ICInTmpPos[type]>=ICINLEN)
		ICInTmpPos[type]=0;
 	if(++ICOutTmpPos[type]>=ICOUTLEN)
		ICOutTmpPos[type]=0;
}

u32 IntCoeff(u32 val,u8 type)
{
	u32 result;
	IntCoeffAssign(val,type);
	IntCoeffCal(type);
	result=IntCoeffInvAssign(type);
	IntCoeffNewPos(type);
		//test 
//		tstout[i/200][i%200]=yy(0,1);
//	}
	return result;
}
#else


volatile double XInTmp[CHANNEL][XBUFFER_SIZE]={0};	 //输入缓冲
volatile double Y1OutTmp[CHANNEL][Y1BUFFER_SIZE]={0};	 //输出缓冲
volatile double Y2OutBuf[CHANNEL][Y2BUFFER_SIZE]={0};				 //实际输出
volatile u16 XInTmpPos[CHANNEL]={0,0};		//当前输入缓冲位置
volatile u16 Y1OutTmpPos[CHANNEL]={0,0};		//当前输出缓冲位
volatile u16 Y2OutTmpPos[CHANNEL]={0,0};		//当前输出缓冲位置

void ResetIntCoeff(void)
{
	memset((void*)XInTmp,0,sizeof(XInTmp));
	memset((void*)Y1OutTmp,0,sizeof(Y1OutTmp));
	memset((void*)Y2OutBuf,0,sizeof(Y2OutBuf));
	memset((void*)XInTmpPos,0,sizeof(XInTmpPos));
	memset((void*)Y1OutTmpPos,0,sizeof(Y1OutTmpPos));
	memset((void*)Y2OutTmpPos,0,sizeof(Y2OutTmpPos));
}

s32 IntCoeff(s32 val,u8 type)
{
//k=2 m=2 r=80 q=r/k=40	
	s32 result;
	XInTmp[type][XInTmpPos[type]]=val;
//y1(n)=x(n)-x(n-80)+y1(n-2)	
	Y1OutTmp[type][Y1OutTmpPos[type]]=XInTmp[type][XInTmpPos[type]]-XInTmp[type][(XInTmpPos[type]+XBUFFER_SIZE-80)%XBUFFER_SIZE]+Y1OutTmp[type][(Y1OutTmpPos[type]+Y1BUFFER_SIZE-2)%Y1BUFFER_SIZE];
//y2(n)=y1(n)-y1(n-80)+y2(n-2)
	Y2OutBuf[type][Y2OutTmpPos[type]]=Y1OutTmp[type][Y1OutTmpPos[type]]-Y1OutTmp[type][(Y1OutTmpPos[type]+Y1BUFFER_SIZE-80)%Y1BUFFER_SIZE]+Y2OutBuf[type][(Y2OutTmpPos[type]+Y2BUFFER_SIZE-2)%Y2BUFFER_SIZE];	
//y(n)=x(n-78)-y2(n)/1600
	result=XInTmp[type][(XInTmpPos[type]+XBUFFER_SIZE-78)%XBUFFER_SIZE]-Y2OutBuf[type][Y2OutTmpPos[type]]/1600;
	//result=XInTmp[type][(XInTmpPos[type]+XBUFFER_SIZE-78)%XBUFFER_SIZE]-Y2OutBuf[type][Y2OutTmpPos[type]]/1600+0x7ffff;
	XInTmpPos[type]=(XInTmpPos[type]+1)%XBUFFER_SIZE;
	Y1OutTmpPos[type]=(Y1OutTmpPos[type]+1)%Y1BUFFER_SIZE;
	Y2OutTmpPos[type]=(Y2OutTmpPos[type]+1)%Y2BUFFER_SIZE;

	return result;
/*	
//k=2 m=2 r=28 q=r/k=14	
	u32 result;
	XInTmp[type][XInTmpPos[type]]=val;
//y1(n)=x(n)-x(n-80)+y1(n-2)	
	Y1OutTmp[type][Y1OutTmpPos[type]]=XInTmp[type][XInTmpPos[type]]-XInTmp[type][(XInTmpPos[type]+XBUFFER_SIZE-28)%XBUFFER_SIZE]+Y1OutTmp[type][(Y1OutTmpPos[type]+Y1BUFFER_SIZE-2)%Y1BUFFER_SIZE];
//y2(n)=y1(n)-y1(n-80)+y2(n-2)
	Y2OutBuf[type][Y2OutTmpPos[type]]=Y1OutTmp[type][Y1OutTmpPos[type]]-Y1OutTmp[type][(Y1OutTmpPos[type]+Y1BUFFER_SIZE-28)%Y1BUFFER_SIZE]+Y2OutBuf[type][(Y2OutTmpPos[type]+Y2BUFFER_SIZE-2)%Y2BUFFER_SIZE];	
//y(n)=x(n-78)-y2(n)/1600
	result=XInTmp[type][(XInTmpPos[type]+XBUFFER_SIZE-26)%XBUFFER_SIZE]-Y2OutBuf[type][Y2OutTmpPos[type]]/256;
	XInTmpPos[type]=(XInTmpPos[type]+1)%XBUFFER_SIZE;
	Y1OutTmpPos[type]=(Y1OutTmpPos[type]+1)%Y1BUFFER_SIZE;
	Y2OutTmpPos[type]=(Y2OutTmpPos[type]+1)%Y2BUFFER_SIZE;

	return result;*/
	
	
//k=2 m=2 r=400 q=r/k=200	
/*	s32 result;
	XInTmp[type][XInTmpPos[type]]=val;
//y1(n)=x(n)-x(n-80)+y1(n-2)	
	Y1OutTmp[type][Y1OutTmpPos[type]]=XInTmp[type][XInTmpPos[type]]-XInTmp[type][(XInTmpPos[type]+XBUFFER_SIZE-400)%XBUFFER_SIZE]+Y1OutTmp[type][(Y1OutTmpPos[type]+Y1BUFFER_SIZE-2)%Y1BUFFER_SIZE];
//y2(n)=y1(n)-y1(n-80)+y2(n-2)
	Y2OutBuf[type][Y2OutTmpPos[type]]=Y1OutTmp[type][Y1OutTmpPos[type]]-Y1OutTmp[type][(Y1OutTmpPos[type]+Y1BUFFER_SIZE-400)%Y1BUFFER_SIZE]+Y2OutBuf[type][(Y2OutTmpPos[type]+Y2BUFFER_SIZE-2)%Y2BUFFER_SIZE];	
//y(n)=x(n-78)-y2(n)/1600
	result=XInTmp[type][(XInTmpPos[type]+XBUFFER_SIZE-398)%XBUFFER_SIZE]-Y2OutBuf[type][Y2OutTmpPos[type]]/40000;
	XInTmpPos[type]=(XInTmpPos[type]+1)%XBUFFER_SIZE;
	Y1OutTmpPos[type]=(Y1OutTmpPos[type]+1)%Y1BUFFER_SIZE;
	Y2OutTmpPos[type]=(Y2OutTmpPos[type]+1)%Y2BUFFER_SIZE;

	return result;	*/
}
#endif


