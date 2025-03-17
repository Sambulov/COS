#include "CodeLib.h"

#define PRINTF_FLAG_ZERO_PADDING		0b000000000000000000000001UL
#define PRINTF_FLAG_ALIGNMENT_LEFT		0b000000000000000000000010UL
#define PRINTF_FLAG_SPACE_FOR_SIGN		0b000000000000000000000100UL
#define PRINTF_FLAG_FORCE_SIGN			0b000000000000000000001000UL
#define PRINTF_FLAG_VALUE_PRECEDED		0b000000000000000000010000UL

#define PRINTF_LENGTH_LONG_LONG			0b000000000000000000100000UL
#define PRINTF_LENGTH_LONG				0b000000000000000001000000UL
#define PRINTF_LENGTH_SHORT				0b000000000000000001100000UL
#define PRINTF_LENGTH_CHAR				0b000000000000000010000000UL
#define PRINTF_LENGTH_MASK				0b000000000000000111100000UL

#define PRINTF_WIDTH_PRESENT			0b000000000000001000000000UL
#define PRINTF_PRECISION_PRESENT		0b000000000000010000000000UL
#define PRINTF_SPECIFIER_UPPER_CASE		0b000000000000100000000000UL

#define PRINTF_TYPE_STRING				0b000000000001000000000000UL
#define PRINTF_TYPE_DOUBLE				0b000000000010000000000000UL
#define PRINTF_TYPE_DOUBLE_SCIENTIFIC	0b000000000100000000000000UL
#define PRINTF_TYPE_SIGNED				0b000000001000000000000000UL
#define PRINTF_TYPE_UNSIGNED_BIN		0b000000010000000000000000UL
#define PRINTF_TYPE_UNSIGNED_OCT		0b000000100000000000000000UL
#define PRINTF_TYPE_UNSIGNED_HEX		0b000000110000000000000000UL
#define PRINTF_TYPE_UNSIGNED_MASK		0b000000110000000000000000UL
#define PRINTF_TYPE_UNSIGNED_NO_PREFIX	0b000001000000000000000000UL

#define PRINTF_TYPE_TIME				0b000010000000000000000000UL
#define PRINTF_TYPE_POINTER				0b000100000000000000000000UL
#define PRINTF_TYPE_CHARACTER			0b001000000000000000000000UL
#define PRINTF_TYPE_UNKNOWN				0b010000000000000000000000UL

#define PRINTF_FLOAT_ZERO_TRUNC			0b100000000000000000000000UL

#define FIFO_FAIL    -1

static const uint32_t mantissaMultipliersTable[] = {
	0xF0BDC21A, 0x3DA137D5, 0x9DC5ADA8, 0x2863C1F5, 0x6765C793,
	0x1A784379, 0x43C33C19, 0xAD78EBC5, 0x2C68AF0B, 0x71AFD498,
	0x1D1A94A2, 0x4A817C80, 0xBEBC2000, 0x30D40000, 0x7D000000,
	0x20000000, 0x51EB851E, 0xD1B71758, 0x35AFE535, 0x89705F41,
	0x232F3302, 0x5A126E1A, 0xE69594BE, 0x3B07929F, 0x971DA050,
	0x26AF8533, 0x63090312, 0xFD87B5F2, 0x40E75996, 0xA6274BBD,
	0x2A890926, 0x6CE3EE76
};

// static uint64_t _ullMullDiv64(uint64_t a, uint64_t b) {
// 	uint32_t lowA = (a & 0xffffffff);
// 	uint32_t highA = (a >> 32);
// 	uint32_t lowB = (b & 0xffffffff);
// 	uint32_t highB = (b >> 32);
// 	uint64_t result = (uint64_t)lowA * lowB;
// 	uint64_t ab = (uint64_t)lowA * highB;
// 	uint64_t ba = (uint64_t)highA * lowB;
// 	result = (result >> 32) + (ab & 0xffffffff) + (ba & 0xffffffff);
// 	result = (result >> 32) + (uint64_t)highA * highB  + (ab >> 32) + (ba >> 32);
// 	return result;
// }

