/**
 * transwrap.c - wrapper for ICU 2.0 transliteration services
 *
 * ===================================
 * CONVENTIONS ABOUT MEMORY MANAGEMENT
 * ===================================
 *
 * Within functions:
 *
 *   . A call to a function that allocates something, must be marked as
 *     a comment saying MALLOC
 *
 *   . A call to a function that frees something, must be marked as
 *     a comment saying FREE
 *
 *   . All functions should have as many MALLOC as FREE comments
 *
 *   . If a function name ends with _MALLOC, then we have to
 *     add a FREE comment at the end of the function
 *
 *   . If a function name ends with _FREE, then we have to
 *     add a MALLOC comment at the beginning of the function
 *
 *   Sorry for being so paranoid, I don't want my mod_perl processes to
 *   get bigger than they are :-)
 **/
#include "stdlib.h"
#include "stdio.h"
#include <unicode/urep.h>
#include <unicode/utypes.h>
#include <unicode/utrans.h>
#include <unicode/utf.h>


U_CAPI char* U_EXPORT2 utf8_transliterate_MALLOC (const char* id, const UTransDirection dir, const char* utf8_string,  int* err_PTR);
U_CAPI UChar* U_EXPORT2 utf16_transliterate_MALLOC (const char* id, const UTransDirection dir, const UChar* string, int* err_PTR);
U_CAPI UChar* U_EXPORT2 utf16_transliterate_useTransliterator_MALLOC (const UTransliterator* transliterator, const UChar* string, int* err_PTR);
U_CAPI UTransliterator* U_EXPORT2 utf16_transliterate_openTransliterator_MALLOC (const char* id, const UTransDirection dir, int* err_PTR);
U_CAPI UChar* U_EXPORT2 convert_utf8_to_utf16_MALLOC (const char* src, int* err_PTR);
U_CAPI char* U_EXPORT2 convert_utf16_to_utf8_MALLOC (const UChar* src, int* err_PTR);
U_CAPI void U_EXPORT2 checkErrorCode (int* errorCode_PTR);


/*
int main()
{
  int   errorCode       = U_ZERO_ERROR;
  int*  errorCode_PTR   = (int*) &errorCode;
  char* orig            = "Compyutaa";
  char* orig_trans      = NULL;
  char* orig_trans_orig = NULL;
  
  orig_trans = utf8_transliterate_MALLOC ("Latin-Katakana", UTRANS_FORWARD, orig, errorCode_PTR);
  if (errorCode_PTR > U_ZERO_ERROR) checkErrorCode (errorCode_PTR);
  errorCode = U_ZERO_ERROR;
  
  orig_trans_orig = utf8_transliterate_MALLOC ("Latin-Katakana", UTRANS_REVERSE, orig_trans, errorCode_PTR);
  if (errorCode_PTR > U_ZERO_ERROR) checkErrorCode (errorCode_PTR);
  errorCode = U_ZERO_ERROR;
  
  printf ("%s : %s : %s", orig, orig_trans, orig_trans_orig);
}
*/



/**
 * Transliterates string using transliterator with id 'id' using direction dir
 *
 * @param id - transliterator string identifier
 * @param dir - transliterator direction, can be UTRANS_FORWARD or UTRANS_REVERSE
 * @param string - UTF-8 string that needs be transliterated
 * @param err_PTR - pointer to error code, must be initialized to U_ZERO_ERROR
 * @returns - UTF-16 transliterated newly allocated string
 **/
