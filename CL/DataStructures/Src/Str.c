#include "CodeLib.h"

#define MAX_DIGITS_IN_LONG_INT	11

static int32_t _strnCpy(uint8_t* pcBuffer, int32_t lCount, const uint8_t* pcStr) {
	int32_t copiedCount = 0;
	while (*pcStr != '\0' && ((lCount > 0 && copiedCount < lCount - 1) || lCount == 0)) {
		*pcBuffer = *pcStr;
		pcBuffer++;
		pcStr++;
		copiedCount++;
	}
	*pcBuffer = '\0';
	return copiedCount;
}

static int32_t _strnCmp(const uint8_t* pcStr1, const uint8_t* pcStr2, int32_t lCompareCount) {
	int32_t comparedCount = 1;
	while (*pcStr1 != '\0' && *pcStr2 != '\0' && *pcStr1 == *pcStr2) {
		pcStr1++;
		pcStr2++;
		comparedCount++;
		if (comparedCount == lCompareCount)
			break;
	}
	return (*pcStr1 < *pcStr2) ? -comparedCount : comparedCount;
}

int32_t lStrLen(const uint8_t* pcStr) {
	int32_t length = -1;
	if (pcStr != libNULL) {
		while (++length, *pcStr != '\0') {
			pcStr++;
		}
	}
	return length;
}

int32_t lStrCmp(const uint8_t* pcStr1, const uint8_t* pcStr2) {
	if (pcStr1 != libNULL && pcStr2 != libNULL) {
		if (pcStr1 == pcStr2) {
			return lStrLen(pcStr1) + 1;
		}
		return _strnCmp(pcStr1, pcStr2, 0);
	}
	return 0;
}

int32_t lStrCaseCmp(const uint8_t* pcStr1, const uint8_t* pcStr2) {
	if (pcStr1 != libNULL && pcStr2 != libNULL) {
		int32_t comparedCount = 1;
		while (*pcStr1 != '\0' && *pcStr2 != '\0' && TO_LOWER(*pcStr1) == TO_LOWER(*pcStr2)) {
			pcStr1++;
			pcStr2++;
			comparedCount++;
		}
		return (*pcStr1 < *pcStr2) ? -comparedCount : comparedCount;	
	}
	return 0;
}

int32_t lStrEqual(const uint8_t* pcStr1, const uint8_t* pcStr2) {
	int32_t comparedCount = lStrCmp(pcStr1, pcStr2);
	if (comparedCount > 0 && pcStr1[comparedCount - 1] == '\0' && pcStr2[comparedCount - 1] == '\0') {
		return 1;
	}
	return 0;
}

int32_t lStrCpy(uint8_t* pcBuffer, const uint8_t* pcStr) {
	if (pcBuffer != libNULL && pcStr != libNULL && pcBuffer != pcStr) {
		return _strnCpy(pcBuffer, 0, pcStr);
	}
	return -1;
}

int32_t lStrnCpy(uint8_t* pcBuffer, int32_t lCount, const uint8_t* pcStr) {
	if (pcBuffer != libNULL && pcStr != libNULL && pcBuffer != pcStr && lCount > 0) {
		return _strnCpy(pcBuffer, lCount, pcStr);
	}
	return -1;
}

int32_t lStrCat(uint8_t* pcBuffer, const uint8_t* pcStr1, const uint8_t* pcStr2) {
	int32_t length = -1;
	if (pcStr1 != libNULL && pcStr2 != libNULL && pcBuffer != libNULL && pcBuffer != pcStr1 && pcBuffer != pcStr2) {
		length = _strnCpy(pcBuffer, 0, pcStr1);
		length += _strnCpy(&(pcBuffer[length]), 0, pcStr2);
	}
	return length;
}

int32_t lStrSrc(const uint8_t* pcStr1, const uint8_t* pcStr2) {
	if (pcStr1 != libNULL && pcStr2 != libNULL  && *pcStr1 != '\0' && *pcStr2 != '\0') {
		int32_t comparedCount = 0;
		int32_t offset = 0;
		while (pcStr1[offset] != '\0') {
			if (&(pcStr1[offset]) == pcStr2) {
				return offset;
			}
			comparedCount = _strnCmp(&(pcStr1[offset]), pcStr2, 0);
			if (comparedCount > 0 && pcStr2[comparedCount - 1] == '\0') {
				return offset;
			}
			offset++;
		}
	}
	return -1;
}

int32_t lStrnSrc(const uint8_t* pcStr1, int32_t lLength, const uint8_t* pcStr2) {
	if (pcStr1 != libNULL && pcStr2 != libNULL && *pcStr1 != '\0' && *pcStr2 != '\0') {
		int32_t comparedCount = 0;
		int32_t offset = 0;
		int32_t str2Len = lStrLen(pcStr2);
		str2Len = str2Len > lLength ? lLength : str2Len;
		while (pcStr1[offset] != '\0' && lLength >= str2Len) {
			comparedCount = _strnCmp(&(pcStr1[offset]), pcStr2, lLength);
			if ((comparedCount > 0 && pcStr2[comparedCount - 1] == '\0') ||
				(comparedCount == str2Len && pcStr2[comparedCount - 1] == pcStr1[offset + comparedCount - 1])) {
				return offset;
			}
			offset++;
			lLength--;
		}
	}
	return -1;
}