/*!
	@brief Format flags parsing.
	@param[in]pcFormat			Pointer to format string
	@param[in/out]pulCursor		Current offset in format string
	@param[in/out]pulOptions	Format options
*/
static void _vFifoPrintfParseFlags(const uint8_t* pcFormat, uint32_t *pulCursor, uint32_t *pulOptions) {
	/* Flags */
	uint8_t symbol;
	do {
		symbol = pcFormat[*pulCursor];
		/* Left-pads the number with zeroes (0) instead of spaces when padding is specified.
		   If both 0 and - appear, the 0 is ignored. */
		if ((symbol == '0') && !(*pulOptions & PRINTF_FLAG_ALIGNMENT_LEFT)) {
			*pulOptions |= PRINTF_FLAG_ZERO_PADDING;
		}
		/* If no sign is going to be written, a blank space is inserted before the value.
		   The blank is ignored if both the blank and + flags appear. */
		else if((symbol == ' ') && !(*pulOptions & PRINTF_FLAG_FORCE_SIGN)) {
			*pulOptions |= PRINTF_FLAG_SPACE_FOR_SIGN;
		}
		else if(symbol == '-') { /* Left-justify within the given field width; Right justification is the default */
			*pulOptions |= PRINTF_FLAG_ALIGNMENT_LEFT;
			*pulOptions &= ~PRINTF_FLAG_ZERO_PADDING;
		}
		else if(symbol == '+') { /* Forces to preceed the result with a plus or minus sign (+ or -) even for positive numbers.
							        By default, only negative numbers are preceded with a - sign. */
			*pulOptions |= PRINTF_FLAG_FORCE_SIGN;
			*pulOptions &= ~PRINTF_FLAG_SPACE_FOR_SIGN;
		}
		else if(symbol == '#') { /* Used with o, x or X specifiers the value is preceeded with 0, 0x or 0X respectively for values
							        different than zero.
							        Used with a, A, e, E, f, F, g or G it forces the written output to contain a decimal point
							        even if no more digits follow. By default, if no digits follow, no decimal point is written. */
			*pulOptions |= PRINTF_FLAG_VALUE_PRECEDED;
		}
		else {
			break;
		}
		(*pulCursor)++;
	} while (1);
}

/*!
	@brief Width parsing
	@param[in]pcFormat			Pointer to format string
	@param[in/out]pulCursor		Current offset in format string
	@param[in/out]pulOptions	Format options
	@param[in]xArgs				Pointer to format arguments
	@return 0 or width if present
*/
static int32_t _lFifoPrintfParseWidth(const uint8_t* pcFormat, uint32_t *pulCursor, uint32_t *pulOptions, va_list *xArgs) {
	/* Width */
	uint8_t symbol = pcFormat[*pulCursor];
	int32_t width = 0;
	/* Minimum number of characters to be printed. If the value to be printed is shorter
	   than this number, the result is padded with blank spaces. The value is not truncated
	   even if the result is larger. */
	if (symbol == '*') { /* The width is not specified in the format string, but as an additional integer value argument
						    preceding the argument that has to be formatted. */
		width = va_arg(*xArgs, int);
		if (width < 0) {
			*pulOptions |= PRINTF_FLAG_ALIGNMENT_LEFT;
			width = -width;
		}
		*pulOptions |= PRINTF_WIDTH_PRESENT;
		(*pulCursor)++;
	}
	else {
		while (IS_DIGIT(symbol)) {
			width = width * 10 + (symbol - '0');
			(*pulCursor)++;
			symbol = pcFormat[*pulCursor];
			*pulOptions |= PRINTF_WIDTH_PRESENT;
		}
	}
	return width;
}

/*!
	@brief Precision parsing
	@param[in]pcFormat			Pointer to format string
	@param[in/out]pulCursor		Current offset in format string
	@param[in/out]pulOptions	Format options
	@param[in]xArgs				Pointer to format arguments
	@return 0 or precision if present
*/
static int32_t _lFifoPrintfParsePrecision(const uint8_t* pcFormat, uint32_t *pulCursor, uint32_t *pulOptions, va_list *xArgs) {
	/* Precision */
	uint8_t symbol = pcFormat[*pulCursor];
	int32_t precision = 0;
	/* For integer specifiers(d, i, o, u, x, X) : precision specifies the minimum number of digits to be
	   written. If the value to be written is shorter than this number, the result is padded with leading
	   zeros. The value is not truncated even if the result is longer. A precision of 0 means that no
	   character is written for the value 0.
	   For a, A, e, E, f and F specifiers : this is the number of digits to be printed after the decimal
	   point(by default, this is 6).
	   For g and G specifiers : This is the maximum number of significant digits to be printed.
	   For s : this is the maximum number of characters to be printed.By default all characters are printed
	   until the ending libNULL character is encountered.
	   If the period is specified without an explicit value for precision, 0 is assumed. */
	if (symbol == '.') {
		*pulOptions |= PRINTF_PRECISION_PRESENT;
		(*pulCursor)++;
		symbol = pcFormat[*pulCursor];
		if (symbol == '*') { /* The precision is not specified in the format string, but as an additional integer
							    value argument preceding the argument that has to be formatted. */
			precision = va_arg(*xArgs, int32_t);
			if (precision < 0) {
				precision = 0;
				*pulOptions &= ~PRINTF_PRECISION_PRESENT;
			}
			(*pulCursor)++;
		}
		else {
			while (IS_DIGIT(symbol)) {
				precision = precision * 10 + (symbol - '0');
				(*pulCursor)++;
				symbol = pcFormat[*pulCursor];
			}
		}
	}
	return precision;
}