U_CAPI char* U_EXPORT2 utf8_transliterate_MALLOC (const char* id, const UTransDirection dir, const char* utf8_string,  int* err_PTR)
{
  UChar* utf16_string  = NULL;
  UChar* utf16_result  = NULL;
  char*  utf8_result   = NULL;
  int    errorCode     = U_ZERO_ERROR;
  int*   errorCode_PTR = &errorCode;
  
  /* MALLOC <utf16_string> */
  /* Converts string into UTF-16 */
  utf16_string = convert_utf8_to_utf16_MALLOC (utf8_string, errorCode_PTR);
  if (errorCode > U_ZERO_ERROR) {
    *err_PTR = errorCode;
    fprintf (stderr, "utf8_transliterate_MALLOC: convert_utf8_to_utf16_MALLOC\n");
    if (utf16_string != NULL) free (utf16_string);
    return NULL;
  }
  errorCode = U_ZERO_ERROR;
  
  /* MALLOC <utf16_result> */
  /* Transliterates utf16_string into result */
  utf16_result = utf16_transliterate_MALLOC (id, dir, utf16_string, errorCode_PTR);
  if (errorCode > U_ZERO_ERROR) {
    *err_PTR = errorCode;
    fprintf (stderr, "utf8_transliterate_MALLOC: utf16_transliterate_MALLOC\n");
    if (utf16_string != NULL) free (utf16_string);
    if (utf16_result != NULL) free (utf16_result);
    return NULL;
  }
  errorCode = U_ZERO_ERROR;
  
  /* MALLOC <utf8_result> */
  utf8_result = convert_utf16_to_utf8_MALLOC (utf16_result, errorCode_PTR);
  if (errorCode > U_ZERO_ERROR) {
    fprintf (stderr, "utf8_transliterate_MALLOC: convert_utf16_to_utf8_MALLOC\n");
    if (utf16_string != NULL) free (utf16_string);
    if (utf16_result != NULL) free (utf16_result);
    if (utf8_result  != NULL) free (utf8_result);
    return NULL;
  }
  errorCode = U_ZERO_ERROR;
  
  /* FREE <utf16_string> */
  free (utf16_string);
  
  /* FREE <utf16_result> */
  free (utf16_result);
  
  /* FREE <utf8_result> (delayed) */
  return utf8_result;
}


/**
 * Transliterates string using transliterator with id 'id' using direction dir
 *
 * @param id - transliterator string identifier
 * @param dir - transliterator direction, can be UTRANS_FORWARD or UTRANS_REVERSE
 * @param string - UTF-16 string that needs be transliterated
 * @param err_PTR - pointer to error code, must be initialized to U_ZERO_ERROR
 * @returns - UTF-16 transliterated newly allocated string
 **/
U_CAPI UChar* U_EXPORT2 utf16_transliterate_MALLOC (const char* id, const UTransDirection dir, const UChar* string, int* err_PTR)
{
  UChar*           result         = NULL;
  int              errorCode      = U_ZERO_ERROR;
  int*             errorCode_PTR  = &errorCode;
  UTransliterator* transliterator = NULL;
  
  /* MALLOC <transliterator> */
  /* opens transliterator */
  transliterator = utf16_transliterate_openTransliterator_MALLOC (id, dir, errorCode_PTR);
  if (errorCode > U_ZERO_ERROR) {
    *err_PTR = errorCode;
    fprintf (stderr, "utf16_transliterate_MALLOC: utf16_transliterate_openTransliterator_MALLOC\n");
    if (transliterator != NULL) free (transliterator);
    return NULL;
  }
  
  /* MALLOC <result> */
  /* transliterates the result */
  result = utf16_transliterate_useTransliterator_MALLOC (transliterator, string, err_PTR);
  if (*err_PTR > U_ZERO_ERROR) {
    fprintf (stderr, "utf16_transliterate_MALLOC: utf16_transliterate_useTransliterator_MALLOC\n");
    if (transliterator != NULL) free (transliterator);
    if (result != NULL) free (result);
    return NULL;
  }
  
  /* FREE <transliterator> */
  /* closes transliterator */
  utrans_close (transliterator);
  
  return result;
  /* FREE <result> (delayed) */
}


/**
 * Processes string using transliterator, and returns a newly allocated UChar* string
 *
 * @param transliterator - The transliterator to use
 * @param string - The string to transliterate
 * @returns - The transliterated string
 **/