int32_t lStrnSrcLast(const uint8_t* pcStr1, int32_t lLength, const uint8_t* pcStr2) {
	if (lLength > 0 && pcStr1 != libNULL && pcStr2 != libNULL && *pcStr2 != '\0' && *pcStr1 != '\0') {
		int32_t str1Len = lStrLen(pcStr1);
		int32_t str2Len = lStrLen(pcStr2);
		if (str2Len <= lLength && str2Len <= str1Len) {
			const uint8_t *str1Cursor = libNULL, *str2Cursor = libNULL;
			const uint8_t *str2End = pcStr2 + str2Len, *str1End = pcStr1 + MIN(lLength, str1Len);
			while (str1End != pcStr1) {
				str1Cursor = --str1End;
				str2Cursor = str2End - 1;
				if (*str1Cursor == *str2Cursor) {
					do {
						if (str2Cursor == pcStr2) {
							return (str1Cursor - pcStr1);
						}
						if (str1Cursor == pcStr1) {
							return -1;
						}
						str1Cursor--;
						str2Cursor--;
					} while (*str1Cursor == *str2Cursor);
				}
			}
		}
	}
	return -1;
}

int32_t lLongToStr(uint8_t* pcBufer, int32_t lValue) {
	int32_t count = -1;
	if (pcBufer != libNULL) {
		if (lValue == -2147483648) {
			count = lStrCpy(pcBufer, (const uint8_t *)"-2147483648"); 
		}
		else if (lValue == 0) {
			*pcBufer = '0';
			++pcBufer;
			*pcBufer = '\0';
			count = 1;
		}
		else {
			count = MAX_DIGITS_IN_LONG_INT;
			uint8_t longStr[count + 1];
			int32_t k = lValue;
			lValue = ABS(lValue);
			longStr[count] = '\0';
			while (lValue != 0) {
				count--;
				longStr[count] = '0' + lValue % 10;
				lValue /= 10;
			}
			if (k < 0) {
				count--;
				longStr[count] = '-';
			}
			count = _strnCpy(pcBufer, 0, &(longStr[count]));
		}
	}
	return count;
}

int32_t lStrToLong(uint32_t *pulValue, uint8_t *pcStrValue, uint8_t ucBase) {
	if ((ucBase != 2) && (ucBase != 8) && (ucBase != 10) && (ucBase != 16)) return 0; 
	int32_t result = 0;
	if (pcStrValue != libNULL) {
		uint32_t Value = 0;
		int8_t isSignedInt = 0;
		if(ucBase == 10) {
			if (*pcStrValue == '-' || *pcStrValue == '+') {
				if (*pcStrValue == '-') {
					isSignedInt = 1;
				}
				pcStrValue++;
			}
		}
		while (IS_DIGIT(*pcStrValue) || ((ucBase == 16) && IS_HEX(*pcStrValue))) {
			uint8_t x = CHAR_TO_HEX(*pcStrValue);// IS_ALPHA(*pcStrValue)? 10 + (TO_LOWER(*pcStrValue) - 'a'): ((*pcStrValue) - '0');
			if(x >= ucBase) break;
			++result;
			Value = Value * ucBase + x;
			pcStrValue++;
		}
		if (isSignedInt == 1) {
			Value = ~(Value - 1);
		}
		if(pulValue != libNULL) {
			*pulValue = Value;
		}
	}
	return result;
}

void vStrToLowerCase(uint8_t *pcStr) {
	if (pcStr != libNULL) {
		while (*pcStr != '\0') {
			if (IS_UPALPHA(*pcStr)) {
				*pcStr -= 'A' - 'a';
			}
			pcStr++;
		}
	}
}

void vStrToUpperCase(uint8_t *pcStr) {
	if (pcStr != libNULL) {
		while (*pcStr != '\0') {
			if (IS_LOALPHA(*pcStr)) {
				*pcStr += 'A' - 'a';
			}
			pcStr++;
		}
	}
}

/*!
  Snake notation
*/

int32_t str_len(const uint8_t* str) __attribute__ ((alias ("lStrLen")));
int32_t str_cmp(const uint8_t* str1, const uint8_t* str2) __attribute__ ((alias ("lStrCmp")));
int32_t str_case_cmp(const uint8_t* str1, const uint8_t* str2) __attribute__ ((alias ("lStrCaseCmp")));
int32_t str_equal(const uint8_t* str1, const uint8_t* str2) __attribute__ ((alias ("lStrEqual")));
int32_t str_cpy(uint8_t* buffer, const uint8_t* str) __attribute__ ((alias ("lStrCpy")));
int32_t strn_cpy(uint8_t* buffer, int32_t count, const uint8_t* str) __attribute__ ((alias ("lStrnCpy")));
int32_t str_cat(uint8_t* buffer, const uint8_t* str1, const uint8_t* str2) __attribute__ ((alias ("lStrCat")));
int32_t str_src(const uint8_t* str1, const uint8_t* str2) __attribute__ ((alias ("lStrSrc")));
int32_t strn_src(const uint8_t* str1, int32_t length, const uint8_t* str2) __attribute__ ((alias ("lStrnSrc")));
int32_t strn_src_last(const uint8_t* str1, int32_t length, const uint8_t* str2) __attribute__ ((alias ("lStrnSrcLast")));
int32_t long_to_str(uint8_t* bufer, int32_t value) __attribute__ ((alias ("lLongToStr")));
int32_t str_to_long(uint32_t *value, uint8_t *str_value, uint8_t base) __attribute__ ((alias ("lStrToLong")));
void str_to_lower_case(uint8_t *str) __attribute__ ((alias ("vStrToLowerCase")));
void str_to_upper_case(uint8_t *str) __attribute__ ((alias ("vStrToUpperCase")));