/*!
	@brief Format parameter length parsing
	@param[in]pcFormat			Pointer to format string
	@param[in/out]pulCursor		Current offset in format string
	@param[in/out]pulOptions	Format options
*/
static void _vlFifoPrintfParseLength(const uint8_t* pcFormat, uint32_t *pulCursor, uint32_t *pulOptions) {
	/* Length */
	uint8_t symbol = pcFormat[*pulCursor];
	/* The length sub-specifier modifies the length of the data type. */
	if (symbol == 'l') /* Size is long */ {
		(*pulCursor)++;
		symbol = pcFormat[*pulCursor];
		if (symbol == 'l') { /* Size is long long */
			*pulOptions |= PRINTF_LENGTH_LONG_LONG;
			(*pulCursor)++;
		}
		else {
			*pulOptions |= PRINTF_LENGTH_LONG;
		}
	}
	else if(symbol == 'h') { /* Size is short */
		(*pulCursor)++;
		symbol = pcFormat[*pulCursor];
		if (symbol == 'h') { /* Size is byte */
			*pulOptions |= PRINTF_LENGTH_CHAR;
			(*pulCursor)++;
		}
		else {
			*pulOptions |= PRINTF_LENGTH_SHORT;
		}
	}
}

/*!
	@brief Format specifier parsing
	@param[in]pcFormat			Pointer to format string
	@param[in/out]pulCursor		Current offset in format string
	@param[in/out]pulOptions	Format options
*/
static void _vlFifoPrintfParseSpecifier(const uint8_t* pcFormat, uint32_t *pulCursor, uint32_t *pulOptions) {
	/* Specifier */
	uint8_t symbol = pcFormat[*pulCursor];

	if (IS_UPALPHA(symbol)) {
		*pulOptions |= PRINTF_SPECIFIER_UPPER_CASE;
	}
	else {
		symbol = TO_UPPER(symbol);
	}
	switch (symbol) { /* Type is... */
	case 'S' :					/* String */
		*pulOptions |= PRINTF_TYPE_STRING;
		break;
	case 'C' :					/* Character */
		*pulOptions |= PRINTF_TYPE_CHARACTER;
		break;
	case 'T' :					/* Time */
		*pulOptions |= PRINTF_TYPE_TIME;
		break;
	case 'P' :					/* Pointer address */
		*pulOptions |= PRINTF_TYPE_POINTER;
		break;
	case 'B' :					/* Binary */
		*pulOptions |= PRINTF_TYPE_UNSIGNED_BIN;
		*pulOptions &= ~PRINTF_FLAG_FORCE_SIGN & ~PRINTF_FLAG_SPACE_FOR_SIGN;
		break;
	case 'O' :					/* Octal */
		*pulOptions |= PRINTF_TYPE_UNSIGNED_OCT;
		*pulOptions &= ~PRINTF_FLAG_FORCE_SIGN & ~PRINTF_FLAG_SPACE_FOR_SIGN;
		break;
	case 'I' :					/* Signed decimal */
	case 'D' :					/* Signed decimal */
		*pulOptions |= PRINTF_TYPE_SIGNED;
		/* fall through */
	case 'U' :					/* Unsigned decimal */
		*pulOptions &= ~PRINTF_FLAG_VALUE_PRECEDED;
		break;
	case 'X' :					/* Hexdecimal */
		*pulOptions |= PRINTF_TYPE_UNSIGNED_HEX;
		*pulOptions &= ~PRINTF_FLAG_FORCE_SIGN & ~PRINTF_FLAG_SPACE_FOR_SIGN;
		break;
	case 'F' :					/* Float */
		*pulOptions |= PRINTF_TYPE_DOUBLE;
		break;
	case 'E' :
		*pulOptions |= PRINTF_TYPE_DOUBLE_SCIENTIFIC;
		break;
	case 'G' :
		*pulOptions |= PRINTF_TYPE_DOUBLE | PRINTF_TYPE_DOUBLE_SCIENTIFIC | PRINTF_FLOAT_ZERO_TRUNC;
		break;
	default:
		*pulOptions |= PRINTF_TYPE_UNKNOWN;
		return;
	}
	(*pulCursor)++;
}