U_CAPI UChar* U_EXPORT2 utf16_transliterate_useTransliterator_MALLOC (const UTransliterator* transliterator, const UChar* string, int* err_PTR)
{
  UChar*   text           = NULL;
  int32_t  textLength     = -1;
  int32_t* textLength_PTR = &textLength;
  int32_t  textCapacity   = 0;
  int32_t  start          = 0;
  int32_t  limit          = u_strlen (string);
  int32_t* limit_PTR      = &limit;
  int      errorCode      = U_ZERO_ERROR;
  int*     errorCode_PTR  = &errorCode;
  
  errorCode = U_BUFFER_OVERFLOW_ERROR;
  
  /*
    As long as we have not allocated a buffer which
    is large enough, let's try to reallocate a bigger
    buffer
    
    MALLOC <result>
  */
  while (errorCode == U_BUFFER_OVERFLOW_ERROR) {
    
    errorCode = U_ZERO_ERROR;
    if (textCapacity == 0) textCapacity = u_strlen (string) + 1;
    else                   textCapacity *= 2;
    
    /* MALLOC <text> */
    /* performs some initialization */
    if (text != NULL) free (text);
    text = (UChar*) malloc (textCapacity * sizeof (UChar));
    u_strcpy (text, string);
    
    /* performs the UChar transliteration */
    utrans_transUChars ( transliterator,
			 text,
			 textLength_PTR,
			 textCapacity,
			 start,
			 limit_PTR,
			 errorCode_PTR );
    
    /* We want the string to be zero-terminated */
    if (textCapacity == *textLength_PTR) errorCode = U_BUFFER_OVERFLOW_ERROR;
  }
  
  *err_PTR = U_ZERO_ERROR;
  if (errorCode != U_ZERO_ERROR) {
    free (text);
    text = NULL;
  }

  return text;
  /* FREE <text> (delayed) */
}


/**
 * Allocates and returns a new transliterator
 *
 * @param id - Transliterator ID to open
 * @param dir - Direction, which can be UTRANS_FORWARD or UTRANS_REVERSE
 * @param err_PTR - a pointer to an int value
 * @returns a newly allocated UChar* String that need be freed at some point
 **/
U_CAPI UTransliterator* U_EXPORT2 utf16_transliterate_openTransliterator_MALLOC (const char* id, const UTransDirection dir, int* err_PTR)
{
  int              errorCode      = U_ZERO_ERROR;
  int*             errorCode_PTR  = (int*) &errorCode;
  UTransliterator* transliterator = NULL;
  
  /* Attempts to open the transliterator */
  /* MALLOC <transliterator> */
  transliterator = utrans_open ( id,
				 dir,
				 NULL,
				 -1,
				 NULL,
				 errorCode_PTR );
  
  /* If the open failed, set error code and return NULL */
  *err_PTR = errorCode;
  if (errorCode > U_ZERO_ERROR) {
    if (transliterator != NULL) free (transliterator);
    return NULL;
  }
  
  return transliterator;
  /* FREE <transliterator> (delayed) */
}


/**
 * Converts a char* UTF-8 string into a UChar* UTF-16 string
 *
 * @param src - the UTF-8 const char* string to convert
 * @param err_PTR - a pointer to an int value
 * @returns a newly allocated UChar* UTF-16 String
 **/
U_CAPI UChar* U_EXPORT2 convert_utf8_to_utf16_MALLOC (const char* src, int* err_PTR)
{
  UChar*   result         = NULL;
  int32_t  destCapacity   = 0;
  int32_t  destLength     = 0;
  int32_t* destLength_PTR = &destLength;
  int32_t  srcLength      = -1;
  int      errorCode      = U_ZERO_ERROR;
  int*     errorCode_PTR  = (int*) &errorCode; 
  
  errorCode = U_BUFFER_OVERFLOW_ERROR;
  
  /*
    As long as we have not allocated a buffer which
    is large enough, let's try to reallocate a bigger
    buffer
    
    MALLOC <result>
  */
  while (errorCode == U_BUFFER_OVERFLOW_ERROR) {
    
    errorCode = U_ZERO_ERROR;
    if (destCapacity == 0) destCapacity = u_strlen (src) + 1;
    else                   destCapacity *= 2;
    
    destLength   = 0;
    if (result != NULL) free (result);
    result = (UChar*) malloc ((destCapacity) * sizeof (UChar));
    
    /* Converts string to UTF-8 */
    u_strFromUTF8 ( result,
		    destCapacity,
		    destLength_PTR,
		    src,
		    srcLength,
		    errorCode_PTR );
    
    /* We want the string to be zero-terminated */
    if (destCapacity == *destLength_PTR) errorCode = U_BUFFER_OVERFLOW_ERROR;
  }
  
  *err_PTR = errorCode;
  if (errorCode != U_ZERO_ERROR) {
    free (result);
    result = NULL;
  }
  
  return result;
  /* FREE <result> (delayed) */
}


