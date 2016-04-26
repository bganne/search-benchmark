# $Id: comx_post.py,v 1.1 2004/07/08 01:14:18 alex Exp $
#

global gCDRMap
gCDRMap["boolean"]		= "bool"
gCDRMap["char"]			= "char"
gCDRMap["double"]		= "double"
gCDRMap["float"]		= "float"
gCDRMap["long"]			= "long"
gCDRMap["long  double"]		= "LONGDOUBLE"
gCDRMap["long  long"]		= "LONGLONG"
gCDRMap["octet"]		= "octet"
gCDRMap["short"]		= "short"
gCDRMap["string"]		= "char  *"
gCDRMap["unsigned  long"]	= "unsigned  long"
gCDRMap["unsigned  long  long"]	= "ULONGLONG"
gCDRMap["unsigned  short"]	= "unsigned  short"
gCDRMap["wchar"]		= "wchar_t"
gCDRMap["wstring"]		= "wchar_t  *"


global gAliasMap
	# Sequences of basic CORBA types.
gAliasMap["sequence<boolean>"]			= "BooleanSeq"
gAliasMap["sequence<char>"]			= "CharSeq"
gAliasMap["sequence<double>"]			= "DoubleSeq"
gAliasMap["sequence<float>"]			= "FloatSeq"
gAliasMap["sequence<long>"]			= "LongSeq"
gAliasMap["sequence<long  double>"]		= "LongDoubleSeq"
gAliasMap["sequence<long  long>"]		= "LongLongSeq"
gAliasMap["sequence<octet>"]			= "OctetSeq"
gAliasMap["sequence<short>"]			= "ShortSeq"
gAliasMap["sequence<string>"]			= "StringSeq"
gAliasMap["sequence<unsigned  long>"]		= "ULongSeq"
gAliasMap["sequence<unsigned  long  long>"]	= "ULongLongSeq"
gAliasMap["sequence<unsigned  short>"]		= "UShortSeq"
gAliasMap["sequence<wchar>"]			= "WCharSeq"
gAliasMap["sequence<wstring>"]			= "WStringSeq"
	# Sequences of equivalent types.
gAliasMap["sequence<bool>"]			= "BooleanSeq"
gAliasMap["sequence<char  *>"]			= "StringSeq"
gAliasMap["sequence<wchar_t>"]			= "WCharSeq"
gAliasMap["sequence<wchar_t  *>"]		= "WStringSeq"

global gTypeMap
gTypeMap["Version"]		= "None"
gTypeMap["BooleanSeq"]		= "None"
gTypeMap["CharSeq"]		= "None"
gTypeMap["DoubleSeq"]		= "None"
gTypeMap["FloatSeq"]		= "None"
gTypeMap["LongSeq"]		= "None"
gTypeMap["LongDoubleSeq"]	= "None"
gTypeMap["LongLongSeq"]		= "None"
gTypeMap["OctetSeq"]		= "None"
gTypeMap["ShortSeq"]		= "None"
gTypeMap["StringSeq"]		= "None"
gTypeMap["ULongSeq"]		= "None"
gTypeMap["ULongLongSeq"]	= "None"
gTypeMap["UShortSeq"]		= "None"
gTypeMap["WCharSeq"]		= "None"
gTypeMap["WStringSeq"]		= "None"

global gMarshalMap
	# Basic CORBA types.
gMarshalMap["bool"]		= "comxBoolean"
gMarshalMap["char"]		= "comxChar"
gMarshalMap["double"]		= "comxDouble"
gMarshalMap["float"]		= "comxFloat"
gMarshalMap["long"]		= "comxLong"
gMarshalMap["LONGDOUBLE"]	= "comxLongDouble"
gMarshalMap["LONGLONG"]		= "comxLongLong"
gMarshalMap["octet"]		= "comxOctet"
gMarshalMap["short"]		= "comxShort"
gMarshalMap["char  *"]		= "comxString"
gMarshalMap["unsigned  long"]	= "comxULong"
gMarshalMap["ULONGLONG"]	= "comxULongLong"
gMarshalMap["unsigned  short"]	= "comxUShort"
gMarshalMap["Version"]		= "comxVersion"
gMarshalMap["wchar_t"]		= "comxWChar"
gMarshalMap["wchar_t  *"]	= "comxWString"
	# Sequences of basic types.
gMarshalMap["BooleanSeq"]	= "comxBooleanSeq"
gMarshalMap["CharSeq"]		= "comxCharSeq"
gMarshalMap["DoubleSeq"]	= "comxDoubleSeq"
gMarshalMap["FloatSeq"]		= "comxFloatSeq"
gMarshalMap["LongSeq"]		= "comxLongSeq"
gMarshalMap["LongDoubleSeq"]	= "comxLongDoubleSeq"
gMarshalMap["LongLongSeq"]	= "comxLongLongSeq"
gMarshalMap["OctetSeq"]		= "comxOctetSeq"
gMarshalMap["ShortSeq"]		= "comxShortSeq"
gMarshalMap["StringSeq"]	= "comxStringSeq"
gMarshalMap["ULongSeq"]		= "comxULongSeq"
gMarshalMap["ULongLongSeq"]	= "comxULongLongSeq"
gMarshalMap["UShortSeq"]	= "comxUShortSeq"
gMarshalMap["WCharSeq"]		= "comxWCharSeq"
gMarshalMap["WStringSeq"]	= "comxWStringSeq"