/*!
	@brief Write formated string to stream buffer
	@param[in]xFifo			Basic stream descriptor, must be output stream
	@param[in]pcString			Pointer to string to ouput
	@param[in]ulCount			Format option
	@param[in]ulWidth			Format option
	@param[in]ulOptions			Format option
	@return Fifoed bytes count or STREAM_FAIL if data doesn't fits in stream buffer
*/
static int32_t _lFifoPrintfPrintString(Fifo_t *xFifo, uint8_t *pcString, uint32_t ulCount, int32_t lWidth, uint32_t ulOptions) {
	//if (flags & PRINTF_LENGTH_LONG) //yet unsuported wchar string
	int32_t strLength = lStrLen(pcString);
	int32_t streamed = 0;
	int32_t result;
	if (ulOptions & PRINTF_PRECISION_PRESENT) {
		strLength = (strLength > (int32_t)ulCount) ? (int32_t)ulCount : strLength;
	}
	lWidth -= strLength;
	int32_t stage = 0;
	if (ulOptions & PRINTF_FLAG_ALIGNMENT_LEFT) {
		stage = 2;
	}
	if (lWidth <= 0) {
		stage = 1;
	}
	while (stage < 4) {
		switch (stage) {
		case 0:
		case 3:
			result = lFifoFillAll(xFifo, ' ', lWidth);
			break;
		case 1:
			stage = 3;
			/* fall through */
		case 2:
			result = lFifoWriteStringAll(xFifo, pcString);
			break;
		default:
			result = 0;
			break;
		}
		if (result < 0) {
			return result;
		}
		stage++;
		streamed += result;
	}
	return streamed;
}

/*!
	@brief Extract decimal representation and exponent of 10 of float value
	@param[in]fpValue			Float value to prepare
	@param[out]plOut10Exponent	Exponent of 10
	@param[out]pulOutValue		Significant digits magic value
	@return 0 - positive, 1 - negative, 0xFF - +infinity, 0xFE - -infinity, 0xFD - NaN, 0xFC - subnormal or zero
*/
static int32_t _lFloatToStrPreapare(float fpValue, int32_t *plOut10Exponent, uint32_t *pulOutValue) {
	uint32_t uvalue = *(uint32_t *)(&fpValue);
	uint8_t exponent = (uint8_t)(uvalue >> 23);
	uint32_t fraction = (uvalue & 0x00ffffff) | 0x00800000;
	if (exponent == 0) { // don't care about a subnormals
			return 0xfc; // subnormal
	}
	if (exponent == 0xff) {
		if (fraction & 0x007fffff) {
			return 0xfd; // NaN
		}
		if (uvalue & 0x80000000UL) {
			return 0xfe; // -Infinity
		}
		return 0xff; // +Infinity
	}
	*plOut10Exponent = ((((exponent >> 3)) * 77 + 63) >> 5) - 38;  // convert exponent of 2 to exponent of 10
	fraction <<= 8;
	*pulOutValue = (uint32_t)(((uint64_t)(fraction) * mantissaMultipliersTable[exponent >> 3]) >> 32) + 1;
	*pulOutValue >>= (7 - (exponent & 7));
	while (!(*pulOutValue & 0xf0000000)) {
		*pulOutValue = *pulOutValue * 10;
		(*plOut10Exponent)--;
	}
	if (uvalue & 0x80000000UL) {
		return 1;
	}
	return 0;
}