/**
 * Converts a UChar* UTF-16 string into a char* UTF-8 string
 *
 * @param src - the UTF-16 const UCHar* string to convert
 * @param err_PTR - a pointer to an int value
 * @returns a newly allocated char* UTF-8 string 
 **/
U_CAPI char* U_EXPORT2 convert_utf16_to_utf8_MALLOC (const UChar* src, int* err_PTR)
{
  char*    result         = NULL;
  int32_t  destCapacity   = 0;
  int32_t  destLength     = 0;
  int32_t* destLength_PTR = &destLength;
  int32_t  srcLength      = -1;
  int      errorCode      = U_ZERO_ERROR;
  int*     errorCode_PTR  = (int*) &errorCode;
  
  errorCode = U_BUFFER_OVERFLOW_ERROR;
  
  /*
    As long as we have not allocated a buffer which
    is large enough, let's try to reallocate a bigger
    buffer
    
    MALLOC <result>
  */
  while (errorCode == U_BUFFER_OVERFLOW_ERROR) {
    
    errorCode = U_ZERO_ERROR;
    if (destCapacity == 0) destCapacity = u_strlen (src) + 1;
    else                   destCapacity *= 2;
    
    destLength   = 0;
    if (result != NULL) free (result);
    result = (char*) malloc ((destCapacity) * sizeof (char));
    
    /* Converts string to UTF-8 */
    u_strToUTF8 ( result,
		  destCapacity,
		  destLength_PTR,
		  src,
		  srcLength,
		  errorCode_PTR );

    /* We want the string to be zero-terminated */
    if (destCapacity == *destLength_PTR) errorCode = U_BUFFER_OVERFLOW_ERROR;
  }
  
  *err_PTR = errorCode;
  if (errorCode != U_ZERO_ERROR) {
    free (result);
    result = NULL;
  }
  
  return result;
  /* FREE <result> (delayed) */
}


/**
 * Just a bit of debug...
 **/
