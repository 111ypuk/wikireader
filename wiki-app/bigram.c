#include <string.h>
#include <stdlib.h>
#include "msg.h"
#include "bigram.h"
#include "lcd_buf_draw.h"

char aBigram[128][2];
int aCharIdx[128];
#ifndef WIKIPCF
extern int _wl_read(int, void*, unsigned int);

void init_bigram(int fd)
{
	init_char_idx();
	_wl_read(fd, aBigram, sizeof(aBigram));
}
#endif

void init_char_idx()
{
	char c;
	int i;
	int idx = 1;
	
	memset(aCharIdx, 0, sizeof(aCharIdx));
	for (i = 0; i < 128; i++)
	{
		c = (char)i;
		if (is_supported_search_char(c))
		{
			if ('a' <= c && c <= 'z')
				aCharIdx[i] = aCharIdx[(int)'A' + (c - 'a')];
			else
				aCharIdx[i] = idx++;
		}
	}
}

int bigram_char_idx(char c)
{
	return aCharIdx[(int)c];
}

void bigram_encode(char *outStr, char *inStr)
{
	int i;
	int len;
	int rc;
	char c;
	int idxMatchedBigram;
	
	*outStr = '\0';
	len = strlen(inStr);
	while (len >= 2)
	{
		idxMatchedBigram = -1;
		for (i=0; i < 128; i++)
		{
			if ((rc = memcmp(inStr, &aBigram[i][0], 2)) == 0)
			{
				idxMatchedBigram = i;
				break;
			}
			else if (rc < 0)
				break;
		}
		if (idxMatchedBigram >= 0)
		{
			c = (char)i;
			c |= 0x80;
			*outStr = c;
			outStr++;
			inStr += 2;
			len -= 2;
		}
		else
		{
			*outStr = *inStr;
			outStr++;
			inStr++;
			len--;
		}
	}

	while (len > 0)
	{
		*outStr = *inStr;
		outStr++;
		inStr++;
		len--;
	}
	*outStr = '\0';
}

void bigram_decode(char *outStr, char *inStr)
{
	unsigned char c;

	while ((c = *inStr++) != '\0')
	{
		if (c >= 128)
		{
			*outStr = aBigram[c-128][0];
			outStr++;
			*outStr = aBigram[c-128][1];
			outStr++;
		}
		else
		{
			*outStr = c;
			outStr++;
		}
	}
	*outStr = '\0';
}

int is_supported_search_char(char c)
{
	if (c && (strchr(SUPPORTED_SEARCH_CHARS, c) || ('A' <= c && c <= 'Z')))
		return 1;
	else
		return 0;
}

int search_string_cmp(char *title, char *search, int len)  // assuming search consists of lowercase only
{
	int rc = 0;
	char c = 0;
	
	while (!rc && len > 0)
	{
		c = *title;
		if (c && !is_supported_search_char(c))
		{
			title++;
		}
		else
		{
			if ('A' <= c && c <= 'Z')
				c += 32;
			if (c == *search)
			{
				title++;
				search++;
				len--;
			}
			else if (c > *search)
				rc = 1;
			else
				rc = -1;
		}
	}
	return rc;
}
		