/*!
	@brief Write formated float to stream buffer
	@param[in]pxFifo			Fifo descriptor
	@param[in]fValue			Float value to output
	@param[in]ulOptions			Format option
	@param[in]ulWidth			Format option
	@param[in]lPrecision		Format option
	@return writed bytes count
*/
static int32_t _lFifoPrintFloat(Fifo_t *pxFifo, float fpValue, uint32_t ulOptions, int32_t width, int32_t precision) {
#define FLOAT_BUFFER_SIZE		15
	uint8_t buffer[FLOAT_BUFFER_SIZE + 3];
	uint8_t *sign = &(buffer[0]);
	*sign = '\0';
	buffer[1] = '0'; /* Reserv for rounding operation */
	uint8_t *value = &(buffer[2]);
	*value = '\0';
	uint32_t significantDigits;
	int32_t exp10;
	int32_t result, streamed = 0;
	int32_t stage = 0;
	int32_t extructCount = 0;
	switch (_lFloatToStrPreapare(fpValue, &exp10, &significantDigits)) {
	case 1:
		*sign = '-';
		break;
	case 0:
		if (ulOptions & PRINTF_FLAG_FORCE_SIGN) {
			*sign = '+';
		}
		else if (ulOptions & PRINTF_FLAG_SPACE_FOR_SIGN) {
			*sign = ' ';
		}
		break;
	case 0xfc:
		value[0] = '0';
		if (!(ulOptions & PRINTF_TYPE_DOUBLE)) {
			value[1] = '.';
			value[2] = '0';
			value[4] = '\0';
		}
		else {
			value[1] = '\0';
		}
		break;
	case 0xfd:
		value = (uint8_t *)"NaN";
		width -= 3;
		break;
	case 0xfe:
		value = (uint8_t *)"-Infinity";
		width -= 9;
		break;
	case 0xff:
		value = (uint8_t *)"+Infinity";
		width -= 9;
		break;
	default:
		break;
	}
	if (*value != '\0') {
		ulOptions &= ~PRINTF_TYPE_DOUBLE_SCIENTIFIC;
		ulOptions &= ~PRINTF_TYPE_DOUBLE;
		precision = 0;
		stage = 1;
	}
	else {
		if (!(ulOptions & PRINTF_PRECISION_PRESENT)) { /* Set precision */
			precision = 6;
		}
		if ((ulOptions & PRINTF_TYPE_DOUBLE) && (ulOptions & PRINTF_TYPE_DOUBLE_SCIENTIFIC)) { /* Specifier G; Determine presentation form */
			/*
			Signed values are displayed in f or e format, whichever is more compact for the given value and precision. The e format is
			used only when the exponent of the value is less than -4 or greater than or equal to the precision argument.
			*/
			if (exp10 >= precision || exp10 < -4) {
				/* use scientific presentation */
				ulOptions &= ~PRINTF_TYPE_DOUBLE;
			}
			else {
				/* use decimal presentation */
				ulOptions &= ~PRINTF_TYPE_DOUBLE_SCIENTIFIC;
				if (exp10 > 0) {
					precision -= exp10;
				}
			}
			precision--;
			if (precision < 0) {
				precision = 0;
			}
		}

		if (ulOptions & PRINTF_TYPE_DOUBLE_SCIENTIFIC) {
			width -= 5;  /* reserv place for exponent: X[.{X}]EsXX */
			if((ulOptions & PRINTF_FLOAT_ZERO_TRUNC) && (precision > 14)) {
				precision = 14;
			}
		}
		else {
			width -= (exp10 < 0) ? 1 : exp10 + 1;
		}
		width -= ((precision > 0) ? precision + 1 : 0) + (*sign != '\0' ? 1 : 0);

		if ((ulOptions & PRINTF_TYPE_DOUBLE) && (-precision < exp10 + 1)) {
			extructCount = exp10 + precision + 2;  /* +1 to exponent and +1 to raund */
		}
		else if (ulOptions & PRINTF_TYPE_DOUBLE_SCIENTIFIC) {
			extructCount = precision + 2;
		}
		if (extructCount > 0) {
			int32_t offset = 0;
			while ((offset < extructCount) && (offset < FLOAT_BUFFER_SIZE)) { /* Extract significant digits to string buffer */
				value[offset++] = (uint8_t)(significantDigits >> 28) + '0';
				significantDigits &= 0x0fffffff;
				significantDigits = significantDigits * 10;
			}
			/* Roundup */
			if (offset == extructCount) { /* if extructed all required */
				if (value[--offset] >= '5') {
					value[offset] = '\0';
					value[--offset]++;
					while (value[offset] > '9') {
						value[offset--] -= 10;
						value[offset]++;
					}
					if (offset == -1) {
						value = &(value[offset]);
						exp10++;
						if (ulOptions & PRINTF_TYPE_DOUBLE) {
							width--;
						}
					}
				}
				else {
					value[offset] = '\0';
				}
			}
			else {
				extructCount = extructCount > offset ? offset : extructCount;
			}
		}
		stage = 0;
		if (!(ulOptions & PRINTF_FLAG_ALIGNMENT_LEFT) && !(ulOptions & PRINTF_FLAG_ZERO_PADDING)) {
			stage = 1;
		}
	}
	uint8_t filler = ' ';
	do {
		result = 0;
		switch (stage) {
		case 0: /* print sign */
		case 2 :
			if(*sign != '\0') {
				if (!bFifoWriteByte(pxFifo, *sign)) {
					result = FIFO_FAIL;
					break;
				}
				result = 1;
				*sign = '\0';
			}
			filler = '0';
			break;
		case 1: /* padding */
			if(ulOptions & PRINTF_FLAG_ALIGNMENT_LEFT) break;
			/* fall through */
		case 5: /* trailing blanks */
			if(width > 0) {
				result = lFifoFillAll(pxFifo, filler, width);
				width = 0;
			}
			break;
		case 3:	/* print value */
			if((ulOptions & PRINTF_TYPE_DOUBLE_SCIENTIFIC) || (ulOptions & PRINTF_TYPE_DOUBLE)) {
				int32_t memCount = 1;
				int32_t curretExp = 0;
				int32_t offset = 0;
				uint8_t decimalPointIsSet = 0;
				if (!(ulOptions & PRINTF_TYPE_DOUBLE_SCIENTIFIC)) {
					if (exp10 > 0) {
						curretExp = exp10;
					}
					offset += exp10;
				}

				while (curretExp >= -precision) {
					if (memCount == 0) {
						return FIFO_FAIL;
					}
					if ((!decimalPointIsSet) && (curretExp == -1)) {
						decimalPointIsSet = 1;
						memCount = bFifoWriteByte(pxFifo, '.');
						curretExp++;
					}
					else {
						if (offset - curretExp < extructCount && offset - curretExp >= 0) {
							memCount = bFifoWriteByte(pxFifo, value[offset - curretExp]);
						}
						else {
							memCount = bFifoWriteByte(pxFifo, '0');
						}
					}
					curretExp--;
					result++;
				}
			}
			else {
				result = lFifoWriteStringAll(pxFifo, value);
			}
			filler = ' ';
			break;
		case 4:	/* print exponent */
			if(ulOptions & PRINTF_TYPE_DOUBLE_SCIENTIFIC) {
				buffer[0] = (ulOptions&PRINTF_SPECIFIER_UPPER_CASE) ?  'E' : 'e';
				buffer[1] = exp10 >= 0 ? '+' : '-';
				exp10 = exp10 > 0 ? exp10 : -exp10;
				buffer[2] = (uint8_t)((exp10 / 10) % 10) + '0';
				buffer[3] = (uint8_t)(exp10 % 10) + '0';
				buffer[4] = '\0';
				result = lFifoWriteAll(pxFifo, (uint8_t *)buffer, 4);
			}
			break;
		default:
			break;
		}
		if (result == FIFO_FAIL) {
			return result;
		}
		streamed += result;
		stage++;
	} while (result >= 0 && stage < 6);

	return streamed;
}