U_CAPI void U_EXPORT2 checkErrorCode (int* errorCode_PTR)
{
  int errorCode = *errorCode_PTR;  
  switch (errorCode)
    {
      /** Warnings **/
    case U_USING_FALLBACK_WARNING:
      fprintf (stderr, "%d: U_USING_FALLBACK_WARNING\n", U_USING_FALLBACK_WARNING);
      break;

      /* duplicate case value      
    case U_USING_FALLBACK_ERROR:
      fprintf (stderr, "%d: U_USING_FALLBACK_ERROR\n", U_USING_FALLBACK_ERROR);
      break;
      */
      
      /* duplicate case value            
    case U_ERROR_WARNING_START:
      fprintf (stderr, "%d: U_ERROR_WARNING_START\n", U_ERROR_WARNING_START);
      break;
      */
      
      /* duplicate case value            
    case U_ERROR_INFO_START:
      fprintf (stderr, "%d: U_ERROR_INFO_START\n", U_ERROR_INFO_START);
      break;
      */
      
    case U_USING_DEFAULT_WARNING:
      fprintf (stderr, "%d: U_USING_DEFAULT_WARNING\n", U_USING_DEFAULT_WARNING);
      break;
      
      /* duplicate case value            
    case U_USING_DEFAULT_ERROR:
      fprintf (stderr, "%d: U_USING_DEFAULT_ERROR\n", U_USING_DEFAULT_ERROR);
      break;
      */
      
    case U_SAFECLONE_ALLOCATED_WARNING:
      fprintf (stderr, "%d: U_SAFECLONE_ALLOCATED_WARNING\n", U_SAFECLONE_ALLOCATED_WARNING);
      break;
      
      /* duplicate case value                 
    case U_SAFECLONE_ALLOCATED_ERROR:
      fprintf (stderr, "%d: U_SAFECLONE_ALLOCATED_ERROR\n", U_SAFECLONE_ALLOCATED_ERROR);
      break;
      */
      
    case U_STATE_OLD_WARNING:
      fprintf (stderr, "%d: U_STATE_OLD_WARNING\n", U_STATE_OLD_WARNING);
      break;
      
    case U_STRING_NOT_TERMINATED_WARNING:
      fprintf (stderr, "%d: U_STRING_NOT_TERMINATED_WARNING\n", U_STRING_NOT_TERMINATED_WARNING);
      break;
    
      /** Huh, we probably should not have a warning if there's no error, but for the moment... **/
    case U_ZERO_ERROR: 
      /* fprintf (stderr, "%d: U_ZERO_ERROR\n", U_ZERO_ERROR); */
      break;
      
      /** Fatal Errors **/
    case U_ILLEGAL_ARGUMENT_ERROR:
      fprintf (stderr, "%d: U_ILLEGAL_ARGUMENT_ERROR\n", U_ILLEGAL_ARGUMENT_ERROR);
      break;
      
    case U_MISSING_RESOURCE_ERROR:
      fprintf (stderr, "%d: U_MISSING_RESOURCE_ERROR\n", U_MISSING_RESOURCE_ERROR);
      break;

    case U_INVALID_FORMAT_ERROR:
      fprintf (stderr, "%d: U_INVALID_FORMAT_ERROR\n", U_INVALID_FORMAT_ERROR);
      break;

    case U_FILE_ACCESS_ERROR:
      fprintf (stderr, "%d: U_FILE_ACCESS_ERROR\n", U_FILE_ACCESS_ERROR);
      break;
      
    case U_INTERNAL_PROGRAM_ERROR:
      fprintf (stderr, "%d: U_INTERNAL_PROGRAM_ERROR\n", U_INTERNAL_PROGRAM_ERROR);
      break;
      
    case U_MESSAGE_PARSE_ERROR:
      fprintf (stderr, "%d: U_MESSAGE_PARSE_ERROR\n", U_MESSAGE_PARSE_ERROR);
      break;
      
    case U_MEMORY_ALLOCATION_ERROR:
      fprintf (stderr, "%d: U_MEMORY_ALLOCATION_ERROR\n", U_MEMORY_ALLOCATION_ERROR);
      break;
      
    case U_INDEX_OUTOFBOUNDS_ERROR:
      fprintf (stderr, "%d: U_INDEX_OUTOFBOUNDS_ERROR\n", U_INDEX_OUTOFBOUNDS_ERROR);
      break;
      
    case U_PARSE_ERROR:
      fprintf (stderr, "%d: U_PARSE_ERROR\n", U_PARSE_ERROR);
      break;
      
    case U_INVALID_CHAR_FOUND:
      fprintf (stderr, "%d: U_INVALID_CHAR_FOUND\n", U_INVALID_CHAR_FOUND);
      break;

    case U_TRUNCATED_CHAR_FOUND:
      fprintf (stderr, "%d: U_TRUNCATED_CHAR_FOUND\n", U_TRUNCATED_CHAR_FOUND);
      break;
      
    case U_ILLEGAL_CHAR_FOUND:
      fprintf (stderr, "%d: U_ILLEGAL_CHAR_FOUND\n", U_ILLEGAL_CHAR_FOUND);
      break;
      
    case U_INVALID_TABLE_FORMAT:
      fprintf (stderr, "%d: U_INVALID_TABLE_FORMAT\n", U_INVALID_TABLE_FORMAT);
      break;
      
    case U_INVALID_TABLE_FILE:
      fprintf (stderr, "%d: U_INVALID_TABLE_FILE\n", U_INVALID_TABLE_FILE);
      break;
      
    case U_BUFFER_OVERFLOW_ERROR:
      fprintf (stderr, "%d: U_BUFFER_OVERFLOW_ERROR\n", U_BUFFER_OVERFLOW_ERROR);
      break;
      
    case U_UNSUPPORTED_ERROR:
      fprintf (stderr, "%d: U_UNSUPPORTED_ERROR\n", U_UNSUPPORTED_ERROR);
      break;
      
    case U_RESOURCE_TYPE_MISMATCH:
      fprintf (stderr, "%d: U_RESOURCE_TYPE_MISMATCH\n", U_RESOURCE_TYPE_MISMATCH);
      break;
      
    case U_ILLEGAL_ESCAPE_SEQUENCE:
      fprintf (stderr, "%d: U_ILLEGAL_ESCAPE_SEQUENCE\n", U_ILLEGAL_ESCAPE_SEQUENCE);
      break;
      
    case U_UNSUPPORTED_ESCAPE_SEQUENCE:
      fprintf (stderr, "%d: U_UNSUPPORTED_ESCAPE_SEQUENCE\n", U_UNSUPPORTED_ESCAPE_SEQUENCE);
      break;
      
    case U_NO_SPACE_AVAILABLE:
      fprintf (stderr, "%d: U_NO_SPACE_AVAILABLE\n", U_NO_SPACE_AVAILABLE);
      break;
      
    case U_CE_NOT_FOUND_ERROR:
      fprintf (stderr, "%d: U_CE_NOT_FOUND_ERROR\n", U_CE_NOT_FOUND_ERROR);
      break;
      
    case U_PRIMARY_TOO_LONG_ERROR:
      fprintf (stderr, "%d: U_PRIMARY_TOO_LONG_ERROR\n", U_PRIMARY_TOO_LONG_ERROR);
      break;
      
    case U_STATE_TOO_OLD_ERROR:
      fprintf (stderr, "%d: U_STATE_TOO_OLD_ERROR\n", U_STATE_TOO_OLD_ERROR);
      break;
      
    case U_BAD_VARIABLE_DEFINITION:
      fprintf (stderr, "%d: U_BAD_VARIABLE_DEFINITION\n", U_BAD_VARIABLE_DEFINITION);
      break;
      
    /** Transliterator specific parse errors **/

      /* duplicate case value
    case U_PARSE_ERROR_START:
      fprintf (stderr, "%d: U_PARSE_ERROR_START\n", U_PARSE_ERROR_START);
      break;
      */
      
    case U_MALFORMED_RULE:
      fprintf (stderr, "%d: U_MALFORMED_RULE\n", U_MALFORMED_RULE);
      break;
      
    case U_MALFORMED_SET:
      fprintf (stderr, "%d: U_MALFORMED_SET\n", U_MALFORMED_SET);
      break;
      
    case U_MALFORMED_SYMBOL_REFERENCE:
      fprintf (stderr, "%d: U_MALFORMED_SYMBOL_REFERENCE\n", U_MALFORMED_SYMBOL_REFERENCE);
      break;
      
    case U_MALFORMED_UNICODE_ESCAPE:
      fprintf (stderr, "%d: U_MALFORMED_UNICODE_ESCAPE\n", U_MALFORMED_UNICODE_ESCAPE);
      break;
      
    case U_MALFORMED_VARIABLE_DEFINITION:
      fprintf (stderr, "%d: U_MALFORMED_VARIABLE_DEFINITION\n", U_MALFORMED_VARIABLE_DEFINITION);
      break;
      
    case U_MALFORMED_VARIABLE_REFERENCE:
      fprintf (stderr, "%d: U_MALFORMED_VARIABLE_REFERENCE\n", U_MALFORMED_VARIABLE_REFERENCE);
      break;
      
    case U_MISMATCHED_SEGMENT_DELIMITERS:
      fprintf (stderr, "%d: U_MISMATCHED_SEGMENT_DELIMITERS\n", U_MISMATCHED_SEGMENT_DELIMITERS);
      break;
      
    case U_MISPLACED_ANCHOR_START:
      fprintf (stderr, "%d: U_MISPLACED_ANCHOR_START\n", U_MISPLACED_ANCHOR_START);
      break;
      
    case U_MISPLACED_CURSOR_OFFSET:
      fprintf (stderr, "%d: U_MISPLACED_CURSOR_OFFSET\n", U_MISPLACED_CURSOR_OFFSET);
      break;
      
    case U_MISPLACED_QUANTIFIER:
      fprintf (stderr, "%d: U_MISPLACED_QUANTIFIER\n", U_MISPLACED_QUANTIFIER);
      break;
      
    case U_MISSING_OPERATOR:
      fprintf (stderr, "%d: U_MISSING_OPERATOR\n", U_MISSING_OPERATOR);
      break;

    case U_MISSING_SEGMENT_CLOSE:
      fprintf (stderr, "%d: U_MISSING_SEGMENT_CLOSE\n", U_MISSING_SEGMENT_CLOSE);
      break;
      
    case U_MULTIPLE_ANTE_CONTEXTS:
      fprintf (stderr, "%d: U_MULTIPLE_ANTE_CONTEXTS\n", U_MULTIPLE_ANTE_CONTEXTS);
      break;
      
    case U_MULTIPLE_CURSORS:
      fprintf (stderr, "%d: U_MULTIPLE_CURSORS\n", U_MULTIPLE_CURSORS);
      break;
      
    case U_MULTIPLE_POST_CONTEXTS:
      fprintf (stderr, "%d: U_MULTIPLE_POST_CONTEXTS\n", U_MULTIPLE_POST_CONTEXTS);
      break;
      
    case U_TRAILING_BACKSLASH:
      fprintf (stderr, "%d: U_TRAILING_BACKSLASH\n", U_TRAILING_BACKSLASH);
      break;
      
    case U_UNDEFINED_SEGMENT_REFERENCE:
      fprintf (stderr, "%d: U_UNDEFINED_SEGMENT_REFERENCE\n", U_UNDEFINED_SEGMENT_REFERENCE);
      break;
      
    case U_UNDEFINED_VARIABLE:
      fprintf (stderr, "%d: U_UNDEFINED_VARIABLE\n", U_UNDEFINED_VARIABLE);
      break;
      
    case U_UNQUOTED_SPECIAL:
      fprintf (stderr, "%d: U_UNQUOTED_SPECIAL\n", U_UNQUOTED_SPECIAL);
      break;
      
    case U_UNTERMINATED_QUOTE:
      fprintf (stderr, "%d: U_UNTERMINATED_QUOTE\n", U_UNTERMINATED_QUOTE);
      break;
      
    case U_RULE_MASK_ERROR:
      fprintf (stderr, "%d: U_RULE_MASK_ERROR\n", U_RULE_MASK_ERROR);
      break;
      
    case U_MISPLACED_COMPOUND_FILTER:
      fprintf (stderr, "%d: U_MISPLACED_COMPOUND_FILTER\n", U_MISPLACED_COMPOUND_FILTER);
      break;
      
    case U_MULTIPLE_COMPOUND_FILTERS:
      fprintf (stderr, "%d: U_MULTIPLE_COMPOUND_FILTERS\n", U_MULTIPLE_COMPOUND_FILTERS);
      break;
      
    case U_INVALID_RBT_SYNTAX:
      fprintf (stderr, "%d: U_INVALID_RBT_SYNTAX\n", U_INVALID_RBT_SYNTAX);
      break;
      
    case U_INVALID_PROPERTY_PATTERN:
      fprintf (stderr, "%d: U_INVALID_PROPERTY_PATTERN\n", U_INVALID_PROPERTY_PATTERN);
      break;
      
    case U_MALFORMED_PRAGMA:
      fprintf (stderr, "%d: U_MALFORMED_PRAGMA\n", U_MALFORMED_PRAGMA);
      break;
      
    case U_UNCLOSED_SEGMENT:
      fprintf (stderr, "%d: U_UNCLOSED_SEGMENT\n", U_UNCLOSED_SEGMENT);
      break;
      
    case U_ILLEGAL_CHAR_IN_SEGMENT:
      fprintf (stderr, "%d: U_ILLEGAL_CHAR_IN_SEGMENT\n", U_ILLEGAL_CHAR_IN_SEGMENT);
      break;
      
    case U_VARIABLE_RANGE_EXHAUSTED:
      fprintf (stderr, "%d: U_VARIABLE_RANGE_EXHAUSTED\n", U_VARIABLE_RANGE_EXHAUSTED);
      break;
      
    case U_VARIABLE_RANGE_OVERLAP:
      fprintf (stderr, "%d: U_VARIABLE_RANGE_OVERLAP\n", U_VARIABLE_RANGE_OVERLAP);
      break;
      
    case U_PARSE_ERROR_LIMIT:
      fprintf (stderr, "%d: U_PARSE_ERROR_LIMIT\n", U_PARSE_ERROR_LIMIT);
      break;
      
    case U_UNEXPECTED_TOKEN:      fprintf (stderr, "%d: U_UNEXPECTED_TOKEN\n", U_UNEXPECTED_TOKEN);
      break;
      
      /* duplicate case value
    case U_FMT_PARSE_ERROR_START:
      fprintf (stderr, "%d: U_FMT_PARSE_ERROR_START\n", U_FMT_PARSE_ERROR_START);
      break;
      */

    case U_MULTIPLE_DECIMAL_SEPERATORS:
      fprintf (stderr, "%d: U_MULTIPLE_DECIMAL_SEPERATORS\n", U_MULTIPLE_DECIMAL_SEPERATORS);
      break;

    case U_MULTIPLE_EXPONENTIAL_SYMBOLS:
      fprintf (stderr, "%d: U_MULTIPLE_EXPONENTIAL_SYMBOLS\n", U_MULTIPLE_EXPONENTIAL_SYMBOLS);
      break;

    case U_MALFORMED_EXPONENTIAL_PATTERN:
      fprintf (stderr, "%d: U_MALFORMED_EXPONENTIAL_PATTERN\n", U_MALFORMED_EXPONENTIAL_PATTERN);
      break;

    case U_MULTIPLE_PERCENT_SYMBOLS:
      fprintf (stderr, "%d: U_MULTIPLE_PERCENT_SYMBOLS\n", U_MULTIPLE_PERCENT_SYMBOLS);
      break;

    case U_MULTIPLE_PERMILL_SYMBOLS:
      fprintf (stderr, "%d: U_MULTIPLE_PERMILL_SYMBOLS\n", U_MULTIPLE_PERMILL_SYMBOLS);
      break;

    case U_MULTIPLE_PAD_SPECIFIERS:
      fprintf (stderr, "%d: U_MULTIPLE_PAD_SPECIFIERS\n", U_MULTIPLE_PAD_SPECIFIERS);
      break;

    case U_PATTERN_SYNTAX_ERROR:
      fprintf (stderr, "%d: U_PATTERN_SYNTAX_ERROR\n", U_PATTERN_SYNTAX_ERROR);
      break;

    case U_ILLEGAL_PAD_POSITION:
      fprintf (stderr, "%d: U_ILLEGAL_PAD_POSITION\n", U_ILLEGAL_PAD_POSITION);
      break;

    case U_UNMATCHED_BRACES:
      fprintf (stderr, "%d: U_UNMATCHED_BRACES\n", U_UNMATCHED_BRACES);
      break;

    case U_UNSUPPORTED_PROPERTY:
      fprintf (stderr, "%d: U_UNSUPPORTED_PROPERTY\n", U_UNSUPPORTED_PROPERTY);
      break;

    case U_UNSUPPORTED_ATTRIBUTE:
      fprintf (stderr, "%d: U_UNSUPPORTED_ATTRIBUTE\n", U_UNSUPPORTED_ATTRIBUTE);
      break;
    }
}




