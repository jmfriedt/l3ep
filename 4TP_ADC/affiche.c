void affiche(unsigned short v,char *s)
{char c;
 c=(v>>12)&0x000f;if (c<10) s[0]=c+'0'; else s[0]=c-10+'A';
 c=(v>> 8)&0x000f;if (c<10) s[1]=c+'0'; else s[1]=c-10+'A';
 c=(v>> 4)&0x000f;if (c<10) s[2]=c+'0'; else s[2]=c-10+'A';
 c=(v    )&0x000f;if (c<10) s[3]=c+'0'; else s[3]=c-10+'A';
}