/*!
	@brief Write formated integer to stream buffer
	@param[in]xFifo			Basic stream descriptor, must be output stream
	@param[in]ullValue			Integer value to output
	@param[in]ulOptions			Format option
	@param[in]ulWidth			Format option
	@param[in]lPrecision		Format option
	@return Fifoed bytes count or STREAM_FAIL if data doesn't fits in stream buffer
*/
static int32_t _lFifoPrintInteger(Fifo_t *xFifo, uint64_t ullValue, uint32_t ulOptions, int32_t lWidth, int32_t lPrecision) {
	uint8_t prefix[3];
	prefix[0] = '\0';
	if (ullValue & 0x8000000000000000ULL && (ulOptions & PRINTF_TYPE_SIGNED)) {
		ullValue = (ullValue ^ 0xffffffffffffffffULL) + 1;
		prefix[0] = '-';
	}
	else {
		if (ulOptions & PRINTF_FLAG_SPACE_FOR_SIGN) {
			prefix[0] = ' ';
		}
		else if (ulOptions & PRINTF_FLAG_FORCE_SIGN) {
			prefix[0] = '+';
		}
	}
	prefix[1] = '\0';
	int32_t notation = 10;
	switch (ulOptions & PRINTF_TYPE_UNSIGNED_MASK) {
	case PRINTF_TYPE_UNSIGNED_BIN:
		notation = 2;
		prefix[1] = 'b';
		break;
	case PRINTF_TYPE_UNSIGNED_OCT:
		notation = 8;
		prefix[1] = '\0';
		break;
	case PRINTF_TYPE_UNSIGNED_HEX:
		notation = 16;
		prefix[1] = 'x';
		break;
	default:
		break;
	}
	if (prefix[0] != '\0') {
		lWidth--;
	}
	else if (ulOptions & PRINTF_FLAG_VALUE_PRECEDED) {
		if (ulOptions & PRINTF_SPECIFIER_UPPER_CASE) {
			prefix[1] = TO_UPPER(prefix[1]);
		}
		if (prefix[1] != '\0') {
			lWidth--;
		}
		prefix[0] = '0';
		prefix[2] = '\0';
		lWidth--;
	}
	int32_t intLen = 1;
	uint64_t tmp = ullValue / notation;
	while (tmp > 0) {
		tmp /= notation;
		intLen++;
	}
	lWidth -= intLen;
	if (ulOptions & PRINTF_PRECISION_PRESENT) {
		lPrecision -= intLen;
		if (lPrecision > 0) {
			lWidth -= lPrecision;
		}
	}
	else if (ulOptions & PRINTF_FLAG_ZERO_PADDING) {
		lPrecision = lWidth;
		lWidth = 0;
	}
	int32_t result;
	int32_t streamed = 0;
	int32_t stage = 0;
	while (stage < 5) {
		result = 0;
		switch (stage) {
		case 0:
			if (!(ulOptions & PRINTF_FLAG_ALIGNMENT_LEFT) && lWidth > 0) {
				result = lFifoFillAll(xFifo, ' ', lWidth);
				lWidth = 0;
			}
			break;
		case 1:
			if (!(ulOptions & PRINTF_TYPE_UNSIGNED_NO_PREFIX) && prefix[0] != '\0') {
				result = lFifoWriteStringAll(xFifo, prefix);
			}
			break;
		case 2:
			if (lPrecision > 0) {
				result = lFifoFillAll(xFifo, '0', lPrecision);
			}
			break;
		case 3: {
				uint8_t integerStr[intLen];
				uint8_t integerIndex = intLen;
				uint8_t digit;
				do {
					digit = ullValue % notation;
					if (digit > 9) {
						if (ulOptions & PRINTF_SPECIFIER_UPPER_CASE) {
							digit +=  'A' - 10;
						}
						else {
							digit +=  'a' - 10;
						}
					}
					else {
						digit += '0';
					}
					integerStr[--integerIndex] = digit;
					result++;
					ullValue /= notation;
				} while (integerIndex);
				result = lFifoWriteAll(xFifo, integerStr, intLen);
			}
			break;
		case 4:
			if (ulOptions & PRINTF_FLAG_ALIGNMENT_LEFT && lWidth > 0) {
				result = lFifoFillAll(xFifo, ' ', lWidth);
			}
		default:
			break;
		}
		if (result < 0) {
			return FIFO_FAIL;
		}
		streamed += result;
		stage++;
	}
	return streamed;
}

