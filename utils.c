//#include <string.h>
/*
void intToStr(int nb, char* str)
{
	int i=0;
	int cont=1;
	int neg=0;
	if(nb<0)
	{
		neg=1;
		nb=-nb;
	}
	while(cont)
	{
		str[i++] = '0'+nb%10;
		nb = nb/10;
		if(nb==0) cont=0;
	}
	if(neg)
		str[i++]='-';
	str[i] = 0;
	unsigned char temp;
	unsigned char k;
	for(k=0;k<i/2;k++)
	{
		temp = str[k];
		str[k] = str[i-k-1];
		str[i-k-1]=temp;
	}
}
*/
void doubleToStr(double dnb, char* str)
{
	int nb = (int)(dnb*10);
	//intToStr(nb,str);
	
  str[0] = '0'+nb/1000;
  nb = nb%1000;
  str[1] = '0'+ nb/100;
  nb = nb%100;
  str[2] = '0'+ nb/10;
  nb = nb%10;
  str[3] = '.';
  str[4] = '0'+ nb;
}

/*
int strlen(char* str)
{
	int i=0;
	while((str[i]!='\0') && (i<20))
		i++;
	return i;
}
*/