int32_t lFifoVPrintf(Fifo_t *xFifo, const uint8_t* pcFormat, va_list xArgs, uint32_t *pulFormatOffset) {
    if(xFifo != libNULL) return FIFO_FAIL;
	if (!bFifoTransactionBegin(xFifo)) return 0;
    int32_t streamed = 0;
    int32_t result;
    uint32_t cursor = 0;
    uint8_t symbol;
    int32_t width, precision;
    uint32_t options;
    if (pulFormatOffset != libNULL) {
        cursor = *pulFormatOffset;
    }
    while (pcFormat[cursor] != '\0') {
        result = 0;
        symbol = pcFormat[cursor++];
        if (symbol == '%') {
            /* escape character */
            if (pcFormat[cursor] == '%') {
                /* double % */
                cursor++;
            }
            else {
                /* Parsing */
                symbol = '\0';
                width = precision = options = 0;
                /* Flags */
                _vFifoPrintfParseFlags(pcFormat, &cursor, &options);
                /* Width */
                width = _lFifoPrintfParseWidth(pcFormat, &cursor, &options, &xArgs);
                /* Precision */
                precision = _lFifoPrintfParsePrecision(pcFormat, &cursor, &options, &xArgs);
                /* Length */
                _vlFifoPrintfParseLength(pcFormat, &cursor, &options);
                /* Specifier */
                _vlFifoPrintfParseSpecifier(pcFormat, &cursor, &options);
                if (!(options & PRINTF_TYPE_UNKNOWN)) {/* Format recognized, print parameter. Else unknown type, pass-through */
                    if (options & PRINTF_TYPE_CHARACTER) {
                        int32_t symb = (uint8_t)va_arg(xArgs, int32_t);
                        result = _lFifoPrintfPrintString(xFifo, (uint8_t*)&symb, 1, width, options);
                    }
                    else if (options & PRINTF_TYPE_STRING) {
                        result = _lFifoPrintfPrintString(xFifo, va_arg(xArgs, uint8_t*), precision, width, options);
                    }
                    else if (options & PRINTF_TYPE_DOUBLE || options & PRINTF_TYPE_DOUBLE_SCIENTIFIC) {
                        float fValue = (float)va_arg(xArgs, double);
                        result = _lFifoPrintFloat(xFifo, fValue, options, width, precision);
                    }
                    else { /* Print decimal */
                        uint64_t value;
                        switch (options & PRINTF_LENGTH_MASK) {
                        case PRINTF_LENGTH_LONG_LONG:
                            if (options&PRINTF_TYPE_SIGNED) {
                                value = (uint64_t)va_arg(xArgs, int64_t);
                            }
                            else {
                                value = va_arg(xArgs, uint64_t);
                            }
                            break;
                        case PRINTF_LENGTH_LONG:
                            if (options&PRINTF_TYPE_SIGNED) {
                                value = (uint64_t)va_arg(xArgs, int32_t);
                            }
                            else {
                                value = (uint64_t)va_arg(xArgs, uint32_t);
                            }
                            break;
                        case PRINTF_LENGTH_CHAR:
                        case PRINTF_LENGTH_SHORT:
                        default:
                            if (options&PRINTF_TYPE_SIGNED) {
                                value = (uint64_t)va_arg(xArgs, int32_t);
                            }
                            else {
                                value = (uint64_t)va_arg(xArgs, uint32_t);
                            }
                            break;
                        }
                        result = _lFifoPrintInteger(xFifo, value, options, width, precision);
                    }
                }
            }
        }
        if (symbol != '\0') {
            result = bFifoWriteByte(xFifo, symbol);
        }
        if (result <= 0) {
            break;
        }
        /* Sync stream */
        if(pulFormatOffset != libNULL) {
            *pulFormatOffset = cursor;
        }
        streamed += result;
    }
    if (streamed >= 0) {
        bFifoTransactionCommit(xFifo);
    }
    else {
        bFifoTransactionRollback(xFifo);
    }
    return streamed;
}

int32_t lFifoPrintInteger(Fifo_t *pxFifo, uint64_t ullValue, FifoPrintIntegerFlags_t eFlags) {
	uint32_t options = PRINTF_FLAG_ALIGNMENT_LEFT;
	if (eFlags & NO_PREFIX) {
		options = PRINTF_TYPE_UNSIGNED_NO_PREFIX;
	}
	eFlags &= INT_FLAG_MASK;
	switch (eFlags) {
	case SIGNED_INT:
		options |= PRINTF_TYPE_SIGNED;
		break;
	case BIN_INT:
		options |= PRINTF_TYPE_UNSIGNED_BIN;
		break;
	case OCT_INT:
		options |= PRINTF_TYPE_UNSIGNED_OCT;
		break;
	case HEX_INT:
		options |= PRINTF_TYPE_UNSIGNED_HEX;
		break;
	default:
		break;
	}
	return _lFifoPrintInteger(pxFifo, ullValue, options, 0, 0);
}

int32_t lFifoPrintFloat(Fifo_t *pxFifo, float fpValue) {
	int32_t options = PRINTF_TYPE_DOUBLE_SCIENTIFIC | PRINTF_TYPE_DOUBLE | PRINTF_FLOAT_ZERO_TRUNC | PRINTF_PRECISION_PRESENT;
	return _lFifoPrintFloat(pxFifo, fpValue, options, 0, 10);
}
